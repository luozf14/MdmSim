#ifndef TexPPACSimDetectorConstruction_h
#define TexPPACSimDetectorConstruction_h 1

#include "G4FieldManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"

#include "globals.hh"

class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4GenericMessenger;
/// Detector construction class to define materials and geometry.

namespace TexPPACSim
{
    class DipoleField;

    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

        G4VPhysicalVolume *Construct() override;
        virtual void ConstructSDandField();
        void ParseParams(std::map<std::string, G4double> params);
        void SetMdmAngle(G4double ang);

    private:
        void DefineCommands();
        G4GenericMessenger *fMessenger = nullptr;

        static G4ThreadLocal DipoleField *fDipoleField;
        static G4ThreadLocal G4FieldManager *fFieldMgr;

        G4double fTargetRotationAngle;
        G4double fTargetThickness;

        G4bool fUseDeltaE;
        G4double fSiDetectorAngle;

        G4double fMdmAngle;
        G4RotationMatrix *fMdmRotation = nullptr;
        G4LogicalVolume *fLogicDipole = nullptr;
        G4VPhysicalVolume *fPhysicDipole = nullptr;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
