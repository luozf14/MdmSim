#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    ActionInitialization::ActionInitialization()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    ActionInitialization::~ActionInitialization()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void ActionInitialization::BuildForMaster() const
    {
        RunAction *runAction = new RunAction;
        runAction->SetProcessNumber(std::get<G4int>(fParameters.find("ProcessNumber")->second));
        SetUserAction(runAction);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void ActionInitialization::Build() const
    {
        SetUserAction(new PrimaryGeneratorAction);

        RunAction *runAction = new RunAction;
        runAction->SetProcessNumber(std::get<G4int>(fParameters.find("ProcessNumber")->second));
        SetUserAction(runAction);

        EventAction *eventAction = new EventAction(runAction);
        eventAction->ParseParams(std::get<std::map<std::string, G4double>>(fParameters.find("EventParameters")->second));
        SetUserAction(eventAction);

        SetUserAction(new SteppingAction(eventAction));
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
