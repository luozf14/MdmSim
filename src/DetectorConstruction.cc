#include "DetectorConstruction.hh"
#include "SiDetectorSD.hh"
#include "DipoleField.hh"
#include "FirstMultipoleField.hh"
#include "Constants.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4GenericMessenger.hh"

#include "G4NistManager.hh"

#include "G4RunManager.hh"
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
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"

namespace TexPPACSim
{
    G4ThreadLocal DipoleField *DetectorConstruction::fDipoleField = 0;
    G4ThreadLocal G4FieldManager *DetectorConstruction::fDipoleFieldMgr = 0;
    G4ThreadLocal FirstMultipoleField *DetectorConstruction::fFirstMultipoleField = 0;
    G4ThreadLocal G4FieldManager *DetectorConstruction::fFirstMultipoleFieldMgr = 0;
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
        // Get nist material manager
        G4NistManager *nist = G4NistManager::Instance();

        // Option to switch on/off checking of volumes overlaps
        //
        G4bool checkOverlaps = false;

        //
        // World
        //
        G4double world_sizeXY = 10. * m;
        G4double world_sizeZ = 15. * m;
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
        // G4Material *targetMaterial = new G4Material("Carbon", 2.253 * g / cm3, nist->FindOrBuildMaterial("G4_C"));
        G4Material *targetMaterial = nist->FindOrBuildMaterial("G4_Galactic");
        G4RotationMatrix *targetRot = new G4RotationMatrix;
        targetRot->rotateY(fTargetRotationAngle);
        G4double targetRMin = 0.;
        G4double targetRMax = 3.5 * mm;
        G4double targetDz = fTargetThickness / (2.253 * g / cm3);
        G4cout << "\n---> DetectorConstruction::Construct(): targetDz= " << G4BestUnit(targetDz, "Length") << G4endl;
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

        logicTarget->SetUserLimits(new G4UserLimits(0.1 * targetDz));

        //
        // Si detector - Delta E
        //
        G4Material *DeltaEMaterial = fUseDeltaE ? nist->FindOrBuildMaterial("G4_Si") : nist->FindOrBuildMaterial("G4_Galactic");
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
            new G4LogicalVolume(solidDeltaE,         // its solid
                                DeltaEMaterial,      // its material
                                "SiDetectorDeltaE"); // its name

        new G4PVPlacement(DeltaERot,          // rotation
                          DeltaEPos,          // at position
                          logicDeltaE,        // its logical volume
                          "SiDetectorDeltaE", // its name
                          logicWorld,         // its mother  volume
                          false,              // no boolean operation
                          0,                  // copy number
                          checkOverlaps);     // overlaps checking

        logicDeltaE->SetUserLimits(new G4UserLimits(0.5 * DeltaEZ));

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

        logicE->SetUserLimits(new G4UserLimits(0.5 * EZ));

        //
        // MDM slit box
        //
        G4double slitBoxDistance = 63.5 * cm;
        G4ThreeVector slitBoxPos = G4ThreeVector(slitBoxDistance + 0.5 * mm, 0., 0.); // position in mother frame
        slitBoxPos.setTheta(-1. * fMdmAngle);
        slitBoxPos.setPhi(0. * deg);
        G4RotationMatrix *slitBoxRot = new G4RotationMatrix;
        slitBoxRot->rotateY(-1. * fMdmAngle);

        G4VSolid *solidSlitBoxVoid = new G4Box("SlitBoxVoid", 2.27965 * cm, 2.27965 * cm, 1. * cm); // measured on 1/26/2022
        G4VSolid *solidSlitBoxShape = new G4Box("SlitBoxShape", (2.27965 + 5.) * cm, (2.27965 + 5.) * cm, 1. * cm);
        G4VSolid *solidSlitBox = new G4SubtractionSolid("SlitBox", solidSlitBoxShape, solidSlitBoxVoid);
        G4LogicalVolume *logicSlitBox = new G4LogicalVolume(solidSlitBox, nist->FindOrBuildMaterial("G4_Cu"), "SlitBox");
        new G4PVPlacement(slitBoxRot,     // rotation
                          slitBoxPos,     // at position
                          logicSlitBox,   // its logical volume
                          "SlitBox",      // its name
                          logicWorld,     // its mother  volume
                          false,          // no boolean operation
                          0,              // copy number-m ""
                          checkOverlaps); // overlaps checking

