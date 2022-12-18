#ifndef TexPPACSimDetectorConstruction_h
#define TexPPACSimDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

namespace TexPPACSim
{

    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

        G4VPhysicalVolume *Construct() override;
        virtual void ConstructSDandField();
        void ParseParams(std::map<std::string, G4double> params);

    private:
        G4double fTargetRotationAngle;
        G4double fTargetThickness;
        G4bool fUseDeltaE;
        G4double fSiDetectorAngle;
        G4double fMdmAngle;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
#endif
