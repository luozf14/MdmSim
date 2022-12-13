//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the TexPPACSim::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4UserLimits.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorConstruction::DetectorConstruction()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorConstruction::~DetectorConstruction()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4VPhysicalVolume *DetectorConstruction::Construct()
    {
        printf("fTargetRotationAngle: %.1f\n", fTargetRotationAngle / M_PI * 180.);
        printf("fTargetThickness: %.1f\n", fTargetThickness);
        printf("fSiDetectorAngle: %.1f\n", fSiDetectorAngle / M_PI * 180.);
        printf("fMdmAngle: %.1f\n", fMdmAngle / M_PI * 180.);

        // Get nist material manager
        G4NistManager *nist = G4NistManager::Instance();

        // Option to switch on/off checking of volumes overlaps
        //
        G4bool checkOverlaps = false;

        //
        // World
        //
        G4double world_sizeXY = 2. * m;
        G4double world_sizeZ = 2. * m;
        G4Material *world_mat = nist->FindOrBuildMaterial("G4_Galactic");

        G4Box *solidWorld =
            new G4Box("World",                                                    // its name
                      0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ); // its size

        G4LogicalVolume *logicWorld =
            new G4LogicalVolume(solidWorld, // its solid
                                world_mat,  // its material
                                "World");   // its name

        G4VPhysicalVolume *physWorld =
            new G4PVPlacement(0,               // no rotation
                              G4ThreeVector(), // at (0,0,0)
                              logicWorld,      // its logical volume
                              "World",         // its name
                              0,               // its mother  volume
                              false,           // no boolean operation
                              0,               // copy number
                              checkOverlaps);  // overlaps checking

        //
        // Target
        //
        G4Material *targetMaterial = new G4Material("Carbon", 2.253 * g / cm3, nist->FindOrBuildMaterial("G4_C"));
        G4RotationMatrix *targetRot = new G4RotationMatrix;
        targetRot->rotateY(fTargetRotationAngle);
        G4double targetRMin = 0.;
        G4double targetRMax = 3.5 * mm;
        G4double targetDz = fTargetThickness / (2.253 * g / cm3);
        G4cout << "targetDz: " << G4BestUnit(targetDz, "Length") << G4endl;
        G4Tubs *solidTarget = new G4Tubs("Target",                                                      // its name
                                         targetRMin, targetRMax, 0.5 * targetDz, 2. * M_PI, 2. * M_PI); // its size

        G4LogicalVolume *logicTarget =
            new G4LogicalVolume(solidTarget,    // its solid
                                targetMaterial, // its material
                                "Target");      // its name

        new G4PVPlacement(targetRot,       // rotation
                          G4ThreeVector(), // at (0,0,0)
                          logicTarget,     // its logical volume
                          "Target",        // its name
                          logicWorld,      // its mother  volume
                          false,           // no boolean operation
                          0,               // copy number
                          checkOverlaps);  // overlaps checking

        G4double maxStep = 0.1 * targetDz;
        logicTarget->SetUserLimits(new G4UserLimits(maxStep));

        //
        // Si detector - Delta E
        //
        G4ThreeVector DeltaEPos = G4ThreeVector(0., 0., 13.724 * cm);
        DeltaEPos.setTheta(fSiDetectorAngle);
        DeltaEPos.setPhi(180. * deg);
        // Rotate the dE detector
        G4RotationMatrix *DeltaERot = new G4RotationMatrix;
        DeltaERot->rotateY(fSiDetectorAngle);
        // Box shape
        G4double DeltaEX = 5. * cm;
        G4double DeltaEY = 5. * cm;
        G4double DeltaEZ = 32. * um;

        G4Box *solidDeltaE =
            new G4Box("DetdE",
                      0.5 * DeltaEX, 0.5 * DeltaEY, 0.5 * DeltaEZ);

        G4LogicalVolume *logicDeltaE =
            new G4LogicalVolume(solidDeltaE,                        // its solid
                                nist->FindOrBuildMaterial("G4_Si"), // its material
                                "SiDetectorDeltaE");                // its name

        new G4PVPlacement(DeltaERot,          // rotation
                          DeltaEPos,          // at position
                          logicDeltaE,        // its logical volume
                          "SiDetectorDeltaE", // its name
                          logicWorld,         // its mother  volume
                          false,              // no boolean operation
                          0,                  // copy number
                          checkOverlaps);     // overlaps checking

        //
        // Si detector - E
        //
        G4ThreeVector EPos = G4ThreeVector(0., 0., 14.224 * cm);
        EPos.setTheta(fSiDetectorAngle);
        EPos.setPhi(180. * deg);
        // Rotate the dE detector
        G4RotationMatrix *ERot = new G4RotationMatrix;
        ERot->rotateY(fSiDetectorAngle);
        // Box shape
        G4double EX = 5. * cm;
        G4double EY = 5. * cm;
        G4double EZ = 500. * um;

        G4Box *solidE =
            new G4Box("DetdE",
                      0.5 * EX, 0.5 * EY, 0.5 * EZ);

        G4LogicalVolume *logicE =
            new G4LogicalVolume(solidE,                             // its solid
                                nist->FindOrBuildMaterial("G4_Si"), // its material
                                "SiDetectorE");                     // its name

        new G4PVPlacement(ERot,           // rotation
                          EPos,           // at position
                          logicE,         // its logical volume
                          "SiDetectorE",  // its name
                          logicWorld,     // its mother  volume
                          false,          // no boolean operation
                          0,              // copy number
                          checkOverlaps); // overlaps checking

        //
        // MDM slit box
        //
        G4double slitBoxDistance = 63.5 * cm;
        G4ThreeVector slitBoxPos = G4ThreeVector(slitBoxDistance + 0.5 * mm, 0., 0.); // position in mother frame
        slitBoxPos.setTheta(-1. * fMdmAngle);
        slitBoxPos.setPhi(0. * deg);
        G4RotationMatrix *slitBoxRot = new G4RotationMatrix;
        slitBoxRot->rotateY(-1. * fMdmAngle);

        G4VSolid *solidSlitBox = new G4Box("SlitBox", 2.27965 * cm, 2.27965 * cm, 1. * mm); // measured on 1/26/2022
        G4LogicalVolume *logicSlitBox = new G4LogicalVolume(solidSlitBox, nist->FindOrBuildMaterial("G4_Galactic"), "SlitBox");
        new G4PVPlacement(slitBoxRot,     // rotation
                          slitBoxPos,     // at position
                          logicSlitBox,   // its logical volume
                          "SlitBox",      // its name
                          logicWorld,     // its mother  volume
                          false,          // no boolean operation
                          0,              // copy number
                          checkOverlaps); // overlaps checking

        //
        // always return the physical World
        //
        return physWorld;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DetectorConstruction::ParseParams(std::map<std::string, double> params)
    {
        G4cout << "-----In DetectorConstruction::ParseParams-----" << G4endl;
        for (auto it : params)
        {
            if (it.first == "TargetRotationAngleInDeg")
            {
                fTargetRotationAngle = it.second * deg;
                G4cout << "Set: Target rotation angle = " << G4BestUnit(fTargetRotationAngle, "Angle") << G4endl;
            }
            else if (it.first == "TargetThicknessInMgCm2")
            {
                fTargetThickness = it.second * mg / cm2;
                G4cout << "Set: Target thickness = " << G4BestUnit(fTargetThickness, "Mass/Surface") << G4endl;
            }
            else if (it.first == "SiDetectorAngleInDeg")
            {
                fSiDetectorAngle = it.second * deg;
                G4cout << "Set: Si detector angle = " << G4BestUnit(fSiDetectorAngle, "Angle") << G4endl;
            }
            else if (it.first == "MdmAngleInDeg")
            {
                fMdmAngle = it.second * deg;
                G4cout << "Set: MDM angle = " << G4BestUnit(fMdmAngle, "Angle") << G4endl;
            }
        }
    }

}
