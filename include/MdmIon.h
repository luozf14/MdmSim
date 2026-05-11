#pragma once

struct MdmIon {
  int massNumber = 0;
  int atomicNumber = 0;
  int chargeState = 0;
  double neutralMassU = 0.0;
  double ionMassMeV = 0.0;

  double RaytraceMassAmu() const { return ionMassMeV / 931.48; }
};
