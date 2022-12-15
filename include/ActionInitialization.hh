#ifndef TexPPACSimActionInitialization_h
#define TexPPACSimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

#include <map>
/// Action initialization class.

namespace TexPPACSim
{

    class ActionInitialization : public G4VUserActionInitialization
    {
    public:
        ActionInitialization();
        ~ActionInitialization() override;

        void BuildForMaster() const override;
        void Build() const override;
    private:
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
