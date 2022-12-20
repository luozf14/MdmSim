#ifndef TexPPACSimDipoleField_H
#define TexPPACSimDipoleField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "G4SystemOfUnits.hh"

class G4GenericMessenger;

namespace TexPPACSim
{

/// Dipole field

class DipoleField : public G4MagneticField
{
  public:
    DipoleField();
    ~DipoleField() override;

    void GetFieldValue(const G4double point[4],double* bField ) const override;

    void SetField(G4double val) { fBy = val; }
    G4double GetField() const { return fBy; }

  private:
    void DefineCommands();

    G4GenericMessenger* fMessenger = nullptr;
    G4double fBy = 1.0*CLHEP::tesla;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
