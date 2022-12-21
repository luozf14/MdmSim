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
        DipoleField(G4ThreeVector dipolePos);
        ~DipoleField() override;

        void GetFieldValue(const G4double point[4], double *bField) const override;

        void SetField(G4double val) { fBy0 = val; }
        G4double GetField() const { return fBy0; }

    private:
        G4double fBy0;
        G4ThreeVector fDipolePos;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
