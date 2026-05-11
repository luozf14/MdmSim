#include "MdmChargeStateMagneticEqRhs.hh"

#include "G4ChargeState.hh"
#include "G4SystemOfUnits.hh"

namespace MdmSim
{
    MdmChargeStateMagneticEqRhs::MdmChargeStateMagneticEqRhs(G4MagneticField *field,
                                                             G4double chargeState)
        : G4Mag_UsualEqRhs(field), fChargeState(chargeState)
    {
    }

    void MdmChargeStateMagneticEqRhs::SetChargeMomentumMass(G4ChargeState particleCharge,
                                                            G4double momentumXc,
                                                            G4double mass)
    {
        particleCharge.SetCharge(fChargeState * eplus);
        G4Mag_UsualEqRhs::SetChargeMomentumMass(particleCharge, momentumXc, mass);
    }
}
