#include "DipoleField.hh"
#include "Constants.hh"

#include "G4RotationMatrix.hh"

#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include "TH2D.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TStyle.h"
#include "TApplication.h"

using namespace MdmSim;
int main(int argc, char **argv)
{
    TApplication app("app", &argc, argv);
    double dipoleProbe = 3916.68;
    G4double mdmAngle = 0.;
    auto dipoleField = std::make_unique<DipoleField>(dipoleProbe * 1.034, mdmAngle, G4ThreeVector(-kDipoleFieldRadius, 0., 0.));

    double x, z, y;
    auto h2Bx = std::make_unique<TH2D>("h2Bx", "", 80, -600., 2500., 80, -600., 2500.);
    auto h2By = std::make_unique<TH2D>("h2By", "", 80, -600., 2500., 80, -600., 2500.);
    auto h2Bz = std::make_unique<TH2D>("h2Bz", "", 80, -600., 2500., 80, -600., 2500.);
    auto gr1 = std::make_unique<TGraph>();
    h2Bx->SetTitle("B_{x} [#times B_{0}]; -x [mm]; z [mm]");
    h2By->SetTitle("B_{y} [#times B_{0}]; -x [mm]; z [mm]");
    h2Bz->SetTitle("B_{z} [#times B_{0}]; -x [mm]; z [mm]");
    gr1->SetTitle("B_{z} vs s; s [mm]; B_{z} [#times B_{0}]");
    auto ran = std::make_unique<TRandom3>();
    // y = 0.3 * kDipoleFieldHeight;
    y = 0;
    // y = -0.3 * kDipoleFieldHeight;
    for (int i = 0; i < 2000; i++)
    {
        x = kDipoleFieldWidth * ran->Rndm() - 0.5 * kDipoleFieldWidth;
        z = kDipoleZ11 * ran->Rndm() - kDipoleZ11;
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        h2Bx->SetBinContent(h2Bx->GetXaxis()->FindBin(-x), h2Bx->GetYaxis()->FindBin(z), bField[0] * 1e7 / (dipoleProbe * 1.034));
        h2By->SetBinContent(h2By->GetXaxis()->FindBin(-x), h2By->GetYaxis()->FindBin(z), bField[1] * 1e7 / (dipoleProbe * 1.034));
        h2Bz->SetBinContent(h2Bz->GetXaxis()->FindBin(-x), h2Bz->GetYaxis()->FindBin(z), bField[2] * 1e7 / (dipoleProbe * 1.034));
    }
    for (int i = 0; i < 20000; i++)
    {
        double theta = kDipoleDeflectionAngle * ran->Rndm();
        double radii = kDipoleFieldRadius - 0.5 * kDipoleFieldWidth + kDipoleFieldWidth * ran->Rndm();
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        h2Bx->SetBinContent(h2Bx->GetXaxis()->FindBin(-x), h2Bx->GetYaxis()->FindBin(z), bField[0] * 1e7 / (dipoleProbe * 1.034));
        h2By->SetBinContent(h2By->GetXaxis()->FindBin(-x), h2By->GetYaxis()->FindBin(z), bField[1] * 1e7 / (dipoleProbe * 1.034));
        h2Bz->SetBinContent(h2Bz->GetXaxis()->FindBin(-x), h2Bz->GetYaxis()->FindBin(z), bField[2] * 1e7 / (dipoleProbe * 1.034));
    }
    for (int i = 0; i < 2000; i++)
    {
        G4ThreeVector pos(kDipoleFieldWidth * ran->Rndm() - 0.5 * kDipoleFieldWidth, y, kDipoleZ22 * ran->Rndm() - 0.5 * kDipoleZ22);
        pos.rotateY(-kDipoleDeflectionAngle);
        G4ThreeVector solidDipoleFieldExitTrans1(-kDipoleFieldRadius - kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
        G4ThreeVector solidDipoleFieldExitTrans2(-0.5 * kDipoleZ22 * std::cos(kDipoleDeflectionAngle - M_PI / 2.), 0., -0.5 * kDipoleZ22 * std::sin(kDipoleDeflectionAngle - M_PI / 2.));
        G4ThreeVector solidDipoleFieldExitTrans = solidDipoleFieldExitTrans1 + solidDipoleFieldExitTrans2;
        pos += solidDipoleFieldExitTrans;
        const G4double point[4] = {pos[0], pos[1], pos[2], 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        h2Bx->SetBinContent(h2Bx->GetXaxis()->FindBin(-pos[0]), h2Bx->GetYaxis()->FindBin(pos[2]), bField[0] * 1e7 / (dipoleProbe * 1.034));
        h2By->SetBinContent(h2By->GetXaxis()->FindBin(-pos[0]), h2By->GetYaxis()->FindBin(pos[2]), bField[1] * 1e7 / (dipoleProbe * 1.034));
        h2Bz->SetBinContent(h2Bz->GetXaxis()->FindBin(-pos[0]), h2Bz->GetYaxis()->FindBin(pos[2]), bField[2] * 1e7 / (dipoleProbe * 1.034));
    }

    // Bz vs s
    G4double s = 0.;
    // G4double dx = 0.;
    G4double dx = 0.3 * kDipoleFieldWidth;
    for (int i = 0; i < 100; i++)
    {
        x = dx;
        z = -kDipoleZ11 + (double)i * kDipoleZ11 / 100.;
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleZ11 / 100.);
    }
    for (int i = 0; i < 1000; i++)
    {
        double theta = kDipoleDeflectionAngle / 1000. * (double)i;
        double radii = kDipoleFieldRadius + dx;
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleFieldRadius * kDipoleDeflectionAngle / 1000.);
    }
    for (int i = 0; i < 100; i++)
    {
        x = dx;
        z = -0.5 * kDipoleZ22 + (double)i * kDipoleZ22 / 100.;
        G4ThreeVector pos(x, y, z);
        pos.rotateY(-kDipoleDeflectionAngle);
        G4ThreeVector solidDipoleFieldExitTrans1(-kDipoleFieldRadius - kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
        G4ThreeVector solidDipoleFieldExitTrans2(-0.5 * kDipoleZ22 * std::cos(kDipoleDeflectionAngle - M_PI / 2.), 0., -0.5 * kDipoleZ22 * std::sin(kDipoleDeflectionAngle - M_PI / 2.));
        G4ThreeVector solidDipoleFieldExitTrans = solidDipoleFieldExitTrans1 + solidDipoleFieldExitTrans2;
        pos += solidDipoleFieldExitTrans;
        const G4double point[4] = {pos[0], pos[1], pos[2], 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleZ22 / 100.);
    }
    gStyle->SetPalette(1);

    auto c1 = std::make_unique<TCanvas>("c1", "", 2048, 768);
    c1->Divide(3, 1);
    c1->cd(1);
    h2Bx->Draw("surf1");
    c1->cd(2);
    h2By->Draw("surf1");
    c1->cd(3);
    h2Bz->Draw("surf1");
    c1->SaveAs("DipoleField.png");

    auto c2 = std::make_unique<TCanvas>("c2", "", 768, 768);
    c2->cd();
    gr1->Draw("alp");

    app.Run();

    return 0;
}