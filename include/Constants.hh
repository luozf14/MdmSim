//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
#ifndef MdmSimConstants_h
#define MdmSimConstants_h 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

namespace MdmSim
{
    constexpr G4double kFirstArmLength = 2420. * mm;

    constexpr G4double kSlitBoxPos = 63.5 * cm;
    constexpr G4double kSlitBoxDz = 1. * mm;

    constexpr G4double kFirstMultipoleEntrancePos = kSlitBoxPos + 0.5 * kSlitBoxDz;
    constexpr G4double kFirstMultipoleAperture = 6.5 * cm;
    constexpr G4double kFirstMultipoleLength = (20. + 10. + 6. + 10. + 20.) * cm;

    constexpr G4double kDipoleFieldRadius = 1600. * mm;
    constexpr G4double kDipoleFieldWidth = 60. * cm;
    constexpr G4double kDipoleFieldHeight = 11.5 * cm;
    constexpr G4double kDipoleZ11 = 46. * cm;
    constexpr G4double kDipoleZ12 = -33. * cm;
    constexpr G4double kDipoleZ21 = -23. * cm;
    constexpr G4double kDipoleZ22 = 50. * cm;
    constexpr G4double kDipoleMagnetRadius = 1600. * 9.2 / 5.5 * mm;
    constexpr G4double kDipoleDeflectionAngle = 100. * deg;

    constexpr G4double kSecondArmLength = (32.55 + 0.2 + 35. + 1.5 + 96.13 - 1.5) * cm; // CD(dipole) + [A + L + B](MULT) + 96.13-B

    constexpr G4double kPpacWidth = 40. * cm;
    constexpr G4double kPpacHeight = 10. * cm;
    constexpr G4double kPpacChamberLength = 100. * cm;
    constexpr G4double kPpacSpacingWindowCathode = 1. * cm;
    constexpr G4double kPpacEntranceWindowThickness = 2.5 * um;
    constexpr G4double kPpacCathodeMylarThickness = 220. * 1e-6 * g / cm2;
    constexpr G4double kPpacCathodeAlThickness = 80. * 1e-6 * g / cm2;

}

#endif
