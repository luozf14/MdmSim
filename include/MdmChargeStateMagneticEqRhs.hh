#ifndef MdmSimMdmChargeStateMagneticEqRhs_h
#define MdmSimMdmChargeStateMagneticEqRhs_h 1

#include "G4Mag_UsualEqRhs.hh"
#include "globals.hh"

class G4MagneticField;
class G4ChargeState;

namespace MdmSim
{
    // Geant4 may replace an ion's dynamic charge with an effective charge for
    // EM loss models. MDM magnetic rigidity must use the configured charge
    // state instead, matching RAYTRACE and the field-map generation.
    class MdmChargeStateMagneticEqRhs : public G4Mag_UsualEqRhs
    {
    public:
        MdmChargeStateMagneticEqRhs(G4MagneticField *field, G4double chargeState);

        void SetChargeMomentumMass(G4ChargeState particleCharge,
                                   G4double momentumXc,
                                   G4double mass) override;

    private:
        G4double fChargeState;
    };
}

#endif
