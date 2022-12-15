#ifndef TexPPACSimAnalysis_h
#define TexPPACSimAnalysis_h 1

#include "G4AnalysisManager.hh"
#include "globals.hh"

class TFile;
class TTree;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace TexPPACSim
{

  class Analysis
  {
  public:
    Analysis();
    ~Analysis();

    void Book(std::string beamEnergy);
    void Save();

    void FillHisto(G4int id, G4double e, G4double weight = 1.0);

    void FillNtuple(G4int fTrackID, G4String fParticleName,
                    G4double fEnergyDeposit, G4double fKineticEnergy,
                    G4ThreeVector fMomentum, G4ThreeVector fPosition);

    void PrintStatistic();

  private:
    G4bool fFactoryOn;
  };

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
