#include "Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <TFile.h>
#include <TTree.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace TexPPACSim
{

  Analysis::Analysis()
      : fFactoryOn(false)
  {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  Analysis::~Analysis()
  {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void Analysis::Book(std::string beamEnergy)
  {
    // Create or get analysis manager
    // The choice of analysis technology is done via selection of a namespace
    // in Analysis.hh
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    if (!fFactoryOn)
    {
      //
      analysisManager->SetDefaultFileType("root");
      analysisManager->SetVerboseLevel(1);
      // Only merge in MT mode to avoid warning when running in Sequential mode
#ifdef G4MULTITHREADED
      analysisManager->SetNtupleMerging(true);
#endif

      // Create directories
      analysisManager->SetHistoDirectoryName("histo");
      analysisManager->SetNtupleDirectoryName("ntuple");
    }

    // Open an output file
    //
    G4bool fileOpen = analysisManager->OpenFile("Data~"+beamEnergy);
    if (!fileOpen)
    {
      G4cerr << "\n---> Analysis::Book(): cannot open "
             << analysisManager->GetFileName() << G4endl;
      return;
    }

    if (!fFactoryOn)
    {
      // Create histograms.
      // Histogram ids are generated automatically starting from 0.
      // The start value can be changed by:
      // analysisManager->SetFirstHistoId(1);

      // id = 0
      analysisManager->CreateH1("EnergyDeposit", "Energy deposit in Si detector [keV]", 100, 0., 100.); //[keV]
      // id = 1
      analysisManager->CreateH1("PositionX", "Position at X-axis [mm]", 100, -20., 20.);
      // id = 2
      analysisManager->CreateH1("PositionY", "Position at Y-axis [mm]", 100, -20., 20.);

      // Create ntuples.
      // Ntuples ids are generated automatically starting from 0.
      // The start value can be changed by:
      // analysisManager->SetFirstMtupleId(1);

      // Create 1st ntuple (id = 0)
      analysisManager->CreateNtuple("Ntuple1", "Info of hits");
      analysisManager->CreateNtupleIColumn("TrackID");       // column Id = 0
      analysisManager->CreateNtupleSColumn("ParticleName");  // column Id = 1
      analysisManager->CreateNtupleDColumn("EnergyDeposit"); // column Id = 2
      analysisManager->CreateNtupleDColumn("KineticEnergy"); // column Id = 3
      analysisManager->CreateNtupleDColumn("Momentum.x");    // column Id = 4
      analysisManager->CreateNtupleDColumn("Momentum.y");    // column Id = 5
      analysisManager->CreateNtupleDColumn("Momentum.z");    // column Id = 6
      analysisManager->CreateNtupleDColumn("Position.x");    // column Id = 7
      analysisManager->CreateNtupleDColumn("Position.y");    // column Id = 8
      analysisManager->CreateNtupleDColumn("Position.z");    // column Id = 9
      analysisManager->FinishNtuple();

      fFactoryOn = true;
    }

    G4cout << "\n----> Output file is open in "
           << analysisManager->GetFileName() << "."
           << analysisManager->GetFileType() << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void Analysis::Save()
  {
    if (!fFactoryOn)
      return;

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();

    G4cout << "\n----> Histograms and ntuples are saved\n"
           << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void Analysis::FillHisto(G4int ih, G4double xbin, G4double weight)
  {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH1(ih, xbin, weight);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void Analysis::FillNtuple(G4int fTrackID, G4String fParticleName,
                                G4double fEnergyDeposit, G4double fKineticEnergy,
                                G4ThreeVector fMomentum, G4ThreeVector fPosition)
  {
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    // Fill 1st ntuple ( id = 0)
    analysisManager->FillNtupleIColumn(0, 0, fTrackID);
    analysisManager->FillNtupleSColumn(0, 1, fParticleName);
    analysisManager->FillNtupleDColumn(0, 2, fEnergyDeposit);
    analysisManager->FillNtupleDColumn(0, 3, fKineticEnergy);
    analysisManager->FillNtupleDColumn(0, 4, fMomentum.x());
    analysisManager->FillNtupleDColumn(0, 5, fMomentum.y());
    analysisManager->FillNtupleDColumn(0, 6, fMomentum.z());
    analysisManager->FillNtupleDColumn(0, 7, fPosition.x());
    analysisManager->FillNtupleDColumn(0, 8, fPosition.y());
    analysisManager->FillNtupleDColumn(0, 9, fPosition.z());
    analysisManager->AddNtupleRow(0);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void Analysis::PrintStatistic()
  {
    if (!fFactoryOn)
      return;

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

    G4cout << "\n ----> print histograms statistic \n"
           << G4endl;
    for (G4int i = 0; i < analysisManager->GetNofH1s(); ++i)
    {
      G4String name = analysisManager->GetH1Name(i);
      auto h1 = analysisManager->GetH1(i);

      G4String unitCategory;
      if (name[0U] == 'E')
        unitCategory = "Energy";
      if (name[0U] == 'L')
        unitCategory = "Length";
      // we use an explicit unsigned int type for operator [] argument
      // to avoid problems with windows compiler

      G4cout << name
             << ": mean = " << G4BestUnit(h1->mean(), unitCategory)
             << " rms = " << G4BestUnit(h1->rms(), unitCategory)
             << G4endl;
    }
  }

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......