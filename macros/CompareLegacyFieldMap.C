// Compare legacy MDMTrace focal-plane output to the Geant4 field-map track.
//
// Usage from the repository root:
//   root -l -b -q 'macros/CompareLegacyFieldMap.C("build/SimData~99.root")'
//
// Source-selection examples:
//   // Auto: use LegacyFocalPlaneHit* if available, otherwise PPAC projection.
//   root -l -b -q 'macros/CompareLegacyFieldMap.C("SimData~93.root")'
//
//   // Force the vacuum legacy focal-plane scorer.
//   root -l -b -q 'macros/CompareLegacyFieldMap.C("SimData~93.root","compare_focal.root",-600,-489.99875,-89.99875,"AccurateData","legacy")'
//
//   // Force projection from PPAC1/PPAC2 hit positions.
//   root -l -b -q 'macros/CompareLegacyFieldMap.C("SimData~93.root","compare_ppac.root",-600,-489.99875,-89.99875,"AccurateData","ppac")'
//
// The legacy plane is expressed in the PPAC chamber local z coordinate, in mm.
// In the current geometry the PPAC chamber center is placed 60 cm downstream of
// the legacy focal plane, so the default comparison plane is z = -600 mm.
//
// New MdmSim files store a vacuum LegacyFocalPlane hit at the RAYTRACE focal
// plane. By default the macro uses that branch when present, avoiding PPAC
// gas/window/cathode scattering in the comparison. The final macro argument can
// force either "legacy" focal-plane hits or "ppac" projection. Older files are
// still handled by projecting the PPAC1/PPAC2 track back to the same plane.
//
// The stored PPAC local z values are local to each cathode volume, not to the
// PPAC chamber. The PPAC fallback therefore uses explicit chamber-local PPAC
// plane z values. Current defaults:
//   PPAC1 cathode: -500 mm + 2.5 um / 2 + 10 mm = -489.99875 mm
//   PPAC2 cathode: PPAC1 + 400 mm = -89.99875 mm
//
// PPAC local X is chamber-centered. In the current detector placement the
// chamber center lies on the legacy D-axis, so no empirical X offset is applied.
// The DIPO "Drawing Exit" coefficients in rayin.dat are EFB curvature terms
// (S12..S18), not coordinate-origin shifts. If a future deck uses nonzero
// DIPO XCR2, handle that as geometry, not as a fitted comparison offset.

#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLine.h>
#include <TMath.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TTree.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

template <typename T>
bool HasValue(const std::vector<T>* values) {
  return values && !values->empty();
}

double FirstOrNaN(const std::vector<double>* values) {
  return HasValue(values) ? values->front() : std::numeric_limits<double>::quiet_NaN();
}

double AtOrNaN(const std::vector<double>* values, std::size_t index) {
  return values && index < values->size() ? values->at(index) : std::numeric_limits<double>::quiet_NaN();
}

int FirstTrackOr(const std::vector<int>* values, int fallback) {
  return values && !values->empty() ? values->front() : fallback;
}

std::size_t IndexForTrack(const std::vector<int>* trackIds, int trackId) {
  if (!trackIds) {
    return 0;
  }
  for (std::size_t i = 0; i < trackIds->size(); ++i) {
    if (trackIds->at(i) == trackId) {
      return i;
    }
  }
  return 0;
}

struct CompareRow {
  double legacyX = 0.0;
  double legacyY = 0.0;
  double legacyAngleX = 0.0;
  double legacyAngleY = 0.0;
  double fieldX = 0.0;
  double fieldY = 0.0;
  double fieldAngleX = 0.0;
  double fieldAngleY = 0.0;
};

struct Quantity {
  const char* key;
  const char* title;
  const char* unit;
  std::function<double(const CompareRow&)> legacyValue;
  std::function<double(const CompareRow&)> fieldValue;
};

enum class FieldMapSourceMode {
  Auto,
  LegacyFocalPlane,
  PpacProjection,
};

std::string Lowercase(const char* value) {
  std::string text = value ? value : "";
  std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return text;
}

