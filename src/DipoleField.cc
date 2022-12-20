#include "DipoleField.hh"
#include "Constants.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::DipoleField()
    {
        // define commands for this class
        DefineCommands();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::~DipoleField()
    {
        delete fMessenger;
    }

    void DipoleField::GetFieldValue(const G4double PositionAndTime[4], double *bField) const
    {
        bField[0] = 0.;
        G4double x = std::sqrt(PositionAndTime[0]*PositionAndTime[0]+PositionAndTime[2]*PositionAndTime[2])-kDipoleRadius;
        G4double By=fBy0*(1.+kDipoleAlpha*(x/kDipoleRadius));
        bField[1] = fBy0;
        bField[2] = 0.;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DipoleField::DefineCommands()
    {
        // Define /TexPPACSim/field command directory using generic messenger class
        fMessenger = new G4GenericMessenger(this,
                                            "/TexPPACSim/field/",
                                            "Field control");

        // fieldValue command
        auto &valueCmd = fMessenger->DeclareMethodWithUnit("value", "tesla",
                                                           &DipoleField::SetField,
                                                           "Set field strength.");
        valueCmd.SetParameterName("field", true);
        valueCmd.SetDefaultValue("1.");
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
