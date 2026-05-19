#include "MdmChargeStateMagneticEqRhs.hh"

#include "G4ChargeState.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>
#include <utility>

namespace MdmSim
{
    MdmChargeStateMagneticEqRhs::MdmChargeStateMagneticEqRhs(G4MagneticField *field,
                                                             G4double chargeState,
                                                             G4bool useFixedChargeState)
        : G4Mag_UsualEqRhs(field),
          fChargeState(chargeState),
          fUseFixedChargeState(useFixedChargeState)
    {
    }

    MdmChargeStateMagneticEqRhs::MdmChargeStateMagneticEqRhs(
        G4MagneticField *field,
        G4double chargeState,
        std::vector<MdmChargeOverride> chargeOverrides)
        : G4Mag_UsualEqRhs(field),
          fChargeState(chargeState),
          fUseFixedChargeState(false),
          fChargeOverrides(std::move(chargeOverrides))
    {
    }

    void MdmChargeStateMagneticEqRhs::SetChargeMomentumMass(G4ChargeState particleCharge,
                                                            G4double momentumXc,
                                                            G4double mass)
    {
        if (fUseFixedChargeState)
        {
            particleCharge.SetCharge(fChargeState * eplus);
        }
        else
        {
            for (const auto &chargeOverride : fChargeOverrides)
            {
                if (std::abs(mass - chargeOverride.mass) < 1e-3 * std::max(1., chargeOverride.mass))
                {
                    particleCharge.SetCharge(chargeOverride.chargeState * eplus);
                    break;
                }
            }
        }
        G4Mag_UsualEqRhs::SetChargeMomentumMass(particleCharge, momentumXc, mass);
    }
}
