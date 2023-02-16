#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4ChargedGeantino.hh"

namespace MdmSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    PrimaryGeneratorAction::PrimaryGeneratorAction()
    {
        G4int n_particle = 1;
        fParticleGun = new G4ParticleGun(n_particle);

        // default particle kinematic
        G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition *particle = particleTable->FindParticle("chargedgeantino");
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
        G4ParticleDefinition *particle = fParticleGun->GetParticleDefinition();
        if (particle == G4ChargedGeantino::ChargedGeantino())
        {
            G4ParticleDefinition *ion;
            G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
            G4int Z = 1, A = 1;
            if (particleTable->GetIonTable()->FindIon(Z, A, 0.0))
                ion = particleTable->GetIonTable()->FindIon(Z, A, 0.0);
            else
                ion = particleTable->GetIonTable()->GetIon(Z, A, 0.0);
            // ion = particleTable->FindParticle("alpha");
            fParticleGun->SetParticleDefinition(ion);
        }

        G4double xAngle = G4RandFlat::shoot(-8. * deg, 8. * deg);
        G4double yAngle = G4RandFlat::shoot(-4. * deg, 4. * deg);
        // G4double yAngle = 0.;
        G4ThreeVector direction(std::tan(xAngle), std::tan(yAngle) * std::sqrt(std::tan(xAngle) * std::tan(xAngle) + 1.), 1.);
        // G4ThreeVector direction(0., 0., 1.);
        // direction.setTheta(std::acos(G4RandFlat::shoot(std::cos(8 * deg), std::cos(0. * deg))));
        // G4double phi = G4RandFlat::shoot(-180. * deg, 180. * deg);
        // G4double phi = G4RandFlat::shoot() > 0.5 ? 90. * deg : -90. * deg;
        // direction.setPhi(phi);
        fParticleGun->SetParticleMomentumDirection(direction);

        fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -1.));
        // fParticleGun->SetParticleEnergy(G4RandGauss::shoot(20. * MeV, 0.005 * 20. * MeV / 2.355));
        fParticleGun->SetParticleEnergy(20. * MeV);

        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
