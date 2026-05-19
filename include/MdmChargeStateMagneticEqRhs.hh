#ifndef MdmSimMdmChargeStateMagneticEqRhs_h
#define MdmSimMdmChargeStateMagneticEqRhs_h 1

#include "G4Mag_UsualEqRhs.hh"
#include "globals.hh"

#include <vector>

class G4MagneticField;
class G4ChargeState;

namespace MdmSim
{
    struct MdmChargeOverride
    {
        G4double mass = 0.;
        G4double chargeState = 0.;
    };

    // Geant4 may replace an ion's dynamic charge with an effective charge for
    // EM loss models. MDM magnetic rigidity must use the configured charge
    // state instead, matching RAYTRACE and the field-map generation.
    class MdmChargeStateMagneticEqRhs : public G4Mag_UsualEqRhs
    {
    public:
        MdmChargeStateMagneticEqRhs(G4MagneticField *field,
                                    G4double chargeState,
                                    G4bool useFixedChargeState = true);
        MdmChargeStateMagneticEqRhs(G4MagneticField *field,
                                    G4double chargeState,
                                    std::vector<MdmChargeOverride> chargeOverrides);

        void SetChargeMomentumMass(G4ChargeState particleCharge,
                                   G4double momentumXc,
                                   G4double mass) override;

    private:
        G4double fChargeState;
        G4bool fUseFixedChargeState;
        std::vector<MdmChargeOverride> fChargeOverrides;
    };
}

#endif
