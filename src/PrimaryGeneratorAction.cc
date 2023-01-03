#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PrimaryGeneratorAction::PrimaryGeneratorAction()
    {
        G4int n_particle = 1;
        fParticleGun = new G4ParticleGun(n_particle);

        // default particle kinematic
        G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
        G4String particleName;
        G4ParticleDefinition *particle = particleTable->FindParticle(particleName = "proton");
        fParticleGun->SetParticleDefinition(particle);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PrimaryGeneratorAction::~PrimaryGeneratorAction()
    {
        delete fParticleGun;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
    {
        // this function is called at the begining of ecah event
        //

        // In order to avoid dependence of PrimaryGeneratorAction
        // on DetectorConstruction class we get Envelope volume
        // from G4LogicalVolumeStore.

        G4ThreeVector direction(1., 0, 0);
        direction.setPhi(G4RandFlat::shoot(-180 * deg, 180 * deg));
        direction.setTheta(std::acos(G4RandFlat::shoot(std::cos(1.5 * deg), std::cos(0. * deg))));
        fParticleGun->SetParticleMomentumDirection(direction);

        fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -1.));
        fParticleGun->SetParticleEnergy(G4RandGauss::shoot(20. * MeV, 0.005 * 20. * MeV / 2.355));

        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
