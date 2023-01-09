#include "PpacSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
namespace MdmSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PpacSD::PpacSD(const G4String &name,
                   const G4String &hitsCollectionName)
        : G4VSensitiveDetector(name)
    {
        collectionName.insert(hitsCollectionName);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PpacSD::~PpacSD()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void PpacSD::Initialize(G4HCofThisEvent *hce)
    {
        // Create hits collection

        fHitsCollection = new PpacHitsCollection(SensitiveDetectorName, collectionName[0]);

        // Add this collection in hce

        G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        hce->AddHitsCollection(hcID, fHitsCollection);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4bool PpacSD::ProcessHits(G4Step *aStep,
                               G4TouchableHistory *)
    {
        // energy deposit
        G4double edep = aStep->GetTotalEnergyDeposit();

        if (edep == 0.)
            return false;

        PpacHit *newHit = new PpacHit();

        // Copy number
        newHit->SetCopyNo(aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetCopyNo());

        // Track ID
        newHit->SetTrackID(aStep->GetTrack()->GetTrackID());

        // Hit position
        newHit->SetGlobalPosition(aStep->GetPreStepPoint()->GetPosition());
        G4ThreeVector volumePos = fPpacChamberRot->operator()(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation()) + fPpacChamberPos;
        G4ThreeVector translatedPosition = aStep->GetPreStepPoint()->GetPosition() - volumePos;
        G4ThreeVector rotatedPosition = fPpacChamberRot->inverse().operator()(translatedPosition);
        newHit->SetLocalPosition(rotatedPosition);

        // G4String volumeName=aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
        // G4cout << "PpacSD::ProcessHits(): volumeName=" << volumeName << G4endl;
        // G4cout << "PpacSD::ProcessHits(): volumePosInChamber=" << G4BestUnit(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation(), "Length") << G4endl;
        // G4cout << "PpacSD::ProcessHits(): volumePosRotated=" << G4BestUnit(fPpacChamberRot->operator()(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation()), "Length") << G4endl;
        // G4cout << "PpacSD::ProcessHits(): volumePosInWorld=" << G4BestUnit(volumePos, "Length") << G4endl;
        // G4cout << "PpacSD::ProcessHits(): stepPos=" << G4BestUnit(aStep->GetPreStepPoint()->GetPosition(), "Length") << G4endl;
        // G4cout << "PpacSD::ProcessHits(): translatedPosition=" << G4BestUnit(translatedPosition, "Length") << G4endl;
        // G4cout << "PpacSD::ProcessHits(): rotatedPosition=" << G4BestUnit(rotatedPosition, "Length") << G4endl;

        // Hit momentum
        newHit->SetGlobalMomentum(aStep->GetTrack()->GetMomentum());
        G4ThreeVector rotatedMomentum = fPpacChamberRot->inverse().operator()(aStep->GetTrack()->GetMomentum());
        newHit->SetLocalMomentum(rotatedMomentum);

        // Energy deposition
        newHit->SetEnergyDeposit(edep);

        // Hit time
        newHit->SetTime(aStep->GetTrack()->GetGlobalTime());

        fHitsCollection->insert(newHit);

        // newHit->Print();

        return true;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void PpacSD::EndOfEvent(G4HCofThisEvent *)
    {
        return;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    void PpacSD::SetPpacChamberPosRot(G4RotationMatrix *rot, G4ThreeVector pos)
    {
        fPpacChamberRot = rot;
        fPpacChamberPos = pos;
    }

}
