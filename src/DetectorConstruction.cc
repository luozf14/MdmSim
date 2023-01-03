#include "DetectorConstruction.hh"
#include "SiDetectorSD.hh"
#include "DipoleField.hh"
#include "FirstMultipoleField.hh"
#include "Constants.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ChordFinder.hh"
#include "G4Field.hh"
#include "G4AutoDelete.hh"

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
#include "G4RotationMatrix.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"

namespace TexPPACSim
{
    G4ThreadLocal DipoleField *DetectorConstruction::fDipoleField = nullptr;
    G4ThreadLocal G4FieldManager *DetectorConstruction::fDipoleFieldMgr = nullptr;
    G4ThreadLocal FirstMultipoleField *DetectorConstruction::fFirstMultipoleField = nullptr;
    G4ThreadLocal G4FieldManager *DetectorConstruction::fFirstMultipoleFieldMgr = nullptr;
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorConstruction::DetectorConstruction()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorConstruction::~DetectorConstruction()
    {
        delete fDipoleField;
        delete fDipoleFieldMgr;
        delete fFirstMultipoleField;
        delete fFirstMultipoleFieldMgr;
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
        G4ThreeVector slitBoxPos = G4ThreeVector(kSlitBoxPos, 0., 0.); // position in mother frame
        slitBoxPos.setTheta(-1. * fMdmAngle);
        slitBoxPos.setPhi(0. * deg);
        G4RotationMatrix *slitBoxRot = new G4RotationMatrix;
        slitBoxRot->rotateY(-1. * fMdmAngle);

        G4VSolid *solidSlitBoxVoid = new G4Box("SlitBoxVoid", 2.27965 * cm, 2.27965 * cm, 0.5 * kSlitBoxDz); // measured on 1/26/2022
        G4VSolid *solidSlitBoxShape = new G4Box("SlitBoxShape", (2.27965 + 5.) * cm, (2.27965 + 5.) * cm, 0.5 * kSlitBoxDz);
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
        // First multipole field
        //
        G4VSolid *solidFirstMultipoleField = new G4Tubs("FirstMultipoleFieldTubs", 0., kFirstMultipoleAperture, 0.5 * kFirstMultipoleLength, 0., 360. * deg);
        fLogicFirstMultipoleField = new G4LogicalVolume(solidFirstMultipoleField, nist->FindOrBuildMaterial("G4_Galactic"), "FirstMultipoleFieldLogical");
        fMultipoleFieldPos.setX(kFirstMultipoleEntrancePos + 0.5 * kFirstMultipoleLength);
        fMultipoleFieldPos.setY(0.);
        fMultipoleFieldPos.setZ(0.);
        fMultipoleFieldPos.setTheta(fMdmAngle);
        fMultipoleFieldPos.setPhi(0. * deg);
        G4RotationMatrix *firstMultipoleFieldRot = new G4RotationMatrix;
        firstMultipoleFieldRot->rotateY(fMdmAngle);
        fPhysicFirstMultipoleField = new G4PVPlacement(G4Transform3D(*firstMultipoleFieldRot, fMultipoleFieldPos), fLogicFirstMultipoleField,
                                                       "FirstMultipoleFieldPhysical", logicWorld,
                                                       false, 0, checkOverlaps);
        fLogicFirstMultipoleField->SetUserLimits(new G4UserLimits(1. * cm));

        //
        // First multipole magnet
        //
        G4VSolid *solidFirstMultipoleMagnetShape = new G4Tubs("FirstMultipoleMagnetShape", 0., kFirstMultipoleAperture + 0.1 * cm, 0.5 * kFirstMultipoleLength, 0., 360. * deg);
        auto solidFirstMultipoleMagnet = new G4SubtractionSolid("FirstMultipoleMagnetSolid", solidFirstMultipoleMagnetShape, solidFirstMultipoleField);
        auto logicFirstMultipoleMagnet = new G4LogicalVolume(solidFirstMultipoleMagnet, nist->FindOrBuildMaterial("G4_Al"), "FirstMultipoleMagnetLogical");
        new G4PVPlacement(G4Transform3D(*firstMultipoleFieldRot, fMultipoleFieldPos), logicFirstMultipoleMagnet,
                          "FirstMultipoleMagnetPhysical", logicWorld,
                          false, 0, checkOverlaps);

        //
        // Dipole field
        //
        G4VSolid *solidDipoleFieldEntrace = new G4Box("DipoleFieldEntrance", 0.5 * kDipoleFieldWidth, 0.5 * kDipoleZ11, 0.5 * kDipoleFieldHeight);
        G4ThreeVector solidDipoleFieldEntraceTrans(kDipoleFieldRadius, -0.5 * kDipoleZ11, 0.);
        G4VSolid *solidDipoleFieldExit = new G4Box("DipoleFieldEntrance", 0.5 * kDipoleFieldWidth, 0.5 * kDipoleZ22, 0.5 * kDipoleFieldHeight);
        G4RotationMatrix *solidDipoleFieldExitRot = new G4RotationMatrix;
        solidDipoleFieldExitRot->rotateZ(kDipoleDeflectionAngle);
        G4ThreeVector solidDipoleFieldExitTrans1(-kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle), 0.);
        G4ThreeVector solidDipoleFieldExitTrans2(-0.5 * kDipoleZ22 * std::cos(kDipoleDeflectionAngle - M_PI / 2.), -0.5 * kDipoleZ22 * std::sin(kDipoleDeflectionAngle - M_PI / 2.), 0.);
        G4ThreeVector solidDipoleFieldExitTrans = solidDipoleFieldExitTrans1 + solidDipoleFieldExitTrans2;
        G4VSolid *solidDipoleFieldUniform = new G4Tubs("DipoleFieldUniform", kDipoleFieldRadius - 0.5 * kDipoleFieldWidth, kDipoleFieldRadius + 0.5 * kDipoleFieldWidth, 0.5 * kDipoleFieldHeight, 0., kDipoleDeflectionAngle);
        G4UnionSolid *solidDipoleFieldTemp = new G4UnionSolid("solidDipoleFieldTemp", solidDipoleFieldUniform, solidDipoleFieldEntrace, nullptr, solidDipoleFieldEntraceTrans);
        G4UnionSolid *solidDipoleField = new G4UnionSolid("solidDipoleField", solidDipoleFieldTemp, solidDipoleFieldExit, G4Transform3D(*solidDipoleFieldExitRot, solidDipoleFieldExitTrans));
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
        auto solidDipoleMagnetShape = new G4Tubs("DipoleMagnetShape", 30. * cm, kDipoleMagnetRadius, 0.5 * m, 0., kDipoleDeflectionAngle);
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
        fFirstMultipoleField = new FirstMultipoleField(fFirstMultipoleProbe, fMdmAngle, fMultipoleFieldPos);
        fFirstMultipoleFieldMgr = new G4FieldManager();
        fFirstMultipoleFieldMgr->SetDetectorField(fFirstMultipoleField);
        fFirstMultipoleFieldMgr->CreateChordFinder(fFirstMultipoleField);
        // fFirstMultipoleFieldMgr->GetChordFinder()->SetDeltaChord(1. * mm);
        // fFirstMultipoleFieldMgr->SetAccuraciesWithDeltaOneStep(1. * mm);
        G4bool forceToAllDaughters = true;
        fLogicFirstMultipoleField->SetFieldManager(fFirstMultipoleFieldMgr, forceToAllDaughters);
        // Dipole
        fDipoleField = new DipoleField(fDipoleProbe * 1.034, fMdmAngle, fDipoleFieldPos);
        fDipoleFieldMgr = new G4FieldManager();
        fDipoleFieldMgr->SetDetectorField(fDipoleField);
        fDipoleFieldMgr->CreateChordFinder(fDipoleField);
        // fDipoleFieldMgr->GetChordFinder()->SetDeltaChord(1. * mm);
        // fDipoleFieldMgr->SetAccuraciesWithDeltaOneStep(1. * mm);
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
                fFirstMultipoleProbe = it.second;
                G4cout << "Set: Multipole probe = " << fFirstMultipoleProbe << " Gauss" << G4endl;
            }
            else if (it.first == "DipoleProbe")
            {
                fDipoleProbe = it.second;
                G4cout << "Set: Dipole probe = " << fDipoleProbe << " Gauss" << G4endl;
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