bool ParseFieldMapSourceMode(const char* value, FieldMapSourceMode& mode) {
  const std::string text = Lowercase(value);
  if (text.empty() || text == "auto") {
    mode = FieldMapSourceMode::Auto;
    return true;
  }
  if (text == "legacy" || text == "legacyfocal" ||
      text == "legacyfocalplane" || text == "focal" ||
      text == "focalplane") {
    mode = FieldMapSourceMode::LegacyFocalPlane;
    return true;
  }
  if (text == "ppac" || text == "ppacs" || text == "ppacprojection") {
    mode = FieldMapSourceMode::PpacProjection;
    return true;
  }
  return false;
}

const char* SourceLabel(bool useLegacyFocalPlane) {
  return useLegacyFocalPlane ? "LegacyFocalPlane hit" : "PPAC1/PPAC2 projection";
}

struct LinearFit {
  double intercept = 0.0;
  double slope = 0.0;
  double r2 = 0.0;
};

std::pair<double, double> Range(const std::vector<double>& values,
                                bool includeZero = false) {
  double lo = *std::min_element(values.begin(), values.end());
  double hi = *std::max_element(values.begin(), values.end());
  if (includeZero) {
    lo = std::min(lo, 0.0);
    hi = std::max(hi, 0.0);
  }
  const double span = hi - lo;
  const double pad = span > 0.0 ? 0.08 * span : 0.08 * std::max(1.0, std::abs(lo));
  return {lo - pad, hi + pad};
}

LinearFit FitLine(const std::vector<double>& x, const std::vector<double>& y) {
  LinearFit fit;
  const double n = static_cast<double>(x.size());
  double meanX = 0.0;
  double meanY = 0.0;
  for (std::size_t i = 0; i < x.size(); ++i) {
    meanX += x[i];
    meanY += y[i];
  }
  meanX /= n;
  meanY /= n;

  double sxx = 0.0;
  double sxy = 0.0;
  for (std::size_t i = 0; i < x.size(); ++i) {
    const double dx = x[i] - meanX;
    sxx += dx * dx;
    sxy += dx * (y[i] - meanY);
  }
  fit.slope = sxx > 0.0 ? sxy / sxx : 0.0;
  fit.intercept = meanY - fit.slope * meanX;

  double ssResidual = 0.0;
  double ssTotal = 0.0;
  for (std::size_t i = 0; i < x.size(); ++i) {
    const double fitted = fit.intercept + fit.slope * x[i];
    ssResidual += (y[i] - fitted) * (y[i] - fitted);
    ssTotal += (y[i] - meanY) * (y[i] - meanY);
  }
  fit.r2 = ssTotal > 0.0 ? 1.0 - ssResidual / ssTotal : 1.0;
  return fit;
}

std::string PlotPngPath(const char* outputPath, const char* canvasName) {
  std::string path = outputPath ? outputPath : "";
  const std::size_t slash = path.find_last_of("/\\");
  const std::string directory =
      slash == std::string::npos ? "" : path.substr(0, slash + 1);
  std::string basename =
      slash == std::string::npos ? path : path.substr(slash + 1);
  const std::size_t dot = basename.find_last_of('.');
  if (dot != std::string::npos) {
    basename = basename.substr(0, dot);
  }
  if (basename.empty()) {
    basename = "legacy_fieldmap_compare";
  }
  return directory + basename + "_" + canvasName + ".png";
}

