#ifndef TexPPACSimFirstMultipoleField_H
#define TexPPACSimFirstMultipoleField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RotationMatrix.hh"

class G4GenericMessenger;

namespace TexPPACSim
{

    /// FirstMultipole field

    class FirstMultipoleField : public G4MagneticField
    {
    public:
        FirstMultipoleField(G4ThreeVector, G4RotationMatrix*);
        ~FirstMultipoleField() override;

        void GetFieldValue(const G4double point[4], double *bField) const override;

        void SetField(G4double multipoleProbe);
        // G4double GetField() const { return fBy0; }
    private:
        // G4GenericMessenger *fMessenger = nullptr;
        G4ThreeVector fFirstMultipolePos;
        G4RotationMatrix* fFirstMultipoleRot;
        G4double fBQR;  // quadrupole at r=R;
        G4double fBHR;  // hexapole
        G4double fBOR;  // qctapole
        G4double fBDR;  // decapole
        G4double fBDDR; // dodecapole
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
