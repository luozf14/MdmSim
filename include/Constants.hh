//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
#ifndef TexPPACSimConstants_h
#define TexPPACSimConstants_h 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

namespace TexPPACSim
{
    constexpr G4double kJeffParameters[6] = {-0.51927, 0.038638, 0.028404, -0.022797, -0.019275, 0.755583};

    constexpr G4double kFirstArmLength = 2420. * mm;

    constexpr G4double kSlitBoxPos = 63.5 * cm;
    constexpr G4double kSlitBoxDz = 1. * mm;

    constexpr G4double kFirstMultipoleEntrancePos = kSlitBoxPos + 0.5 * kSlitBoxDz;
    constexpr G4double kFirstMultipoleAperture = 6.5 * cm;
    constexpr G4double kFirstMultipoleLength = (20. + 10. + 6. + 10. + 20.) * cm;
    constexpr G4double kFirstMultipoleCoefficients[6] = {0.1122, 6.2671, -1.4982, 3.5882, -2.1209, 1.723};

    constexpr G4double kDipoleFieldRadius = 1600. * mm;
    constexpr G4double kDipoleFieldWidth = 60. * cm;
    constexpr G4double kDipoleFieldHeight = 11.5 * cm;
    constexpr G4double kDipoleDG = 0.3 * kDipoleFieldHeight;
    constexpr G4double kDipoleNDX = 0.191;
    constexpr G4double kDipoleBET1 = -0.04;
    constexpr G4double kDipoleGAMA = 0.;
    constexpr G4double kDipoleDELT = 0.;
    constexpr G4double kDipoleZ11 = 46. * cm;
    constexpr G4double kDipoleZ12 = -33. * cm;
    constexpr G4double kDipoleZ21 = -23. * cm;
    constexpr G4double kDipoleZ22 = 50. * cm;
    constexpr G4double kDipoleJeffsFrngs[6] = {.048, 3.70, .0125, -.299, .016, .020};
    constexpr G4double kDipoleDrawingEntrance[7] = {1.242, -3.11, 4.142, -1.06, 0., 0., 0.};
    constexpr G4double kDipoleDrawingExit[7] = {-1.579, 1.719, -13.43, -24.58, 779.35, 821.26, -21410.};
    constexpr G4double kDipoleMagnetRadius = 1600. * 9.2 / 5.5 * mm;
    constexpr G4double kDipoleDeflectionAngle = 100. * deg;

    constexpr G4double kSecondArmLength = 1295. * mm;
}

#endif
