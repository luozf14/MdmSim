#include "DipoleField.hh"
#include "Constants.hh"

#include "G4RotationMatrix.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::DipoleField(G4double By0, G4ThreeVector dipolePos, G4double mdmAng) : fBy0(By0), fDipolePos(dipolePos), fMdmAngle(mdmAng)
    {
        G4ThreeVector dCO(-kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
        G4cout << "\ndCO= " << G4BestUnit(dCO, "Length") << G4endl;

        std::unique_ptr<G4RotationMatrix> dipoleRot = std::make_unique<G4RotationMatrix>();
        dipoleRot->rotateY(fMdmAngle);

        G4ThreeVector rotateddCO = dipoleRot->operator()(dCO);
        G4cout << "\nrotateddCO= " << G4BestUnit(rotateddCO, "Length") << G4endl;

        fCoordinateCPos = fDipolePos + rotateddCO;
        G4cout << "\nfDipolePos= " << G4BestUnit(fDipolePos, "Length") << G4endl;
        G4cout << "\nfCoordinateCPos= " << G4BestUnit(fCoordinateCPos, "Length") << G4endl;

        fCoordinateCRot = std::make_unique<G4RotationMatrix>();
        fCoordinateCRot->rotateY((kDipoleDeflectionAngle - fMdmAngle));
        fDipolePosInC = fCoordinateCRot->operator()(fDipolePos - fCoordinateCPos);

        G4cout << "\nfDipolePosInC= " << G4BestUnit(fDipolePosInC, "Length") << G4endl;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::~DipoleField()
    {
    }

    void DipoleField::GetFieldValue(const G4double PositionAndTime[4], double *bField) const
    {

        G4ThreeVector particlePos(PositionAndTime[0], PositionAndTime[1], PositionAndTime[2]);
        G4ThreeVector particlePosInC = fCoordinateCRot->operator()(particlePos - fCoordinateCPos);
        G4double y = particlePos.y();
        G4ThreeVector deltaRInC = particlePosInC - fDipolePosInC;
        G4double dR = std::sqrt(deltaRInC.x() * deltaRInC.x() + deltaRInC.z() * deltaRInC.z()) - kDipoleFieldRadius;
        // G4double By0 = fBy0 * gauss * (1. - kDipoleNDX * (dR / kDipoleFieldRadius) + kDipoleBET1 * std::pow(dR / kDipoleFieldRadius, 2.) + kDipoleGAMA * std::pow(dR / kDipoleFieldRadius, 3.) + kDipoleDELT * std::pow(dR / kDipoleFieldRadius, 4.));

        G4double B[5][5] = {0.};
        // G4ThreeVector deltaRInC;
        G4double xx, zz;
        for (int i = 0; i < 5; i++) // i in z-axis
        {
            for (int j = 0; j < 5; j++) // j in x-axis
            {
                xx = particlePos.x() + (G4double)(j - 2) * kDipoleDG;
                zz = particlePos.z() + (G4double)(i - 2) * kDipoleDG;
                G4ThreeVector pos(xx, 0., zz);
                G4ThreeVector deltaR = pos - fDipolePos;
                dR = std::sqrt(deltaR.x() * deltaR.x() + deltaR.z() * deltaR.z()) - kDipoleFieldRadius;
                B[i][j] = fBy0 * gauss * (1. - kDipoleNDX * (dR / kDipoleFieldRadius) + kDipoleBET1 * std::pow(dR / kDipoleFieldRadius, 2.) + kDipoleGAMA * std::pow(dR / kDipoleFieldRadius, 3.) + kDipoleDELT * std::pow(dR / kDipoleFieldRadius, 4.));
            }
        }

        G4double By2nd = -std::pow(y / kDipoleDG, 2.) * (2. / 3. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) - 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]));
        G4double By4th = std::pow(y / kDipoleDG, 4.) * (-1. / 6. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) + 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]) + 1. / 12. * (B[3][3] + B[1][3] + B[3][1] + B[1][1] - 2. * B[3][2] - 2. * B[1][2] - 2. * B[2][3] - 2. * B[2][1] + 4. * B[2][2]));

        bField[0] = y / kDipoleDG * (2. / 3. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0]) - 1. / 12. * (B[3][3] + B[1][3] - B[3][1] - B[1][1] - 2. * B[2][3] + 2. * B[2][1]));
        bField[1] = B[2][2] + By2nd + By4th;
        bField[2] = y / kDipoleDG * (2. / 3. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2]) - 1. / 12. * (B[3][3] + B[3][1] - B[1][3] - B[1][1] - 2. * B[3][2] + 2. * B[1][2]));
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    /*
        void DipoleField::DefineCommands()
        {
            // Define /TexPPACSim/field command directory using generic messenger class
            fMessenger = new G4GenericMessenger(this,
                                                "/TexPPACSim/field/",
                                                "Field control");

            // fieldValue command
            auto &valueCmd = fMessenger->DeclareMethodWithUnit("value", "tesla",
                                                               &DipoleField::SetField,
                                                               "Set field strength.");
            valueCmd.SetParameterName("field", true);
            valueCmd.SetDefaultValue(".375");
        }
    */
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
