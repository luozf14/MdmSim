#ifndef MdmSimSiDetectorSD_h
#define MdmSimSiDetectorSD_h 1

#include "G4VSensitiveDetector.hh"

#include "SiDetectorHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

namespace MdmSim
{

    /// SiDetector sensitive detector class
    ///
    /// The hits are accounted in hits in ProcessHits() function which is called
    /// by Geant4 kernel at each step. A hit is created with each step with non zero
    /// energy deposit.

    class SiDetectorSD : public G4VSensitiveDetector
    {
    public:
        SiDetectorSD(const G4String &name,
                     const G4String &hitsCollectionName);
        ~SiDetectorSD() override;

        // methods from base class
        void Initialize(G4HCofThisEvent *hitCollection) override;
        G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;
        void EndOfEvent(G4HCofThisEvent *hitCollection) override;

    private:
        SiDetectorHitsCollection *fHitsCollection = nullptr;
    };

}

#endif
