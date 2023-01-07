#ifndef MdmSimDetectorConstruction_h
#define MdmSimDetectorConstruction_h 1

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

namespace MdmSim
{
    class DipoleField;
    class FirstMultipoleField;

    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

        G4VPhysicalVolume *Construct() override;
        virtual void ConstructSDandField();
        void ParseParams(std::map<std::string, G4double> params);
        // void SetMdmAngle(G4double ang);

    private:
        static G4ThreadLocal DipoleField *fDipoleField;
        static G4ThreadLocal FirstMultipoleField *fFirstMultipoleField;

        static G4ThreadLocal G4FieldManager *fDipoleFieldMgr;
        static G4ThreadLocal G4FieldManager *fFirstMultipoleFieldMgr;

        G4double fTargetRotationAngle;
        G4double fTargetThickness;

        G4bool fUseDeltaE;
        G4double fSiDetectorAngle;

        G4double fMdmAngle;

        G4double fFirstMultipoleProbe;
        G4double fDipoleProbe;

        G4LogicalVolume *fLogicFirstMultipoleField = nullptr;
        G4VPhysicalVolume *fPhysicFirstMultipoleField = nullptr;
        G4ThreeVector fMultipoleFieldPos;

        G4LogicalVolume *fLogicDipoleField = nullptr;
        G4VPhysicalVolume *fPhysicDipoleField = nullptr;
        G4ThreeVector fDipoleFieldPos;

        G4LogicalVolume *fLogicDipoleMagnet = nullptr;
        G4VPhysicalVolume *fPhysicDipoleMagnet = nullptr;

        G4double fPpacVacuum;
        G4double fPpacLength;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