        //
        // First multipole
        //
        G4VSolid *solidFirstMultipoleField = new G4Tubs("FirstMultipoleFieldTubs", 0., kFirstMultipoleAperture, kFirstMultipoleLength, 0., 360. * deg);
        fLogicFirstMultipoleField = new G4LogicalVolume(solidFirstMultipoleField, nist->FindOrBuildMaterial("G4_Galactic"), "FirstMultipoleFieldLogical");
        fMultipoleFieldPos.setX(kFirstMultipolePos + 0.5 * kFirstMultipoleLength);
        fMultipoleFieldPos.setY(0.);
        fMultipoleFieldPos.setZ(0.);
        fMultipoleFieldPos.setTheta(fMdmAngle);
        fMultipoleFieldPos.setPhi(0. * deg);
        G4RotationMatrix *firstMultipoleFieldRot = new G4RotationMatrix;
        firstMultipoleFieldRot->rotateY(fMdmAngle);
        fPhysicFirstMultipoleField = new G4PVPlacement(G4Transform3D(*firstMultipoleFieldRot, fMultipoleFieldPos), fLogicFirstMultipoleField,
                                                       "FirstMultipolePhysical", logicWorld,
                                                       false, 0, checkOverlaps);
        fLogicFirstMultipoleField->SetUserLimits(new G4UserLimits(1. * cm));

        //
        // Dipole field
        //
        G4VSolid *solidDipoleField = new G4Tubs("DipoleFieldTubs", kDipoleFieldRadius - 0.5 * kDipoleFieldWidth, kDipoleFieldRadius + 0.5 * kDipoleFieldWidth, kDipoleFieldHeight, 0., kDipoleDeflectionAngle);
        fLogicDipoleField = new G4LogicalVolume(solidDipoleField, nist->FindOrBuildMaterial("G4_Galactic"), "DipoleFieldLogical");
        fDipoleFieldPos.setX(-kDipoleFieldRadius);
        fDipoleFieldPos.setY(0.);
        fDipoleFieldPos.setZ(kFirstArmLength);
        fDipoleFieldPos.rotateY(fMdmAngle);
        G4RotationMatrix *dipoleRot = new G4RotationMatrix;
        dipoleRot->rotateX(90. * deg);
        dipoleRot->rotateY(fMdmAngle);
        fPhysicDipoleField = new G4PVPlacement(G4Transform3D(*dipoleRot, fDipoleFieldPos), fLogicDipoleField,
                                               "DipoleFieldPhysical", logicWorld,
                                               false, 0, checkOverlaps);
        fLogicDipoleField->SetUserLimits(new G4UserLimits(1. * cm));

        //
        // Dipole magnet
        //
        auto solidDipoleMagnetShape = new G4Tubs("DipoleMagnetShape", 0., kDipoleMagnetRadius, 0.5 * m, 0., kDipoleDeflectionAngle);
        auto solidDipoleMagnet = new G4SubtractionSolid("DipoleMagnetSolid", solidDipoleMagnetShape, solidDipoleField);
        fLogicDipoleMagnet = new G4LogicalVolume(solidDipoleMagnet, nist->FindOrBuildMaterial("G4_Cu"), "DipoleMagnetLogical");
        fPhysicDipoleMagnet = new G4PVPlacement(G4Transform3D(*dipoleRot, fDipoleFieldPos), fLogicDipoleMagnet,
                                                "DipoleMagnetPhysical", logicWorld,
                                                false, 0, checkOverlaps);