void MakePlot(const std::vector<CompareRow>& rows, const Quantity& q,
              const std::string& pngPath) {
  std::vector<double> legacy;
  std::vector<double> fieldMap;
  std::vector<double> residual;
  legacy.reserve(rows.size());
  fieldMap.reserve(rows.size());
  residual.reserve(rows.size());
  for (const CompareRow& row : rows) {
    legacy.push_back(q.legacyValue(row));
    fieldMap.push_back(q.fieldValue(row));
    residual.push_back(legacy.back() - fieldMap.back());
  }

  std::vector<double> both = legacy;
  both.insert(both.end(), fieldMap.begin(), fieldMap.end());
  const auto xyRange = Range(both);
  const auto residualRange = Range(residual, true);
  const LinearFit fit = FitLine(legacy, fieldMap);
  double residualSumSq = 0.0;
  for (const double value : residual) {
    residualSumSq += value * value;
  }
  const double residualRms =
      residual.empty() ? 0.0 : std::sqrt(residualSumSq / residual.size());

  TCanvas canvas(q.key, (std::string(q.title) + " legacy vs field-map").c_str(), 900, 900);
  canvas.SetBorderMode(0);
  canvas.SetBorderSize(0);
  canvas.SetFrameBorderMode(0);
  TPad top("top", "", 0.0, 0.32, 1.0, 1.0);
  TPad bottom("bottom", "", 0.0, 0.0, 1.0, 0.32);
  top.SetBorderMode(0);
  top.SetBorderSize(0);
  top.SetFrameBorderMode(0);
  top.SetBottomMargin(0.03);
  top.SetLeftMargin(0.12);
  top.SetRightMargin(0.04);
  bottom.SetBorderMode(0);
  bottom.SetBorderSize(0);
  bottom.SetFrameBorderMode(0);
  bottom.SetTopMargin(0.04);
  bottom.SetBottomMargin(0.30);
  bottom.SetLeftMargin(0.12);
  bottom.SetRightMargin(0.04);
  top.Draw();
  bottom.Draw();

  top.cd();
  TH2D topFrame("topFrame",
                (std::string(q.title) + ";Legacy " + q.title + " [" + q.unit +
                 "];FieldMap " + q.title + " [" + q.unit + "]")
                    .c_str(),
                1, xyRange.first, xyRange.second, 1, xyRange.first,
                xyRange.second);
  topFrame.SetStats(false);
  topFrame.GetXaxis()->SetLabelSize(0.0);
  topFrame.GetXaxis()->SetTitleSize(0.0);
  topFrame.Draw();
  TGraph scatter(static_cast<int>(rows.size()), legacy.data(), fieldMap.data());
  scatter.SetMarkerStyle(20);
  scatter.Draw("P SAME");
  TLine fitLine(xyRange.first, fit.intercept + fit.slope * xyRange.first,
                xyRange.second, fit.intercept + fit.slope * xyRange.second);
  fitLine.SetLineColor(kRed);
  fitLine.SetLineWidth(2);
  fitLine.Draw("SAME");
  TPaveText fitText(0.15, 0.74, 0.62, 0.90, "NDC");
  fitText.SetFillColor(0);
  fitText.SetFillStyle(0);
  fitText.SetBorderSize(0);
  fitText.SetTextAlign(12);
  fitText.SetTextSize(0.045);
  std::ostringstream equation;
  equation << "Fit: y = " << std::setprecision(5) << fit.intercept << " + "
           << fit.slope << " x";
  std::ostringstream r2Text;
  r2Text << "R^{2} = " << std::setprecision(6) << fit.r2;
  fitText.AddText(equation.str().c_str());
  fitText.AddText(r2Text.str().c_str());
  fitText.Draw("SAME");

  bottom.cd();
  TH2D bottomFrame(
      "bottomFrame",
      (std::string(";Legacy ") + q.title + " [" + q.unit +
       "];Residual [" + q.unit + "]")
          .c_str(),
      1, xyRange.first, xyRange.second, 1, residualRange.first,
      residualRange.second);
  bottomFrame.SetStats(false);
  bottomFrame.GetXaxis()->SetTitleSize(0.11);
  bottomFrame.GetXaxis()->SetLabelSize(0.10);
  bottomFrame.GetYaxis()->SetTitleSize(0.10);
  bottomFrame.GetYaxis()->SetLabelSize(0.09);
  bottomFrame.GetYaxis()->SetTitleOffset(0.55);
  bottomFrame.Draw();
  TGraph residualGraph(static_cast<int>(rows.size()), legacy.data(), residual.data());
  residualGraph.SetMarkerStyle(20);
  residualGraph.Draw("P SAME");
  TLine zero(xyRange.first, 0.0, xyRange.second, 0.0);
  zero.SetLineColor(kGray + 2);
  zero.SetLineStyle(2);
  zero.Draw("SAME");
  TPaveText residualText(0.58, 0.72, 0.96, 0.90, "NDC");
  residualText.SetFillColor(0);
  residualText.SetFillStyle(0);
  residualText.SetBorderSize(0);
  residualText.SetTextAlign(12);
  residualText.SetTextSize(0.09);
  std::ostringstream residualLabel;
  residualLabel << "Residual RMS = " << std::fixed << std::setprecision(4)
                << residualRms << " " << q.unit;
  residualText.AddText(residualLabel.str().c_str());
  residualText.Draw("SAME");

  canvas.Modified();
  canvas.Update();
  canvas.Write();
  canvas.SaveAs(pngPath.c_str());
}

