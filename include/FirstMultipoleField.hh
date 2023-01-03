#ifndef MdmSimFirstMultipoleField_H
#define MdmSimFirstMultipoleField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RotationMatrix.hh"

class G4GenericMessenger;
class TF1;

namespace MdmSim
{

    /// FirstMultipole field

    class FirstMultipoleField : public G4MagneticField
    {
    public:
        FirstMultipoleField(G4double, G4double, G4ThreeVector);
        ~FirstMultipoleField() override;

        void GetFieldValue(const G4double point[4], double *bField) const override;
        // void SetField(G4double multipoleProbe);

        // 0th to 6th order derivatives of f1S
        double S0(double *x, double *par) const;
        double S1(double *x, double *par) const;
        double S2(double *x, double *par) const;
        double S3(double *x, double *par) const;
        double S4(double *x, double *par) const;
        double S5(double *x, double *par) const;
        double S6(double *x, double *par) const;

    private:
        // G4GenericMessenger *fMessenger = nullptr;
        // G4ThreeVector GetEntranceFringingField(G4ThreeVector pos) const;
        // G4ThreeVector GetUniformZoneField(G4ThreeVector pos) const;
        // G4ThreeVector GetExitFringingField(G4ThreeVector pos) const;
        G4ThreeVector GetQuadrupoleField(G4ThreeVector pos) const;
        G4ThreeVector GetHighOrderField(G4ThreeVector pos) const;

        G4double fMultipoleProbe;
        G4double fMdmAngle;
        G4ThreeVector fFirstMultipolePos;
        std::unique_ptr<G4RotationMatrix> fFirstMultipoleRot;
        G4double fBQR;  // quadrupole at r=R;
        G4double fBHR;  // hexapole
        G4double fBOR;  // qctapole
        G4double fBDR;  // decapole
        G4double fBDDR; // dodecapole
        G4double fG1;   // quadrupole at r=R;
        G4double fG2;   // hexapole
        G4double fG3;   // qctapole
        G4double fG4;   // decapole
        G4double fG5;   // dodecapole
        std::unique_ptr<TF1> fEngeFunc;
    };

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
