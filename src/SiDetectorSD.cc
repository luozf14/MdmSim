#include "SiDetectorSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

namespace MdmSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SiDetectorSD::SiDetectorSD(const G4String &name,
                               const G4String &hitsCollectionName)
        : G4VSensitiveDetector(name)
    {
        collectionName.insert(hitsCollectionName);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SiDetectorSD::~SiDetectorSD()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void SiDetectorSD::Initialize(G4HCofThisEvent *hce)
    {
        // Create hits collection

        fHitsCollection = new SiDetectorHitsCollection(SensitiveDetectorName, collectionName[0]);

        // Add this collection in hce

        G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        hce->AddHitsCollection(hcID, fHitsCollection);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4bool SiDetectorSD::ProcessHits(G4Step *aStep,
                                     G4TouchableHistory *)
    {
        // energy deposit
        G4double edep = aStep->GetTotalEnergyDeposit();

        if (edep == 0.)
            return false;

        SiDetectorHit *newHit = new SiDetectorHit();

        // Track ID
        newHit->SetTrackID(aStep->GetTrack()->GetTrackID());

        // Hit position
        newHit->SetGlobalPosition(aStep->GetPreStepPoint()->GetPosition());
        G4ThreeVector translatedPosition = aStep->GetPreStepPoint()->GetPosition() - aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectTranslation();
        G4ThreeVector rotatedPosition = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectRotation()->inverse().operator()(translatedPosition);
        newHit->SetLocalPosition(rotatedPosition);

        // Hit momentum
        newHit->SetGlobalMomentum(aStep->GetTrack()->GetMomentum());
        G4ThreeVector rotatedMomentum = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetObjectRotation()->inverse().operator()(aStep->GetTrack()->GetMomentum());
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

    void SiDetectorSD::EndOfEvent(G4HCofThisEvent *)
    {
        return;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
