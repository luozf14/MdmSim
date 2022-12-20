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

constexpr G4double kFirstArmLength = 2420.*mm;
constexpr G4double kDipoleRadius = 1600.*mm;
constexpr G4double kDipoleDeflectionAngle = 100.*deg;
constexpr G4double kDipoleAlpha = -0.191;
constexpr G4double kSecondArmLength = 1295.*mm;


}

#endif
