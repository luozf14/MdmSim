#ifndef MdmSimActionInitialization_h
#define MdmSimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

#include <map>
#include <variant>
/// Action initialization class.

namespace MdmSim
{

    class ActionInitialization : public G4VUserActionInitialization
    {
    public:
        ActionInitialization();
        ~ActionInitialization() override;

        void BuildForMaster() const override;
        void Build() const override;

        void SetParameters(std::map<std::string, std::variant<G4int, std::map<std::string, G4double>>> i) { fParameters = i; };

    private:
        std::map<std::string, std::variant<G4int, std::map<std::string, G4double>>> fParameters;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
