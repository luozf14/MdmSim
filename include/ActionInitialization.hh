#ifndef TexPPACSimActionInitialization_h
#define TexPPACSimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

#include <map>
#include <variant>
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

        void SetParameters(std::map<std::string, std::variant<G4int, G4double>> i) { fParameters = i; };

    private:
        std::map<std::string, std::variant<G4int, G4double>> fParameters;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
