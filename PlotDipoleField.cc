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
#include "TLegend.h"

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
    h2Bx->SetTitle("B_{x} [#times B_{0}]; -x [mm]; z [mm]");
    h2By->SetTitle("B_{y} [#times B_{0}]; -x [mm]; z [mm]");
    h2Bz->SetTitle("B_{z} [#times B_{0}]; -x [mm]; z [mm]");
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
    auto gr1 = std::make_unique<TGraph>();
    gr1->SetName("gr1");
    gr1->SetTitle("B_{y} vs s; s [mm]; B_{y} [#times B_{0}]");
    auto gr2 = std::make_unique<TGraph>();
    gr2->SetName("gr2");
    auto gr3 = std::make_unique<TGraph>();
    gr3->SetName("gr3");

    G4double s = 0.;
    // G4double dx = 0.;
    for (int i = 0; i < 100; i++)
    {
        z = -kDipoleZ11 + (double)i * kDipoleZ11 / 100.;
        double bField[3];
        //
        const G4double point1[4] = {0.1 * kDipoleFieldRadius, 0., z, 0};
        dipoleField->GetFieldValue(point1, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        const G4double point2[4] = {0., 0., z, 0};
        dipoleField->GetFieldValue(point2, bField);
        gr2->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        const G4double point3[4] = {-0.1 * kDipoleFieldRadius, 0., z, 0};
        dipoleField->GetFieldValue(point3, bField);
        gr3->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleZ11 / 100.);
    }

    for (int i = 0; i < 1000; i++)
    {
        double bField[3];
        double theta = kDipoleDeflectionAngle / 1000. * (double)i;
        //
        double radii = 1.1 * kDipoleFieldRadius;
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point1[4] = {x, 0., z, 0};
        dipoleField->GetFieldValue(point1, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        radii = 1.0 * kDipoleFieldRadius;
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point2[4] = {x, 0., z, 0};
        dipoleField->GetFieldValue(point2, bField);
        gr2->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        radii = 0.9 * kDipoleFieldRadius;
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point3[4] = {x, 0., z, 0};
        dipoleField->GetFieldValue(point3, bField);
        gr3->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleFieldRadius * kDipoleDeflectionAngle / 1000.);
    }

    G4ThreeVector solidDipoleFieldExitTrans1(-kDipoleFieldRadius - kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
    G4ThreeVector solidDipoleFieldExitTrans2(-0.5 * kDipoleZ22 * std::cos(kDipoleDeflectionAngle - M_PI / 2.), 0., -0.5 * kDipoleZ22 * std::sin(kDipoleDeflectionAngle - M_PI / 2.));
    G4ThreeVector solidDipoleFieldExitTrans = solidDipoleFieldExitTrans1 + solidDipoleFieldExitTrans2;
    for (int i = 0; i < 100; i++)
    {
        z = -0.5 * kDipoleZ22 + (double)i * kDipoleZ22 / 100.;
        double bField[3];
        //
        G4ThreeVector pos1(0.1 * kDipoleFieldRadius, 0., z);
        pos1.rotateY(-kDipoleDeflectionAngle);
        pos1 += solidDipoleFieldExitTrans;
        const G4double point1[4] = {pos1[0], pos1[1], pos1[2], 0};
        dipoleField->GetFieldValue(point1, bField);
        gr1->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        G4ThreeVector pos2(0., 0., z);
        pos2.rotateY(-kDipoleDeflectionAngle);
        pos2 += solidDipoleFieldExitTrans;
        const G4double point2[4] = {pos2[0], pos2[1], pos2[2], 0};
        dipoleField->GetFieldValue(point2, bField);
        gr2->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        //
        G4ThreeVector pos3(-0.1 * kDipoleFieldRadius, 0., z);
        pos3.rotateY(-kDipoleDeflectionAngle);
        pos3 += solidDipoleFieldExitTrans;
        const G4double point3[4] = {pos3[0], pos3[1], pos3[2], 0};
        dipoleField->GetFieldValue(point3, bField);
        gr3->AddPoint(s, bField[1] * 1e7 / (dipoleProbe * 1.034));
        s += (kDipoleZ22 / 100.);
    }

    gStyle->SetPalette(1);
    gStyle->SetNumberContours(40);

    auto c1 = std::make_unique<TCanvas>("c1", "", 2048, 768);
    c1->Divide(3, 1);
    c1->cd(1);
    h2Bx->Draw("surf1");
    c1->cd(2);
    h2By->Draw("cont1");
    c1->cd(3);
    h2Bz->Draw("surf1");
    c1->SaveAs("DipoleField.png");

    auto c2 = std::make_unique<TCanvas>("c2", "", 1024, 768);
    c2->cd();
    gr1->SetLineColor(kRed);
    gr1->SetLineWidth(2);
    gr1->GetXaxis()->SetRangeUser(0., 3800.);
    gr1->GetYaxis()->SetRangeUser(0., 1.25);
    gr1->Draw("al");
    gr2->SetLineColor(kBlue);
    gr2->SetLineWidth(2);
    gr2->Draw("same");
    gr3->SetLineColor(8);
    gr3->SetLineWidth(2);
    gr3->Draw("same");
    auto legend = std::make_unique<TLegend>(0.75,0.75, 0.9,0.9);
    legend->AddEntry("gr1","r = 1.1 R_{0}","l");
    legend->AddEntry("gr2","r = 1.0 R_{0}","l");
    legend->AddEntry("gr3","r = 0.9 R_{0}","l");
    legend->Draw("same");
    c2->SaveAs("ByvsS.png");
    app.Run();

    return 0;
}