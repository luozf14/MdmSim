#include "TrackingPlaneSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"

namespace MdmSim
{
    TrackingPlaneSD::TrackingPlaneSD(const G4String &name,
                                     const G4String &hitsCollectionName)
        : G4VSensitiveDetector(name)
    {
        collectionName.insert(hitsCollectionName);
    }

    TrackingPlaneSD::~TrackingPlaneSD()
    {
    }

    void TrackingPlaneSD::Initialize(G4HCofThisEvent *hce)
    {
        fHitsCollection = new PpacHitsCollection(SensitiveDetectorName, collectionName[0]);
        const G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        hce->AddHitsCollection(hcID, fHitsCollection);
    }

    G4bool TrackingPlaneSD::ProcessHits(G4Step *step, G4TouchableHistory *)
    {
        auto *hit = new PpacHit();
        hit->SetCopyNo(step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetCopyNo());
        hit->SetTrackID(step->GetTrack()->GetTrackID());
        hit->SetEnergyDeposit(step->GetTotalEnergyDeposit());
        hit->SetTime(step->GetTrack()->GetGlobalTime());

        const G4ThreeVector globalPosition = step->GetPreStepPoint()->GetPosition();
        const G4ThreeVector globalMomentum = step->GetTrack()->GetMomentum();
        hit->SetGlobalPosition(globalPosition);
        hit->SetGlobalMomentum(globalMomentum);

        if (fReferenceRot)
        {
            hit->SetLocalPosition(fReferenceRot->inverse()(globalPosition - fReferencePos));
            hit->SetLocalMomentum(fReferenceRot->inverse()(globalMomentum));
        }
        else
        {
            hit->SetLocalPosition(globalPosition);
            hit->SetLocalMomentum(globalMomentum);
        }

        fHitsCollection->insert(hit);
        return true;
    }

    void TrackingPlaneSD::EndOfEvent(G4HCofThisEvent *)
    {
    }

    void TrackingPlaneSD::SetReferenceFrame(G4RotationMatrix *rot, G4ThreeVector pos)
    {
        fReferenceRot = rot;
        fReferencePos = pos;
    }
}
