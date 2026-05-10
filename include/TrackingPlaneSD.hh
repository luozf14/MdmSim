#ifndef MdmSimTrackingPlaneSD_h
#define MdmSimTrackingPlaneSD_h 1

#include "PpacHit.hh"

#include "G4RotationMatrix.hh"
#include "G4VSensitiveDetector.hh"
#include "globals.hh"

namespace MdmSim
{
    class TrackingPlaneSD : public G4VSensitiveDetector
    {
    public:
        TrackingPlaneSD(const G4String &name, const G4String &hitsCollectionName);
        ~TrackingPlaneSD() override;

        void Initialize(G4HCofThisEvent *hce) override;
        G4bool ProcessHits(G4Step *step, G4TouchableHistory *history) override;
        void EndOfEvent(G4HCofThisEvent *) override;

        void SetReferenceFrame(G4RotationMatrix *rot, G4ThreeVector pos);

    private:
        PpacHitsCollection *fHitsCollection = nullptr;
        G4RotationMatrix *fReferenceRot = nullptr;
        G4ThreeVector fReferencePos;
    };
}

#endif
