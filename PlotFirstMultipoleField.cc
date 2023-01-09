#include "FirstMultipoleField.hh"
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
    // TApplication app("app", &argc, argv);
    double multipoleProbe = 2780.84;
    G4double mdmAngle = 0.;
    auto *firstMultipoleField = new FirstMultipoleField(multipoleProbe, mdmAngle, G4ThreeVector());

    TGraph *gr1 = new TGraph(1000);
    gr1->SetName("gr1");
    gr1->SetTitle("B_{x} [#times B_{R}] vs z [mm]");
    TGraph *gr2 = new TGraph(1000);
    gr2->SetName("gr2");
    gr2->SetTitle("B_{y} [#times B_{R}] vs z [mm]");
    TGraph *gr3 = new TGraph(1000);
    gr3->SetName("gr3");
    gr3->SetTitle("B_{z} [#times B_{R}] vs z [mm]");
    TGraph *gr4 = new TGraph(1000);
    gr4->SetName("gr4");
    gr4->SetTitle("B_{trans} [#times B_{R}] vs z [mm]");
    for (int i = 0; i < 1000; i++)
    {
        double x = 0.3 * kFirstMultipoleAperture * std::sin(30. / 180. * 3.1415926);
        double y = 0.3 * kFirstMultipoleAperture * std::cos(30. / 180. * 3.1415926);
        double z = -330. + 660. / 1000. * (double)i;
        const G4double point[4] = {x, y, z, 4};
        double bField[3];
        firstMultipoleField->GetFieldValue(point, bField);
        gr1->SetPoint(i, z, bField[0] * 1e7 / (multipoleProbe * kJeffParameters[5]));
        gr2->SetPoint(i, z, bField[1] * 1e7 / (multipoleProbe * kJeffParameters[5]));
        gr3->SetPoint(i, z, bField[2] * 1e7 / (multipoleProbe * kJeffParameters[5]));
        gr4->SetPoint(i, z, std::sqrt(std::pow(bField[0] * 1e7 / (multipoleProbe * kJeffParameters[5]), 2.) + std::pow(bField[1] * 1e7 / (multipoleProbe * kJeffParameters[5]), 2.)));
    }
    /*
        double x, y, Bx, P = 330.;
        auto gr2D = new TGraph2D(10000);
        auto gr2Dy = new TGraph2D(10000);
        auto gr2Dz = new TGraph2D(10000);
        gr2D->SetTitle("B_{i}; X [cm]; Y [cm]; B_{i} [T]");
        auto r = new TRandom3();
        for (Int_t N = 0; N < 10000; N++)
        {
            double theta = 2.*3.1415926*r->Rndm();
            double radii = 65* r->Rndm();
            x = radii*std::cos(theta);
            y = radii*std::sin(theta);
            const G4double point[4] = {x, y, -130, 4.};
            double bField[3];
            firstMultipoleField->GetFieldValue(point, bField);
            gr2D->SetPoint(N, x, y, bField[0]);
            gr2Dy->SetPoint(N, x, y, bField[1]);
            gr2Dz->SetPoint(N, x, y, bField[2]);
        }
        gStyle->SetPalette(1);
    */
    TCanvas *c1 = new TCanvas("c1", "", 2048, 768);
    c1->Divide(2, 2);
    c1->cd(1);
    gr1->Draw("al");
    c1->cd(2);
    gr2->Draw("al");
    c1->cd(3);
    gr3->Draw("al");
    c1->cd(4);
    gr4->Draw("al");
    c1->SaveAs("c1.png");
    c1->Update();
    // TCanvas *c2 = new TCanvas("c2", "", 768, 768);
    // c2->cd();
    // gr2D->Draw("surf1");
    // // gr2Dy->Draw("surf1 same");
    // // gr2Dz->Draw("surf1 same");
    // c2->SaveAs("c2.png");
    // c1->Update();
    // app.Run();

    delete firstMultipoleField;
    delete gr1;
    delete gr2;
    delete gr3;
    delete c1;
    return 0;
}