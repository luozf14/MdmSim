#ifndef MdmSimSiDetectorHit_h
#define MdmSimSiDetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

namespace MdmSim
{

    /// SiDetector hit class
    ///
    /// It defines data members to store the trackID, energy deposit, kinetic energy, momentum,
    /// and position of charged particles in a selected volume:
    /// - fTrackID, fEnergyDeposit, fKineticEnergy, fMomentum, fPosition

    class SiDetectorHit : public G4VHit
    {
    public:
        SiDetectorHit();
        SiDetectorHit(const SiDetectorHit &) = default;
        ~SiDetectorHit() override;

        // operators
        SiDetectorHit &operator=(const SiDetectorHit &) = default;
        G4bool operator==(const SiDetectorHit &) const;

        inline void *operator new(size_t);
        inline void operator delete(void *);

        // methods from base class
        void Print() override;

        // Set methods
        void SetTrackID(G4int track) { fTrackID = track; };
        void SetEnergyDeposit(G4double de) { fEnergyDeposit = de; };
        void SetTime(G4double t) { fTime = t; };
        void SetMass(G4int mass) { fMass = mass; };
        void SetCharge(G4int charge) { fCharge = charge; };
        void SetKineticEnergy(G4double e) { fKineticEnergy = e; };
        void SetGlobalPosition(G4ThreeVector xyz) { fGlobalPosition = xyz; };
        void SetLocalPosition(G4ThreeVector xyz) { fLocalPosition = xyz; };
        void SetGlobalMomentum(G4ThreeVector xyz) { fGlobalMomentum = xyz; };
        void SetLocalMomentum(G4ThreeVector xyz) { fLocalMomentum = xyz; };

        // Get methods
        G4int GetTrackID() const { return fTrackID; };
        G4double GetEnergyDeposit() const { return fEnergyDeposit; };
        G4double GetTime() const { return fTime; };
        G4int GetMass() const { return fMass; };
        G4int GetCharge() const { return fCharge; };
        G4double GetKineticEnergy() const { return fKineticEnergy; };
        G4ThreeVector GetLocalMomentum() const { return fLocalMomentum; };
        G4ThreeVector GetGlobalMomentum() const { return fGlobalMomentum; };
        G4ThreeVector GetGlobalPosition() const { return fGlobalPosition; };
        G4ThreeVector GetLocalPosition() const { return fLocalPosition; };

    private:
        G4int fTrackID;
        G4double fEnergyDeposit;
        G4double fTime;
        G4int fMass;
        G4int fCharge;
        G4double fKineticEnergy;
        G4ThreeVector fGlobalPosition;
        G4ThreeVector fLocalPosition;
        G4ThreeVector fGlobalMomentum;
        G4ThreeVector fLocalMomentum;
    };

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    typedef G4THitsCollection<SiDetectorHit> SiDetectorHitsCollection;

    extern G4ThreadLocal G4Allocator<SiDetectorHit> *SiDetectorHitAllocator;

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    inline void *SiDetectorHit::operator new(size_t)
    {
        if (!SiDetectorHitAllocator)
            SiDetectorHitAllocator = new G4Allocator<SiDetectorHit>;
        return (void *)SiDetectorHitAllocator->MallocSingle();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    inline void SiDetectorHit::operator delete(void *hit)
    {
        SiDetectorHitAllocator->FreeSingle((SiDetectorHit *)hit);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

#endif
