#ifndef TexPPACSimDipoleField_H
#define TexPPACSimDipoleField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

class G4GenericMessenger;

namespace TexPPACSim
{

    /// Dipole field

    class DipoleField : public G4MagneticField
    {
    public:
        DipoleField();
        ~DipoleField() override;

        void GetFieldValue(const G4double point[4], double *bField) const override;

        void SetField(G4double val) { fBy0 = val; }
        G4double GetField() const { return fBy0; }

    private:
        void DefineCommands();

        G4GenericMessenger *fMessenger = nullptr;
        G4double fBy0 = 0.404985 * tesla;
        G4ThreeVector fDipolePos;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
