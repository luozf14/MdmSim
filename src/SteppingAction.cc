#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

namespace MdmSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SteppingAction::SteppingAction(EventAction *eventAction)
        : fEventAction(eventAction)
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SteppingAction::~SteppingAction()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void SteppingAction::UserSteppingAction(const G4Step *step)
    {
        G4String volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

        if (std::find(fPipeNames.begin(), fPipeNames.end(), volumeName) != fPipeNames.end())
        {
            step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
        }
        // G4cout << "\n---> SteppingAction::UserSteppingAction: stepPos= " << G4BestUnit(step->GetPreStepPoint()->GetPosition(), "Length") << G4endl;

    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
