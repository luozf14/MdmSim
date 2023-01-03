#include "DipoleField.hh"
#include "Constants.hh"

#include "G4RotationMatrix.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include "TF1.h"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::DipoleField(G4double By0, G4double mdmAng, G4ThreeVector dipolePos) : fBy0(By0), fMdmAngle(mdmAng), fDipolePos(dipolePos)
    {
        G4ThreeVector dCO(-kDipoleFieldRadius * std::cos(M_PI - kDipoleDeflectionAngle), 0., kDipoleFieldRadius * std::sin(M_PI - kDipoleDeflectionAngle));
        G4cout << "\ndCO= " << G4BestUnit(dCO, "Length") << G4endl;

        std::unique_ptr<G4RotationMatrix> dipoleRot = std::make_unique<G4RotationMatrix>();
        dipoleRot->rotateY(fMdmAngle);

        G4ThreeVector rotateddCO = dipoleRot->operator()(dCO);
        G4cout << "rotateddCO= " << G4BestUnit(rotateddCO, "Length") << G4endl;

        fCoordinateCPos = fDipolePos + rotateddCO;
        G4cout << "fDipolePos= " << G4BestUnit(fDipolePos, "Length") << G4endl;
        G4cout << "fCoordinateCPos= " << G4BestUnit(fCoordinateCPos, "Length") << G4endl;

        fCoordinateCRot = std::make_unique<G4RotationMatrix>();
        fCoordinateCRot->rotateY(-(kDipoleDeflectionAngle - fMdmAngle));
        fDipolePosInC = fCoordinateCRot->inverse().operator()(fDipolePos - fCoordinateCPos);

        G4cout << "fDipolePosInC= " << G4BestUnit(fDipolePosInC, "Length") << G4endl;

        G4ThreeVector dBO(kDipoleFieldRadius, 0., 0.);
        G4cout << "\ndBO= " << G4BestUnit(dBO, "Length") << G4endl;

        G4ThreeVector rotateddBO = dipoleRot->operator()(dBO);
        G4cout << "rotateddBO= " << G4BestUnit(rotateddBO, "Length") << G4endl;

        fCoordinateBPos = fDipolePos + rotateddBO;
        G4cout << "fDipolePos= " << G4BestUnit(fDipolePos, "Length") << G4endl;
        G4cout << "fCoordinateBPos= " << G4BestUnit(fCoordinateBPos, "Length") << G4endl;

        fCoordinateBRot = std::make_unique<G4RotationMatrix>();
        fCoordinateBRot->rotateY(fMdmAngle);
        fDipolePosInB = fCoordinateBRot->inverse().operator()(fDipolePos - fCoordinateBPos);

        G4cout << "fDipolePosInB= " << G4BestUnit(fDipolePosInB, "Length") << G4endl;

        fEngeFunc = std::make_unique<TF1>("fEngeFunc", "1./(1.+std::exp([0]+[1]*x+[2]*x**2.+[3]*x**3.+[4]*x**4.+[5]*x**5.))", -kFirstMultipoleLength / kFirstMultipoleAperture, kFirstMultipoleLength / kFirstMultipoleAperture);
        fEngeFunc->SetParameters(kFirstMultipoleCoefficients);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DipoleField::~DipoleField()
    {
    }

    void DipoleField::GetFieldValue(const G4double PositionAndTime[4], double *bField) const
    {
        G4ThreeVector pos(PositionAndTime[0], PositionAndTime[1], PositionAndTime[2]);
        G4ThreeVector posInB = fCoordinateBRot->inverse().operator()(pos - fCoordinateBPos);
        if (posInB.z() < -kDipoleZ12)
        {
            G4ThreeVector fieldVec = GetFieldInB(pos);
            bField[0] = fieldVec.x();
            bField[1] = fieldVec.y();
            bField[2] = fieldVec.z();
        }
        else
        {
            G4ThreeVector fieldVec = GetFieldInC(pos);
            bField[0] = fieldVec.x();
            bField[1] = fieldVec.y();
            bField[2] = fieldVec.z();
        }
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4ThreeVector DipoleField::GetFieldInC(G4ThreeVector particlePos) const
    {
        G4ThreeVector particlePosInC = fCoordinateCRot->inverse().operator()(particlePos - fCoordinateCPos);
        G4double B[5][5];
        G4ThreeVector deltaRInC;
        G4double xx, zz, dR;
        for (int i = 0; i < 5; i++) // i in z-axis
        {
            for (int j = 0; j < 5; j++) // j in x-axis
            {
                xx = particlePosInC.x() + (G4double)(j - 2) * kDipoleDG;
                zz = particlePosInC.z() + (G4double)(i - 2) * kDipoleDG;
                G4ThreeVector posInC(xx, 0., zz);
                deltaRInC = posInC - fDipolePosInC;
                dR = (zz < 0) ? (deltaRInC.mag() - kDipoleFieldRadius) : xx;
                B[i][j] = fEngeFunc->Eval(zz / kDipoleFieldHeight) * fBy0 * gauss * (1. - kDipoleNDX * (dR / kDipoleFieldRadius) + kDipoleBET1 * std::pow(dR / kDipoleFieldRadius, 2.) + kDipoleGAMA * std::pow(dR / kDipoleFieldRadius, 3.) + kDipoleDELT * std::pow(dR / kDipoleFieldRadius, 4.));
            }
        }

        G4double y = particlePosInC.y();
        G4double By2nd = -std::pow(y / kDipoleDG, 2.) * (2. / 3. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) - 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]));
        G4double By4th = std::pow(y / kDipoleDG, 4.) * (-1. / 6. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) + 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]) + 1. / 12. * (B[3][3] + B[1][3] + B[3][1] + B[1][1] - 2. * B[3][2] - 2. * B[1][2] - 2. * B[2][3] - 2. * B[2][1] + 4. * B[2][2]));

        G4double Bx = y / kDipoleDG * (2. / 3. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0]) - 1. / 12. * (B[3][3] + B[1][3] - B[3][1] - B[1][1] - 2. * B[2][3] + 2. * B[2][1]));
        G4double By = B[2][2] + By2nd + By4th;
        G4double Bz = y / kDipoleDG * (2. / 3. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2]) - 1. / 12. * (B[3][3] + B[3][1] - B[1][3] - B[1][1] - 2. * B[3][2] + 2. * B[1][2]));

        return G4ThreeVector(Bx, By, Bz);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4ThreeVector DipoleField::GetFieldInB(G4ThreeVector particlePos) const
    {
        G4ThreeVector particlePosInB = fCoordinateBRot->inverse().operator()(particlePos - fCoordinateBPos);
        G4double B[5][5];
        G4ThreeVector deltaRInB;
        G4double xx, zz, dR;
        for (int i = 0; i < 5; i++) // i in z-axis
        {
            for (int j = 0; j < 5; j++) // j in x-axis
            {
                xx = particlePosInB.x() + (G4double)(j - 2) * kDipoleDG;
                zz = particlePosInB.z() + (G4double)(i - 2) * kDipoleDG;
                G4ThreeVector posInB(xx, 0., zz);
                deltaRInB = posInB - fDipolePosInB;
                dR = (zz < 0) ? xx : (deltaRInB.mag() - kDipoleFieldRadius);
                B[i][j] = fEngeFunc->Eval(-zz / kDipoleFieldHeight) * fBy0 * gauss * (1. - kDipoleNDX * (dR / kDipoleFieldRadius) + kDipoleBET1 * std::pow(dR / kDipoleFieldRadius, 2.) + kDipoleGAMA * std::pow(dR / kDipoleFieldRadius, 3.) + kDipoleDELT * std::pow(dR / kDipoleFieldRadius, 4.));
            }
        }

        G4double y = particlePosInB.y();
        G4double By2nd = -std::pow(y / kDipoleDG, 2.) * (2. / 3. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) - 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]));
        G4double By4th = std::pow(y / kDipoleDG, 4.) * (-1. / 6. * (B[3][2] - B[1][2] + B[2][3] + B[2][1] - 4. * B[2][2]) + 1. / 24. * (B[4][2] - B[0][2] + B[2][4] + B[2][0] - 4. * B[2][2]) + 1. / 12. * (B[3][3] + B[1][3] + B[3][1] + B[1][1] - 2. * B[3][2] - 2. * B[1][2] - 2. * B[2][3] - 2. * B[2][1] + 4. * B[2][2]));

        G4double Bx = y / kDipoleDG * (2. / 3. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[2][3] - B[2][1]) - 1. / 12. * (B[2][4] - B[2][0]) - 1. / 12. * (B[3][3] + B[1][3] - B[3][1] - B[1][1] - 2. * B[2][3] + 2. * B[2][1]));
        G4double By = B[2][2] + By2nd + By4th;
        G4double Bz = y / kDipoleDG * (2. / 3. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2])) + std::pow(y / kDipoleDG, 3.) * (1. / 6. * (B[3][2] - B[1][2]) - 1. / 12. * (B[4][2] - B[0][2]) - 1. / 12. * (B[3][3] + B[3][1] - B[1][3] - B[1][1] - 2. * B[3][2] + 2. * B[1][2]));

        return G4ThreeVector(Bx, By, Bz);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
