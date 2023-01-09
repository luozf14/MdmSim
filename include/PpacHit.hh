#ifndef MdmSimPpacHit_h
#define MdmSimPpacHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

namespace MdmSim
{

    /// PPAC hit class
    ///
    /// It defines data members to store the trackID, energy deposit, kinetic energy, momentum,
    /// and position of charged particles in a selected volume:
    /// - fTrackID, fEnergyDeposit, fKineticEnergy, fMomentum, fPosition

    class PpacHit : public G4VHit
    {
    public:
        PpacHit();
        PpacHit(const PpacHit &) = default;
        ~PpacHit() override;

        // operators
        PpacHit &operator=(const PpacHit &) = default;
        G4bool operator==(const PpacHit &) const;

        inline void *operator new(size_t);
        inline void operator delete(void *);

        // methods from base class
        void Print() override;

        // Set methods
        void SetCopyNo(G4int no) { fCopyNo = no; };
        void SetTrackID(G4int track) { fTrackID = track; };
        void SetEnergyDeposit(G4double de) { fEnergyDeposit = de; };
        void SetGlobalPosition(G4ThreeVector xyz) { fGlobalPosition = xyz; };
        void SetLocalPosition(G4ThreeVector xyz) { fLocalPosition = xyz; };
        void SetGlobalMomentum(G4ThreeVector xyz) { fGlobalMomentum = xyz; };
        void SetLocalMomentum(G4ThreeVector xyz) { fLocalMomentum = xyz; };
        void SetTime(G4double t) { fTime = t; };

        // Get methods
        G4int GetCopyNo() const { return fCopyNo; };
        G4int GetTrackID() const { return fTrackID; };
        G4double GetEnergyDeposit() const { return fEnergyDeposit; };
        G4ThreeVector GetLocalMomentum() const { return fLocalMomentum; };
        G4ThreeVector GetGlobalMomentum() const { return fGlobalMomentum; };
        G4ThreeVector GetGlobalPosition() const { return fGlobalPosition; };
        G4ThreeVector GetLocalPosition() const { return fLocalPosition; };
        G4double GetTime() const { return fTime; };

    private:
        G4int fCopyNo;
        G4int fTrackID;
        G4ThreeVector fGlobalPosition;
        G4ThreeVector fLocalPosition;
        G4ThreeVector fGlobalMomentum;
        G4ThreeVector fLocalMomentum;
        G4double fEnergyDeposit;
        G4double fTime;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    typedef G4THitsCollection<PpacHit> PpacHitsCollection;

    extern G4ThreadLocal G4Allocator<PpacHit> *PpacHitAllocator;

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    inline void *PpacHit::operator new(size_t)
    {
        if (!PpacHitAllocator)
            PpacHitAllocator = new G4Allocator<PpacHit>;
        return (void *)PpacHitAllocator->MallocSingle();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    inline void PpacHit::operator delete(void *hit)
    {
        PpacHitAllocator->FreeSingle((PpacHit *)hit);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

#endif
