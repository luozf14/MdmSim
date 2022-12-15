#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace TexPPACSim
{

    Analysis *Analysis::fInstance = nullptr;

    Analysis::Analysis()
        : fFactoryOn(false)
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    Analysis::~Analysis()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    Analysis *Analysis::Instance()
    {
        if (!fInstance)
        {
            fInstance = new Analysis();
        }
        return fInstance;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void Analysis::Book(std::string processNumber)
    {

        if (!fFactoryOn)
        {
            // Open a root file
            std::string fileName = "Data~" + processNumber + ".root";
            fFile = new TFile(fileName.c_str(), "RECREATE");
            if (!fFile)
            {
                G4cerr << "\n---> Analysis::Book(): cannot open "
                       << fileName << G4endl;
                return;
            }

            // Define two trees, one for accurate hit info, the other one for what you will get in experiment.
            fTreeAccurate = new TTree("AccurateData", "Accurate data recorded");
            fTreeExperiment = new TTree("ExperimentalData", "Experimental data recorded by DAQ");

            // Add branches to trees
            fTreeAccurate->Branch("SiEHitGlobalPos", &fSiEHitGlobalPos);
            fTreeAccurate->Branch("SiEHitLocalPos", &fSiEHitLocalPos);
            fTreeAccurate->Branch("SiEHitGlobalMomentum", &fSiEHitGlobalMomentum);
            fTreeAccurate->Branch("SiEHitLocalMomentum", &fSiEHitLocalMomentum);
            fTreeAccurate->Branch("SiEHitTrackId", &fSiEHitTrackId);
            fTreeAccurate->Branch("SiEHitEDep", &fSiEHitEDep);
            fTreeAccurate->Branch("SiEHitTime", &fSiEHitTime);

            fTreeExperiment->Branch("DaqTrigger", &fDaqTrigger, "DaqTrigger/O");
            fTreeExperiment->Branch("SiEHitHorizontalNo", &fSiEHitHorizontalNo);
            fTreeExperiment->Branch("SiEHitVerticalNo", &fSiEHitVerticalNo);
            fTreeExperiment->Branch("SiEHitEDep", &fSiEHitEDepExp);
            fTreeExperiment->Branch("SiEHitTime", &fSiEHitTimeExp);

            fFactoryOn = true;
        }

        G4cout << "\n----> Output file is open in "
               << fFile->GetName() << "."
               << G4endl;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void Analysis::Save()
    {
        if (!fFactoryOn)
            return;

        fTreeAccurate->Write();
        fTreeExperiment->Write();
        fFile->Close();

        G4cout << "\n----> Histograms and ntuples are saved.\n"
               << G4endl;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void Analysis::FillTreeAccurate()
    {
        fTreeAccurate->Fill();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void Analysis::FillTreeExperiment()
    {
        fTreeExperiment->Fill();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......