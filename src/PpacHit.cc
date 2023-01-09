#include "PpacHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace MdmSim
{

    G4ThreadLocal G4Allocator<PpacHit> *PpacHitAllocator = nullptr;

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PpacHit::PpacHit()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PpacHit::~PpacHit() {}

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4bool PpacHit::operator==(const PpacHit &right) const
    {
        return (this == &right) ? true : false;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void PpacHit::Print()
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
