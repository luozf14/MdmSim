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

    constexpr G4double kFirstArmLength = 2420. * mm;

    constexpr G4double kJeffParameters[6] = {-0.51927, 0.038638, 0.028404, -0.022797, -0.019275, 0.755583};

    constexpr G4double kFirstMultipolePos = (63.5 + 18.075) * cm;
    constexpr G4double kFirstMultipoleAperture = 6.5 * cm;
    constexpr G4double kFirstMultipoleLength = 26. * cm;

    constexpr G4double kDipoleFieldRadius = 1600. * mm;
    constexpr G4double kDipoleFieldWidth = 60. * cm;
    constexpr G4double kDipoleFieldHeight = 6. * cm;
    constexpr G4double kDipoleNDX = 0.191;
    constexpr G4double kDipoleBET1 =  -0.04;
    constexpr G4double kDipoleGAMA =  0.;
    constexpr G4double kDipoleDELT =  0.;
    constexpr G4double kDipoleMagnetRadius = 1600. * 9.2 / 5.5 * mm;
    constexpr G4double kDipoleDeflectionAngle = 100. * deg;

    constexpr G4double kSecondArmLength = 1295. * mm;

}

#endif
