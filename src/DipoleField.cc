#include "DipoleField.hh"
#include "Constants.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::DipoleField(G4ThreeVector dipolePos) : fDipolePos(dipolePos)
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::~DipoleField()
    {
    }

    void DipoleField::GetFieldValue(const G4double PositionAndTime[4], double *bField) const
    {
        bField[0] = 0.;
        G4ThreeVector particlePos(PositionAndTime[0], PositionAndTime[1], PositionAndTime[2]);
        G4ThreeVector deltaR = particlePos - fDipolePos;
        G4double x = std::sqrt(deltaR.x() * deltaR.x() + deltaR.z() * deltaR.z()) - kDipoleFieldRadius;
        G4double By = fBy0 * gauss * (1. - kDipoleNDX * (x / kDipoleFieldRadius) + kDipoleBET1 * std::pow(x / kDipoleFieldRadius, 2.) + kDipoleGAMA * std::pow(x / kDipoleFieldRadius, 3.) + kDipoleDELT * std::pow(x / kDipoleFieldRadius, 4.));
        // printf("\n---> DipoleField::GetFieldValue(): fBy0=%.4f\n",By);
        bField[1] = By;
        bField[2] = 0.;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    /*
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
            valueCmd.SetDefaultValue(".375");
        }
    */
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
