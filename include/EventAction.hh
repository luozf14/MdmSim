#ifndef MdmSimEventAction_h
#define MdmSimEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

/// Event action class
///

namespace MdmSim
{

    class RunAction;

    class EventAction : public G4UserEventAction
    {
    public:
        EventAction(RunAction *runAction);
        ~EventAction() override;

        void BeginOfEventAction(const G4Event *event) override;
        void EndOfEventAction(const G4Event *event) override;

        void ParseParams(std::map<std::string, G4double> params);

    private:
        RunAction *fRunAction = nullptr;
        G4int fHCID_SiDetectorDeltaE;
        G4int fHCID_SiDetectorE;
        G4int fHCID_Slit;
        G4int fHCID_Ppac1;
        G4int fHCID_Ppac2;
        G4double fSiDetectorEnergyResolution;
        G4double fTdcResolution;
        G4double fFirstMultipoleProbe;
        G4double fDipoleProbe;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