        //
        // always return the physical World
        //
        return physWorld;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DetectorConstruction::ConstructSDandField()
    {
        // Sensitive detector
        SiDetectorSD *aSiDetectorESD = new SiDetectorSD("TexPPACSim/SiDetectorESD", "SiDetectorEHitsCollection");
        G4SDManager::GetSDMpointer()->AddNewDetector(aSiDetectorESD);
        SetSensitiveDetector("SiDetectorE", aSiDetectorESD, true);

        SiDetectorSD *aSiDetectorDeltaESD = new SiDetectorSD("TexPPACSim/SiDetectorDeltaESD", "SiDetectorDeltaEHitsCollection");
        G4SDManager::GetSDMpointer()->AddNewDetector(aSiDetectorDeltaESD);
        SetSensitiveDetector("SiDetectorDeltaE", aSiDetectorDeltaESD, true);

        // Magnetic field ----------------------------------------------------------
        // First multipole
        fFirstMultipoleField = new FirstMultipoleField(fMultipoleFieldPos, fPhysicFirstMultipoleField->GetObjectRotation());
        fFirstMultipoleField->SetField(fFirstMultipoleProbe);
        fFirstMultipoleFieldMgr = new G4FieldManager();
        fFirstMultipoleFieldMgr->SetDetectorField(fFirstMultipoleField);
        fFirstMultipoleFieldMgr->CreateChordFinder(fFirstMultipoleField);
        G4bool forceToAllDaughters = true;
        fLogicFirstMultipoleField->SetFieldManager(fFirstMultipoleFieldMgr, forceToAllDaughters);
        // Dipole
        fDipoleField = new DipoleField(fDipoleFieldPos);
        fDipoleField->SetField(fDipoleProbe * 1.034);
        fDipoleFieldMgr = new G4FieldManager();
        fDipoleFieldMgr->SetDetectorField(fDipoleField);
        fDipoleFieldMgr->CreateChordFinder(fDipoleField);
        fLogicDipoleField->SetFieldManager(fDipoleFieldMgr, forceToAllDaughters);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DetectorConstruction::ParseParams(std::map<std::string, G4double> params)
    {
        G4cout << "\n---> DetectorConstruction::ParseParams():" << G4endl;
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
            else if (it.first == "UseDeltaE")
            {
                fUseDeltaE = it.second;
                G4cout << std::boolalpha << "Set: Use Delta-E detector = " << it.second << G4endl;
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
            else if (it.first == "FirstMultipoleProbe")
            {
                fFirstMultipoleProbe = it.second * gauss;
                G4cout << "Set: Multipole probe = " << G4BestUnit(fFirstMultipoleProbe, "Magnetic flux density") << G4endl;
            }
            else if (it.first == "DipoleProbe")
            {
                fDipoleProbe = it.second * gauss;
                G4cout << "Set: Dipole probe = " << G4BestUnit(fDipoleProbe, "Magnetic flux density") << G4endl;
            }
        }
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    /*
        void DetectorConstruction::DefineCommands()
        {
            // Define /TexPPACSim/detector command directory using generic messenger class
            fMessenger = new G4GenericMessenger(this,
                                                "/TexPPACSim/detector/",
                                                "Detector control");

            // MdmAngle command
            auto &MdmAngleCmd = fMessenger->DeclareMethodWithUnit("MdmAngle", "deg",
                                                                  &DetectorConstruction::SetMdmAngle,
                                                                  "Set rotation angle of the MDM.");
            MdmAngleCmd.SetParameterName("angle", true);
            MdmAngleCmd.SetRange("angle>=-50. && angle<=150.");
            MdmAngleCmd.SetDefaultValue("0.");
        }
    */

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    /*
        void DetectorConstruction::SetMdmAngle(G4double val)
    {
        if (!fPhysicDipoleField)
        {
            G4cerr << "\n---> DetectorConstruction::SetMdmAngle(): Detector has not yet been constructed." << G4endl;
            return;
        }

        fMdmAngle = val;
        *fMdmRotation = G4RotationMatrix(); // make it unit vector
        fMdmRotation->rotateY(-fMdmAngle);
        fMdmRotation->rotateX(-90. * deg);
        G4ThreeVector dipolePos(-kDipoleFieldRadius, 0., kFirstArmLength);
        dipolePos.rotateY(-fMdmAngle);
        fPhysicDipoleField->SetTranslation(dipolePos);
        fPhysicDipoleMagnet->SetTranslation(dipolePos);
        printf("\n---> DetectorConstruction::SetMdmAngle(): fMdmAngle: %.2f\n", fMdmAngle);

        // tell G4RunManager that we change the geometry
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
    */
}