double RmsResidual(const std::vector<CompareRow>& rows, const Quantity& q) {
  double sumSq = 0.0;
  for (const CompareRow& row : rows) {
    const double residual = q.legacyValue(row) - q.fieldValue(row);
    sumSq += residual * residual;
  }
  return rows.empty() ? 0.0 : std::sqrt(sumSq / static_cast<double>(rows.size()));
}

double MaxAbsResidual(const std::vector<CompareRow>& rows, const Quantity& q) {
  double maxAbs = 0.0;
  for (const CompareRow& row : rows) {
    const double residual = q.legacyValue(row) - q.fieldValue(row);
    maxAbs = std::max(maxAbs, std::abs(residual));
  }
  return maxAbs;
}

}  // namespace

void CompareLegacyFieldMap(const char* inputPath = "SimData~0.root",
                           const char* outputPath = "legacy_fieldmap_compare.root",
                           double legacyPlaneZLocalMm = -600.0,
                           double ppac1PlaneZLocalMm = -489.99875,
                           double ppac2PlaneZLocalMm = -89.99875,
                           const char* treeName = "AccurateData",
                           const char* fieldMapSource = "auto") {
  std::unique_ptr<TFile> input(TFile::Open(inputPath, "READ"));
  if (!input || input->IsZombie()) {
    std::cerr << "ERROR: cannot open input file: " << inputPath << '\n';
    return;
  }

  TTree* tree = nullptr;
  input->GetObject(treeName, tree);
  if (!tree) {
    std::cerr << "ERROR: cannot find tree '" << treeName << "' in " << inputPath
              << '\n';
    return;
  }

  bool slitLegacyTransmitted = false;
  bool legacyFocalPlaneAccepted = false;
  bool ppac1Accepted = false;
  bool ppac2Accepted = false;

  std::vector<int>* slitTrackId = nullptr;
  std::vector<int>* legacyFocalPlaneTrackId = nullptr;
  std::vector<double>* legacyX = nullptr;
  std::vector<double>* legacyY = nullptr;
  std::vector<double>* legacyAngleX = nullptr;
  std::vector<double>* legacyAngleY = nullptr;
  std::vector<double>* legacyFocalPlaneX = nullptr;
  std::vector<double>* legacyFocalPlaneY = nullptr;
  std::vector<double>* legacyFocalPlaneMomentumX = nullptr;
  std::vector<double>* legacyFocalPlaneMomentumY = nullptr;
  std::vector<double>* legacyFocalPlaneMomentumZ = nullptr;
  std::vector<double>* ppac1X = nullptr;
  std::vector<double>* ppac1Y = nullptr;
  std::vector<double>* ppac2X = nullptr;
  std::vector<double>* ppac2Y = nullptr;

  const bool hasLegacyFocalPlane =
      tree->GetBranch("LegacyFocalPlaneAccepted") &&
      tree->GetBranch("LegacyFocalPlaneHitLocalPosX") &&
      tree->GetBranch("LegacyFocalPlaneHitLocalPosY") &&
      tree->GetBranch("LegacyFocalPlaneHitLocalMomentumX") &&
      tree->GetBranch("LegacyFocalPlaneHitLocalMomentumY") &&
      tree->GetBranch("LegacyFocalPlaneHitLocalMomentumZ");

  FieldMapSourceMode sourceMode = FieldMapSourceMode::Auto;
  if (!ParseFieldMapSourceMode(fieldMapSource, sourceMode)) {
    std::cerr << "ERROR: unknown fieldMapSource '" << fieldMapSource
              << "'. Use \"auto\", \"legacy\", or \"ppac\".\n";
    return;
  }

  if (sourceMode == FieldMapSourceMode::LegacyFocalPlane &&
      !hasLegacyFocalPlane) {
    std::cerr << "ERROR: fieldMapSource=\"legacy\" was requested, but "
              << "LegacyFocalPlaneHit* branches are not present in "
              << inputPath << ".\n";
    return;
  }

  const bool useLegacyFocalPlane =
      sourceMode == FieldMapSourceMode::LegacyFocalPlane ||
      (sourceMode == FieldMapSourceMode::Auto && hasLegacyFocalPlane);

  tree->SetBranchAddress("SlitHitTransmitted", &slitLegacyTransmitted);
  tree->SetBranchAddress("SlitHitTrackId", &slitTrackId);
  tree->SetBranchAddress("Ppac1Accepted", &ppac1Accepted);
  tree->SetBranchAddress("Ppac2Accepted", &ppac2Accepted);
  tree->SetBranchAddress("MdmTracePositionX", &legacyX);
  tree->SetBranchAddress("MdmTracePositionY", &legacyY);
  tree->SetBranchAddress("MdmTraceAngleX", &legacyAngleX);
  tree->SetBranchAddress("MdmTraceAngleY", &legacyAngleY);
  if (useLegacyFocalPlane) {
    tree->SetBranchAddress("LegacyFocalPlaneAccepted", &legacyFocalPlaneAccepted);
    tree->SetBranchAddress("LegacyFocalPlaneHitTrackId", &legacyFocalPlaneTrackId);
    tree->SetBranchAddress("LegacyFocalPlaneHitLocalPosX", &legacyFocalPlaneX);
    tree->SetBranchAddress("LegacyFocalPlaneHitLocalPosY", &legacyFocalPlaneY);
    tree->SetBranchAddress("LegacyFocalPlaneHitLocalMomentumX", &legacyFocalPlaneMomentumX);
    tree->SetBranchAddress("LegacyFocalPlaneHitLocalMomentumY", &legacyFocalPlaneMomentumY);
    tree->SetBranchAddress("LegacyFocalPlaneHitLocalMomentumZ", &legacyFocalPlaneMomentumZ);
  }
  tree->SetBranchAddress("Ppac1HitLocalPosX", &ppac1X);
  tree->SetBranchAddress("Ppac1HitLocalPosY", &ppac1Y);
  tree->SetBranchAddress("Ppac2HitLocalPosX", &ppac2X);
  tree->SetBranchAddress("Ppac2HitLocalPosY", &ppac2Y);

  std::unique_ptr<TFile> output(TFile::Open(outputPath, "RECREATE"));
  if (!output || output->IsZombie()) {
    std::cerr << "ERROR: cannot create output file: " << outputPath << '\n';
    return;
  }

  auto* residualTree = new TTree(
      "LegacyFieldMapResiduals",
      "Field-map PPAC track projected to the legacy MDMTrace plane");

  Long64_t event = 0;
  double fieldX = 0.0;
  double rawFieldX = 0.0;
  double fieldY = 0.0;
  double fieldAngleX = 0.0;
  double fieldAngleY = 0.0;
  double mdmX = 0.0;
  double mdmY = 0.0;
  double mdmAngleX = 0.0;
  double mdmAngleY = 0.0;
  double dx = 0.0;
  double dy = 0.0;
  double dAngleX = 0.0;
  double dAngleY = 0.0;
  double ppac1LocalZ = 0.0;
  double ppac2LocalZ = 0.0;

  residualTree->Branch("Event", &event, "Event/L");
  residualTree->Branch("LegacyPlaneZLocalMm", &legacyPlaneZLocalMm,
                       "LegacyPlaneZLocalMm/D");
  residualTree->Branch("RawFieldMapX", &rawFieldX, "RawFieldMapX/D");
  residualTree->Branch("FieldMapX", &fieldX, "FieldMapX/D");
  residualTree->Branch("FieldMapY", &fieldY, "FieldMapY/D");
  residualTree->Branch("FieldMapAngleXDeg", &fieldAngleX,
                       "FieldMapAngleXDeg/D");
  residualTree->Branch("FieldMapAngleYDeg", &fieldAngleY,
                       "FieldMapAngleYDeg/D");
  residualTree->Branch("LegacyX", &mdmX, "LegacyX/D");
  residualTree->Branch("LegacyY", &mdmY, "LegacyY/D");
  residualTree->Branch("LegacyAngleXDeg", &mdmAngleX, "LegacyAngleXDeg/D");
  residualTree->Branch("LegacyAngleYDeg", &mdmAngleY, "LegacyAngleYDeg/D");
  residualTree->Branch("DeltaX", &dx, "DeltaX/D");
  residualTree->Branch("DeltaY", &dy, "DeltaY/D");
  residualTree->Branch("DeltaAngleXDeg", &dAngleX, "DeltaAngleXDeg/D");
  residualTree->Branch("DeltaAngleYDeg", &dAngleY, "DeltaAngleYDeg/D");
  residualTree->Branch("Ppac1LocalZ", &ppac1LocalZ, "Ppac1LocalZ/D");
  residualTree->Branch("Ppac2LocalZ", &ppac2LocalZ, "Ppac2LocalZ/D");

  auto* hDx = new TH1D("hDeltaX", "Field map - legacy;#Delta x [mm];Events", 200, -100.0, 100.0);
  auto* hDy = new TH1D("hDeltaY", "Field map - legacy;#Delta y [mm];Events", 200, -100.0, 100.0);
  auto* hDAngleX = new TH1D("hDeltaAngleX", "Field map - legacy;#Delta x' [deg];Events", 200, -10.0, 10.0);
  auto* hDAngleY = new TH1D("hDeltaAngleY", "Field map - legacy;#Delta y' [deg];Events", 200, -10.0, 10.0);
  hDx->SetTitle("Legacy - field map;#Delta x [mm];Events");
  hDy->SetTitle("Legacy - field map;#Delta y [mm];Events");
  hDAngleX->SetTitle("Legacy - field map;#Delta x' [deg];Events");
  hDAngleY->SetTitle("Legacy - field map;#Delta y' [deg];Events");

  std::vector<CompareRow> rows;
  Long64_t selected = 0;
  Long64_t skipped = 0;
  Long64_t legacyStopped = 0;
  const Long64_t entries = tree->GetEntries();
  for (Long64_t i = 0; i < entries; ++i) {
    tree->GetEntry(i);

    if (!ppac1Accepted || !ppac2Accepted || !slitLegacyTransmitted ||
        !HasValue(legacyX) || !HasValue(legacyY) || !HasValue(legacyAngleX) ||
        !HasValue(legacyAngleY)) {
      ++skipped;
      continue;
    }

    if (useLegacyFocalPlane) {
      if (!legacyFocalPlaneAccepted || !HasValue(legacyFocalPlaneX) ||
          !HasValue(legacyFocalPlaneY) ||
          !HasValue(legacyFocalPlaneMomentumX) ||
          !HasValue(legacyFocalPlaneMomentumY) ||
          !HasValue(legacyFocalPlaneMomentumZ)) {
        ++skipped;
        continue;
      }
    } else if (!HasValue(ppac1X) || !HasValue(ppac1Y) ||
               !HasValue(ppac2X) || !HasValue(ppac2Y)) {
      ++skipped;
      continue;
    }

    mdmX = FirstOrNaN(legacyX);
    mdmY = FirstOrNaN(legacyY);
    mdmAngleX = FirstOrNaN(legacyAngleX);
    mdmAngleY = FirstOrNaN(legacyAngleY);
    if (std::abs(mdmX) > 1.0e10 || std::abs(mdmY) > 1.0e10) {
      ++legacyStopped;
      continue;
    }

    if (useLegacyFocalPlane) {
      const int trackId = FirstTrackOr(slitTrackId, FirstTrackOr(legacyFocalPlaneTrackId, -99));
      const std::size_t focalIndex = IndexForTrack(legacyFocalPlaneTrackId, trackId);
      fieldX = AtOrNaN(legacyFocalPlaneX, focalIndex);
      fieldY = AtOrNaN(legacyFocalPlaneY, focalIndex);
      rawFieldX = fieldX;
      const double px = AtOrNaN(legacyFocalPlaneMomentumX, focalIndex);
      const double py = AtOrNaN(legacyFocalPlaneMomentumY, focalIndex);
      const double pz = AtOrNaN(legacyFocalPlaneMomentumZ, focalIndex);
      if (!std::isfinite(fieldX) || !std::isfinite(fieldY) ||
          !std::isfinite(px) || !std::isfinite(py) || !std::isfinite(pz) ||
          std::abs(pz) < 1.0e-12) {
        ++skipped;
        continue;
      }
      fieldAngleX = std::atan2(px, pz) * TMath::RadToDeg();
      fieldAngleY =
          std::atan2(py, std::sqrt(px * px + pz * pz)) * TMath::RadToDeg();
      ppac1LocalZ = legacyPlaneZLocalMm;
      ppac2LocalZ = legacyPlaneZLocalMm;
    } else {
      const double x1 = ppac1X->front();
      const double y1 = ppac1Y->front();
      const double z1 = ppac1PlaneZLocalMm;
      const double x2 = ppac2X->front();
      const double y2 = ppac2Y->front();
      const double z2 = ppac2PlaneZLocalMm;
      const double dzPpac = z2 - z1;
      if (std::abs(dzPpac) < 1.0e-9) {
        ++skipped;
        continue;
      }

      const double t = (legacyPlaneZLocalMm - z1) / dzPpac;
      rawFieldX = x1 + t * (x2 - x1);
      fieldX = rawFieldX;
      fieldY = y1 + t * (y2 - y1);
      fieldAngleX = std::atan2(x2 - x1, dzPpac) * TMath::RadToDeg();
      fieldAngleY =
          std::atan2(y2 - y1,
                     std::sqrt((x2 - x1) * (x2 - x1) + dzPpac * dzPpac)) *
          TMath::RadToDeg();
      ppac1LocalZ = z1;
      ppac2LocalZ = z2;
    }

    dx = mdmX - fieldX;
    dy = mdmY - fieldY;
    dAngleX = mdmAngleX - fieldAngleX;
    dAngleY = mdmAngleY - fieldAngleY;
    event = i;

    rows.push_back({mdmX, mdmY, mdmAngleX, mdmAngleY,
                    fieldX, fieldY, fieldAngleX, fieldAngleY});

    residualTree->Fill();
    hDx->Fill(dx);
    hDy->Fill(dy);
    hDAngleX->Fill(dAngleX);
    hDAngleY->Fill(dAngleY);
    ++selected;
  }

  const std::vector<Quantity> quantities{
      {"X", "X", "mm",
       [](const CompareRow& r) { return r.legacyX; },
       [](const CompareRow& r) { return r.fieldX; }},
      {"Y", "Y", "mm",
       [](const CompareRow& r) { return r.legacyY; },
       [](const CompareRow& r) { return r.fieldY; }},
      {"AngX", "AngX", "deg",
       [](const CompareRow& r) { return r.legacyAngleX; },
       [](const CompareRow& r) { return r.fieldAngleX; }},
      {"AngY", "AngY", "deg",
       [](const CompareRow& r) { return r.legacyAngleY; },
       [](const CompareRow& r) { return r.fieldAngleY; }},
  };

  output->cd();
  gStyle->SetOptStat(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  std::vector<std::string> pngPaths;
  if (!rows.empty()) {
    for (const Quantity& quantity : quantities) {
      pngPaths.push_back(PlotPngPath(outputPath, quantity.key));
      MakePlot(rows, quantity, pngPaths.back());
      output->cd();
    }
  }
  residualTree->Write();
  hDx->Write();
  hDy->Write();
  hDAngleX->Write();
  hDAngleY->Write();
  output->Close();

  std::cout << "Input entries: " << entries << '\n'
            << "Compared entries: " << selected << '\n'
            << "Skipped entries: " << skipped << '\n'
            << "Legacy stopped entries: " << legacyStopped << '\n'
            << "Field-map source: " << SourceLabel(useLegacyFocalPlane) << '\n'
            << "Legacy plane local z: " << legacyPlaneZLocalMm << " mm\n"
            << "PPAC1 plane local z: " << ppac1PlaneZLocalMm << " mm\n"
            << "PPAC2 plane local z: " << ppac2PlaneZLocalMm << " mm\n"
            << "PPAC-to-legacy X origin offset: 0 mm\n"
            << "Wrote " << outputPath << " with 4 canvases and " << rows.size()
            << " rays\n";
  if (!pngPaths.empty()) {
    std::cout << "Saved PNG plots:\n";
    for (const std::string& pngPath : pngPaths) {
      std::cout << "  " << pngPath << '\n';
    }
  }
  std::cout
            << "Residual convention: Legacy - FieldMap\n";
  if (rows.empty()) {
    std::cout << "No comparable rays found; canvases and PNG plots were not written.\n";
    return;
  }
  std::cout << std::fixed << std::setprecision(6);
  for (const Quantity& quantity : quantities) {
    std::cout << std::setw(6) << quantity.key << " RMS="
              << RmsResidual(rows, quantity)
              << " MaxAbs=" << MaxAbsResidual(rows, quantity) << " "
              << quantity.unit << "\n";
  }
}
