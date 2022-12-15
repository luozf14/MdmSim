#include "SiDetectorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace TexPPACSim
{

    G4ThreadLocal G4Allocator<SiDetectorHit> *SiDetectorHitAllocator = nullptr;

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SiDetectorHit::SiDetectorHit()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    SiDetectorHit::~SiDetectorHit() {}

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4bool SiDetectorHit::operator==(const SiDetectorHit &right) const
    {
        return (this == &right) ? true : false;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void SiDetectorHit::Print()
    {
        G4cout
            << "  TrackID: " << fTrackID
            << "  Energy deposit: "
            << std::setw(7) << G4BestUnit(fEnergyDeposit, "Energy")
            //  <<fEnergyDeposit
            << "  Local position: "
            << std::setw(7) << G4BestUnit(fLocalPosition, "Length")
            << G4endl;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
