#ifndef MdmSimRunAction_h
#define MdmSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

namespace MdmSim
{

    class RunAction : public G4UserRunAction
    {
    public:
        RunAction();
        ~RunAction() override;

        void BeginOfRunAction(const G4Run *) override;
        void EndOfRunAction(const G4Run *) override;

        void SetProcessNumber(G4int i) { fProcessNumber = i; };

    private:
        G4int fProcessNumber;
    };

}

#endif
