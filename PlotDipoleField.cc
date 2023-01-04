#include "DipoleField.hh"
#include "Constants.hh"

#include "G4RotationMatrix.hh"

#include "G4SystemOfUnits.hh"
#include "globals.hh"

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
    auto gr2Bx = std::make_unique<TGraph2D>(10000);
    auto gr2By = std::make_unique<TGraph2D>(10000);
    auto gr2Bz = std::make_unique<TGraph2D>(10000);
    auto gr1 = std::make_unique<TGraph>(10000);
    gr2Bx->SetTitle("B_{x}; -x [mm]; z [mm]; B_{x} [#times B_{0}]");
    gr2By->SetTitle("B_{y}; -x [mm]; z [mm]; B_{y} [#times B_{0}]");
    gr2Bz->SetTitle("B_{z}; -x [mm]; z [mm]; B_{z} [#times B_{0}]");
    auto ran = std::make_unique<TRandom3>();
    // y = 0.3 * kDipoleFieldHeight;
    y = 0;
    // y = -0.3 * kDipoleFieldHeight;
    for (int i = 0; i < 10000; i++)
    {
        double theta = kDipoleDeflectionAngle * ran->Rndm();
        double radii = kDipoleFieldRadius - 0.5 * kDipoleFieldWidth + kDipoleFieldWidth * ran->Rndm();
        x = radii * std::cos(theta) - kDipoleFieldRadius;
        z = radii * std::sin(theta);
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr2Bx->SetPoint(i, -x, z, bField[0] * 1e7 / (dipoleProbe * 1.034));
        gr2By->SetPoint(i, -x, z, bField[1] * 1e7 / (dipoleProbe * 1.034));
        gr2Bz->SetPoint(i, -x, z, bField[2] * 1e7 / (dipoleProbe * 1.034));
        gr1->SetPoint(i, -x, z);
    }
    for (int i = 0; i < 1000; i++)
    {
        x = kDipoleFieldWidth * ran->Rndm() - 0.5 * kDipoleFieldWidth;
        z = kDipoleZ11 * ran->Rndm() - kDipoleZ11;
        const G4double point[4] = {x, y, z, 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr2Bx->AddPoint(-x, z, bField[0] * 1e7 / (dipoleProbe * 1.034));
        gr2By->AddPoint(-x, z, bField[1] * 1e7 / (dipoleProbe * 1.034));
        gr2Bz->AddPoint(-x, z, bField[2] * 1e7 / (dipoleProbe * 1.034));
        gr1->AddPoint(-x, z);
    }
    for (int i = 0; i < 1000; i++)
    {
        G4ThreeVector pos(kDipoleFieldWidth * ran->Rndm() - 0.5 * kDipoleFieldWidth, y, kDipoleZ11 * ran->Rndm() - 0.5 * kDipoleZ11);
        pos.rotateY(-kDipoleDeflectionAngle);
        G4ThreeVector solidDipoleFieldExitTrans1(-kDipoleFieldRadius - kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
        G4ThreeVector solidDipoleFieldExitTrans2(-0.5 * kDipoleZ22 * std::cos(kDipoleDeflectionAngle - M_PI / 2.), 0., -0.5 * kDipoleZ22 * std::sin(kDipoleDeflectionAngle - M_PI / 2.));
        G4ThreeVector solidDipoleFieldExitTrans = solidDipoleFieldExitTrans1 + solidDipoleFieldExitTrans2;
        pos += solidDipoleFieldExitTrans;
        const G4double point[4] = {pos[0], pos[1], pos[2], 0};
        double bField[3];
        dipoleField->GetFieldValue(point, bField);
        gr2Bx->AddPoint(-pos[0], pos[2], bField[0] * 1e7 / (dipoleProbe * 1.034));
        gr2By->AddPoint(-pos[0], pos[2], bField[1] * 1e7 / (dipoleProbe * 1.034));
        gr2Bz->AddPoint(-pos[0], pos[2], bField[2] * 1e7 / (dipoleProbe * 1.034));
        gr1->AddPoint(-pos[0], pos[2]);
    }
    gStyle->SetPalette(1);

    auto c1 = std::make_unique<TCanvas>("c1", "", 2048, 768);
    c1->Divide(3, 1);
    c1->cd(1);
    gr2Bx->Draw("SURF1");
    c1->cd(2);
    gr2By->Draw("SURF1");
    c1->cd(3);
    gr2Bz->Draw("SURF1");
    c1->SaveAs("DipoleField.png");

    auto c2 = std::make_unique<TCanvas>("c2", "", 768, 768);
    c2->cd();
    gr1->Draw();

    app.Run();

    return 0;
}