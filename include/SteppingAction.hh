#ifndef TexPPACSimSteppingAction_h
#define TexPPACSimSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4LogicalVolume;

/// Stepping action class
///

namespace TexPPACSim
{

    class EventAction;

    class SteppingAction : public G4UserSteppingAction
    {
    public:
        SteppingAction(EventAction *eventAction);
        ~SteppingAction() override;

        // method from the base class
        void UserSteppingAction(const G4Step *) override;

    private:
        EventAction *fEventAction = nullptr;
        std::array<G4String, 3> fPipeNames = {"FirstMultipoleMagnetPhysical", "SlitBox", "DipoleMagnetPhysical"};
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
