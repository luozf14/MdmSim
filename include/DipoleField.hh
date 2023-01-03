#ifndef MdmSimDipoleField_H
#define MdmSimDipoleField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RotationMatrix.hh"

class G4GenericMessenger;
class TF1;

namespace MdmSim
{
    /// Dipole field

    class DipoleField : public G4MagneticField
    {
    public:
        DipoleField(G4double By0, G4double mdmAng, G4ThreeVector dipolePos);
        ~DipoleField() override;

        void GetFieldValue(const G4double point[4], double *bField) const override;

        void SetField(G4double val) { fBy0 = val; }
        G4double GetField() const { return fBy0; }

    private:
        G4ThreeVector GetFieldInB(G4ThreeVector pos) const;
        G4ThreeVector GetFieldInC(G4ThreeVector pos) const;

        G4double fBy0;
        G4double fMdmAngle;
        G4ThreeVector fDipolePos;
        G4ThreeVector fDipolePosInB;
        G4ThreeVector fDipolePosInC;
        G4ThreeVector fCoordinateBPos;
        G4ThreeVector fCoordinateCPos;
        std::unique_ptr<G4RotationMatrix> fCoordinateBRot;
        std::unique_ptr<G4RotationMatrix> fCoordinateCRot;
        std::unique_ptr<TF1> fEngeFunc;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
