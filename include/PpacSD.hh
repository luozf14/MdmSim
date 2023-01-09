#ifndef MdmSimPpacSD_h
#define MdmSimPpacSD_h 1

#include "G4VSensitiveDetector.hh"

#include "PpacHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

namespace MdmSim
{

    /// Ppac sensitive detector class
    ///
    /// The hits are accounted in hits in ProcessHits() function which is called
    /// by Geant4 kernel at each step. A hit is created with each step with non zero
    /// energy deposit.

    class PpacSD : public G4VSensitiveDetector
    {
    public:
        PpacSD(const G4String &name,
                     const G4String &hitsCollectionName);
        ~PpacSD() override;

        // methods from base class
        void Initialize(G4HCofThisEvent *hitCollection) override;
        G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;
        void EndOfEvent(G4HCofThisEvent *hitCollection) override;
        void SetPpacChamberPosRot(G4RotationMatrix* rot, G4ThreeVector pos);

    private:
        PpacHitsCollection *fHitsCollection = nullptr;
        G4RotationMatrix *fPpacChamberRot;
        G4ThreeVector fPpacChamberPos;
    };

}

#endif
