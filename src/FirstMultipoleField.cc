#include "FirstMultipoleField.hh"
#include "Constants.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "globals.hh"

#include "TF1.h"

namespace MdmSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    FirstMultipoleField::FirstMultipoleField(G4double pole, G4double mdmAngle, G4ThreeVector pos) : fMultipoleProbe(pole), fMdmAngle(mdmAngle), fFirstMultipolePos(pos)
    {
        fFirstMultipoleRot = std::make_unique<G4RotationMatrix>();
        fFirstMultipoleRot->rotateY(fMdmAngle);
        fBQR = -1. * fMultipoleProbe * 1e-4 * kJeffParameters[5];
        fBHR = fBQR * kJeffParameters[1] / kJeffParameters[0];
        fBOR = fBQR * kJeffParameters[2] / kJeffParameters[0];
        fBDR = fBQR * kJeffParameters[3] / kJeffParameters[0];
        fBDDR = fBQR * kJeffParameters[4] / kJeffParameters[0];
        fG1 = fBQR / std::pow(kFirstMultipoleAperture * 0.1, 1.);
        fG2 = fBHR / std::pow(kFirstMultipoleAperture * 0.1, 2.);
        fG3 = fBOR / std::pow(kFirstMultipoleAperture * 0.1, 3.);
        fG4 = fBDR / std::pow(kFirstMultipoleAperture * 0.1, 4.);
        fG5 = fBDDR / std::pow(kFirstMultipoleAperture * 0.1, 5.);
        fEngeFunc = std::make_unique<TF1>("fEngeFunc", "1./(1.+std::exp([0]+[1]*x+[2]*x**2.+[3]*x**3.+[4]*x**4.+[5]*x**5.))", -100., 100.);
        fEngeFunc->SetParameters(kFirstMultipoleCoefficients);
        G4cout << "\n---> FirstMultipoleField::FirstMultipoleField(): fFirstMultipolePos= " << G4BestUnit(fFirstMultipolePos, "Length") << G4endl;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    FirstMultipoleField::~FirstMultipoleField()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void FirstMultipoleField::GetFieldValue(const G4double PositionAndTime[4], G4double *bField) const
    {
        G4ThreeVector particlePos(PositionAndTime[0], PositionAndTime[1], PositionAndTime[2]);
        // G4cout << "\n---> FirstMultipoleField::GetFieldValue: particlePos= " << G4BestUnit(particlePos, "Length") << G4endl;
        G4ThreeVector translatedR = particlePos - fFirstMultipolePos;
        G4ThreeVector rotatedR = fFirstMultipoleRot->inverse().operator()(translatedR);
        G4double z = rotatedR.z();
        // G4cout << "\n---> FirstMultipoleField::GetFieldValue: rotatedR= " << G4BestUnit(rotatedR, "Length") << G4endl;
        // G4cout << "\n---> FirstMultipoleField::GetFieldValue: rotatedR.z()= " << z << G4endl;
        G4ThreeVector posInPole;

        // quadrupole
        if (z < -3. * cm) // entrance fringing field
        {
            posInPole[0] = rotatedR.x();
            posInPole[1] = rotatedR.y();
            posInPole[2] = -130. - rotatedR.z();
        }
        else // uniform zone and exit fringing field
        {
            posInPole[0] = rotatedR.x();
            posInPole[1] = rotatedR.y();
            posInPole[2] = rotatedR.z() - 130.;
        }
        G4ThreeVector quadrupoleField = GetQuadrupoleField(posInPole);

        // high order field entrance
        posInPole[2] = -65. - rotatedR.z();
        G4ThreeVector highOrderFieldEntrance = GetHighOrderField(posInPole);

        // high order field exit
        posInPole[2] = rotatedR.z() - 65.;
        G4ThreeVector highOrderFieldExit = GetHighOrderField(posInPole);

        // high order field offset
        posInPole[2] = -65.;
        G4ThreeVector highOrderFieldOffSet = GetHighOrderField(posInPole);

        G4ThreeVector highOrderField = 0.5 * (highOrderFieldEntrance + highOrderFieldExit - highOrderFieldOffSet);

        G4ThreeVector field = fFirstMultipoleRot->operator()(quadrupoleField + highOrderField);
        // G4ThreeVector field = highOrderField;
        // G4ThreeVector field = quadrupoleField;
        bField[0] = field.x();
        bField[1] = field.y();
        bField[2] = field.z();
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4ThreeVector FirstMultipoleField::GetQuadrupoleField(G4ThreeVector pos) const
    {
        G4double x = pos.x() * 0.1; // convert from mm to cm
        G4double y = pos.y() * 0.1;
        G4double z = pos.z() * 0.1;
        G4ThreeVector bField;
        // bField[0] = fG1 * y + fG2 * (2. * x * y) + fG3 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) + fG4 * 4. * (std::pow(x, 3.) * y - x * std::pow(y, 3.)) + fG5 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
        // bField[1] = fG1 * x + fG2 * (std::pow(x, 2.) - std::pow(y, 2.)) + fG3 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) + fG4 * (std::pow(x, 4.) - 6. * std::pow(x * y, 2.) + std::pow(y, 4.)) + fG5 * (std::pow(x, 5.) - 10. * std::pow(x, 3.) * std::pow(y, 2.) + 5. * x * std::pow(y, 4.));
        // bField[2] = 0.;
        // printf("x = %.4e cm\n", x);
        // printf("y = %.4e cm\n", y);
        // printf("r = %.4e cm\n", std::sqrt(x*x+y*y));
        G4double ss[1] = {z / (kFirstMultipoleAperture * 0.1)};
        G4double *par = 0;
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): pos.z()=%.4e\n", pos.z());
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): ss[0]=%.4e\n", ss[0]);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): fEngeFunc(ss[0])=%.4e\n", fEngeFunc->Eval(ss[0]));

        // Gnm = G_{n}^{(m)}
        // Quadrupole
        G4double G10 = fG1 * S0(ss, par);
        G4double G11 = fG1 * S1(ss, par);
        G4double G12 = fG1 * S2(ss, par);
        G4double G13 = fG1 * S3(ss, par);
        G4double G14 = fG1 * S4(ss, par);
        G4double G15 = fG1 * S5(ss, par);
        G4double G16 = fG1 * S6(ss, par);
        G4double B1x, B1y, B1z;
        if (z < -10.) // uniform zone
        {
            B1x = G10 * y;
            B1y = G10 * x;
            B1z = 0.;
        }
        else
        {
            B1x = G10 * y - 1. / 12. * G12 * (3. * std::pow(x, 2.) * y + std::pow(y, 3.)) /*+ 1. / 384. * G14 * (5. * std::pow(x, 4.) * y + 6. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.)) - 1. / 23040. * G16 * (7. * std::pow(x, 6.) * y + 15. * std::pow(x, 4.) * std::pow(y, 3.) + 9. * std::pow(x, 2.) * std::pow(y, 5.) + std::pow(y, 7.))*/;
            B1y = G10 * x - 1. / 12. * G12 * (3. * std::pow(y, 2.) * x + std::pow(x, 3.)) /*+ 1. / 384. * G14 * (5. * std::pow(y, 4.) * x + 6. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.)) - 1. / 23040. * G16 * (7. * std::pow(y, 6.) * x + 15. * std::pow(y, 4.) * std::pow(x, 3.) + 9. * std::pow(y, 2.) * std::pow(x, 5.) + std::pow(x, 7.))*/;
            B1z = G11 * x * y - 1. / 12. * G13 * (std::pow(x, 3.) * y + x * std::pow(y, 3.)) + 1. / 384. * G15 * (std::pow(x, 5.) * y + 2. * std::pow(x, 3.) * std::pow(y, 3.) + x * std::pow(y, 5.));
        }
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): G10=%.4e\n", G10);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): |B1x_B1y|=%.4e\n", std::sqrt(B1x*B1x+B1y*B1y));
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): G12=%.4e\n", G12/fG1);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): G13=%.4e\n", G13/fG1);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): G14=%.4e\n", G14/fG1);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): G15=%.4e\n", G15/fG1);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): G16=%.4e\n", G16/fG1);

        bField[0] = B1x;
        bField[1] = B1y;
        bField[2] = B1z;

        bField *= 1. * tesla;
        return bField;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    G4ThreeVector FirstMultipoleField::GetHighOrderField(G4ThreeVector pos) const
    {
        G4double x = pos.x() * 0.1; // convert from mm to cm
        G4double y = pos.y() * 0.1;
        G4double z = pos.z() * 0.1;
        G4ThreeVector bField;
        // bField[0] = fG1 * y + fG2 * (2. * x * y) + fG3 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) + fG4 * 4. * (std::pow(x, 3.) * y - x * std::pow(y, 3.)) + fG5 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
        // bField[1] = fG1 * x + fG2 * (std::pow(x, 2.) - std::pow(y, 2.)) + fG3 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) + fG4 * (std::pow(x, 4.) - 6. * std::pow(x * y, 2.) + std::pow(y, 4.)) + fG5 * (std::pow(x, 5.) - 10. * std::pow(x, 3.) * std::pow(y, 2.) + 5. * x * std::pow(y, 4.));
        // bField[2] = 0.;
        // printf("x = %.4e cm\n", x);
        // printf("y = %.4e cm\n", y);
        // printf("r = %.4e cm\n", std::sqrt(x*x+y*y));
        G4double ss[1] = {z / (kFirstMultipoleAperture * 0.1)};
        G4double *par = 0;
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): pos.z()=%.4e\n", pos.z());
        // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): ss[0]=%.4e\n", ss[0]);
        // printf("---> FirstMultipoleField::GetEntranceFringingField(): fEngeFunc(ss[0])=%.4e\n", fEngeFunc->Eval(ss[0]));

        // Hexapole
        G4double G20 = fG2 * S0(ss, par);
        G4double G21 = fG2 * S1(ss, par);
        G4double G22 = fG2 * S2(ss, par);
        G4double G23 = fG2 * S3(ss, par);
        // G4double G24 = fG2 * S4(ss, par);
        // G4double G25 = fG2 * S5(ss, par);
        G4double B2x = G20 * 2. * x * y - 1. / 48. * G22 * (12. * std::pow(x, 3.) * y + 4. * x * std::pow(y, 3.));
        G4double B2y = G20 * (std::pow(x, 2.) - std::pow(y, 2.)) - 1. / 48. * G22 * (3. * std::pow(x, 4.) + 6. * std::pow(x, 2.) * std::pow(y, 2.) - 5. * std::pow(y, 4.));
        G4double B2z = G21 * (std::pow(x, 2.) * y - std::pow(y, 3.) / 3.) - 1. / 48. * G23 * (3. * std::pow(x, 4.) * y + 2. * std::pow(x, 2.) * std::pow(y, 3.) - std::pow(y, 5.));

        // Octupole
        G4double G30 = fG3 * S0(ss, par);
        G4double G31 = fG3 * S1(ss, par);
        G4double G32 = fG3 * S2(ss, par);
        // G4double G33 = fG3 * S3(ss, par);
        // G4double G34 = fG3 * S4(ss, par);
        // G4double G35 = fG3 * S5(ss, par);
        G4double B3x = G30 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) - 1. / 80. * G32 * (20. * std::pow(x, 4.) * y - 4. * std::pow(y, 5.));
        G4double B3y = G30 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) - 1. / 80. * G32 * (4. * std::pow(x, 5.) - 20. * x * std::pow(y, 4.));
        G4double B3z = G31 * (std::pow(x, 3.) * y - x * std::pow(y, 3.));

        // Decapole
        G4double G40 = fG4 * S0(ss, par);
        G4double G41 = fG4 * S1(ss, par);
        // G4double G42 = fG4 * S2(ss, par);
        // G4double G43 = fG4 * S3(ss, par);
        // G4double G44 = fG4 * S4(ss, par);
        // G4double G45 = fG4 * S5(ss, par);
        G4double B4x = G40 * (4. * std::pow(x, 3.) * y - 4. * x * std::pow(y, 3.));
        G4double B4y = G40 * (std::pow(x, 4.) - 6. * std::pow(x, 2.) * std::pow(y, 2.) + std::pow(y, 4.));
        G4double B4z = G41 * (std::pow(x, 4.) * y - 2. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.) / 5.);

        // Dodecapole
        G4double G50 = fG5 * S0(ss, par);
        // G4double G51 = fG5 * S1(ss, par);
        // G4double G52 = fG5 * S2(ss, par);
        // G4double G53 = fG5 * S3(ss, par);
        // G4double G54 = fG5 * S4(ss, par);
        // G4double G55 = fG5 * S5(ss, par);
        G4double B5x = G50 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
        G4double B5y = G50 * (5. * std::pow(y, 4.) * x - 10. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.));
        G4double B5z = 0.;

        bField[0] = B2x + B3x + B4x + B5x;
        bField[1] = B2y + B3y + B4y + B5y;
        bField[2] = B2z + B3z + B4z + B5z;

        bField *= 1. * tesla;
        return bField;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    /*
        G4ThreeVector FirstMultipoleField::GetEntranceFringingField(G4ThreeVector pos) const
        {
            G4double x = pos.x() * 0.1;
            G4double y = pos.y() * 0.1;
            G4double z = -13. - pos.z() * 0.1;

            G4double ss[1] = {z / (kFirstMultipoleAperture * 0.1)};
            G4double *par = 0;
            // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): pos.z()=%.4e\n", pos.z());
            // printf("\n---> FirstMultipoleField::GetEntranceFringingField(): ss[0]=%.4e\n", ss[0]);
            // printf("---> FirstMultipoleField::GetEntranceFringingField(): fEngeFunc(ss[0])=%.4e\n", fEngeFunc->Eval(ss[0]));

            // Gnm = G_{n}^{(m)}
            // Quadrupole
            G4double G10 = fG1 * S0(ss, par);
            G4double G11 = fG1 * S1(ss, par);
            G4double G12 = fG1 * S2(ss, par);
            G4double G13 = fG1 * S3(ss, par);
            G4double G14 = fG1 * S4(ss, par);
            G4double G15 = fG1 * S5(ss, par);
            G4double G16 = fG1 * S6(ss, par);
            G4double B1x = G10 * y - 1. / 12. * G12 * (3. * std::pow(x, 2.) * y + std::pow(y, 3.)) + 1. / 384. * G14 * (5. * std::pow(x, 4.) * y + 6. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.)) - 1. / 23040. * G16 * (7. * std::pow(x, 6.) * y + 15. * std::pow(x, 4.) * std::pow(y, 3.) + 9 * std::pow(x, 2.) * std::pow(y, 5.) + std::pow(y, 7.));
            G4double B1y = G10 * x - 1. / 12. * G12 * (3. * std::pow(y, 2.) * x + std::pow(x, 3.)) + 1. / 384. * G14 * (5. * std::pow(y, 4.) * x + 6. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.)) - 1. / 23040. * G16 * (7. * std::pow(y, 6.) * x + 15. * std::pow(y, 4.) * std::pow(x, 3.) + 9 * std::pow(y, 2.) * std::pow(x, 5.) + std::pow(x, 7.));
            G4double B1z = G11 * x * y - 1. / 12. * G13 * (std::pow(x, 3.) * y + x * std::pow(y, 3.)) + 1. / 384. * G15 * (std::pow(x, 5.) * y + 2. * std::pow(x, 3.) * std::pow(y, 3.) + x * std::pow(y, 5.));
            // printf("---> FirstMultipoleField::GetEntranceFringingField(): G12=%.4e\n", G12);

            // Hexapole
            G4double G20 = fG2 * S0(ss, par);
            G4double G21 = fG2 * S1(ss, par);
            G4double G22 = fG2 * S2(ss, par);
            G4double G23 = fG2 * S3(ss, par);
            // G4double G24 = fG2 * S4(ss, par);
            // G4double G25 = fG2 * S5(ss, par);
            G4double B2x = G20 * 2. * x * y - 1. / 48. * G22 * (12. * std::pow(x, 3.) * y + 4. * x * std::pow(y, 3.));
            G4double B2y = G20 * (std::pow(x, 2.) - std::pow(y, 2.)) - 1. / 48. * G22 * (3. * std::pow(x, 4.) + 6. * std::pow(x, 2.) * std::pow(y, 2.) - 5. * std::pow(y, 4.));
            G4double B2z = G21 * (std::pow(x, 2.) * y - std::pow(y, 3.) / 3.) - 1. / 48. * G23 * (3. * std::pow(x, 4.) * y + 2. * std::pow(x, 2.) * std::pow(y, 3.) - std::pow(y, 5.));

            // Octupole
            G4double G30 = fG3 * S0(ss, par);
            G4double G31 = fG3 * S1(ss, par);
            G4double G32 = fG3 * S2(ss, par);
            // G4double G33 = fG3 * S3(ss, par);
            // G4double G34 = fG3 * S4(ss, par);
            // G4double G35 = fG3 * S5(ss, par);
            G4double B3x = G30 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) - 1. / 80. * G32 * (20. * std::pow(x, 4.) * y - 4. * std::pow(y, 5.));
            G4double B3y = G30 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) - 1. / 80. * G32 * (4. * std::pow(x, 5.) - 20. * x * std::pow(y, 4.));
            G4double B3z = G31 * (std::pow(x, 3.) * y - x * std::pow(y, 3.));

            // Decapole
            G4double G40 = fG4 * S0(ss, par);
            G4double G41 = fG4 * S1(ss, par);
            // G4double G42 = fG4 * S2(ss, par);
            // G4double G43 = fG4 * S3(ss, par);
            // G4double G44 = fG4 * S4(ss, par);
            // G4double G45 = fG4 * S5(ss, par);
            G4double B4x = G40 * (4. * std::pow(x, 3.) * y - 4. * x * std::pow(y, 3.));
            G4double B4y = G40 * (std::pow(x, 4.) - 6. * std::pow(x, 2.) * std::pow(y, 2.) + std::pow(y, 4.));
            G4double B4z = G41 * (std::pow(x, 4.) * y - 2. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.) / 5.);

            // Dodecapole
            G4double G50 = fG5 * S0(ss, par);
            // G4double G51 = fG5 * S1(ss, par);
            // G4double G52 = fG5 * S2(ss, par);
            // G4double G53 = fG5 * S3(ss, par);
            // G4double G54 = fG5 * S4(ss, par);
            // G4double G55 = fG5 * S5(ss, par);
            G4double B5x = G50 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
            G4double B5y = G50 * (5. * std::pow(y, 4.) * x - 10. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.));
            G4double B5z = 0.;

            G4ThreeVector bField;
            bField[0] = (B1x + B2x + B3x + B4x + B5x);
            bField[1] = (B1y + B2y + B3y + B4y + B5y);
            bField[2] = (B1z + B2z + B3z + B4z + B5z);
            // bField[0] = B1x;
            // bField[1] = B1y;
            // bField[2] = B1z;
            bField *= 1. * tesla;

            return bField;
        }

        //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

        G4ThreeVector FirstMultipoleField::GetExitFringingField(G4ThreeVector pos) const
        {
            G4double x = pos.x() * 0.1;
            G4double y = pos.y() * 0.1;
            G4double z = pos.z() * 0.1 - 13.;

            G4double ss[1] = {z / (kFirstMultipoleAperture * 0.1)};
            G4double *par = 0;
            // Gnm = G_{n}^{(m)}
            // Quadrupole
            G4double G10 = fG1 * S0(ss, par);
            G4double G11 = fG1 * S1(ss, par);
            G4double G12 = fG1 * S2(ss, par);
            G4double G13 = fG1 * S3(ss, par);
            G4double G14 = fG1 * S4(ss, par);
            G4double G15 = fG1 * S5(ss, par);
            G4double G16 = fG1 * S6(ss, par);
            G4double B1x = G10 * y - 1. / 12. * G12 * (3. * std::pow(x, 2.) * y + std::pow(y, 3.)) + 1. / 384. * G14 * (5. * std::pow(x, 4.) * y + 6. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.)) - 1. / 23040. * G16 * (7. * std::pow(x, 6.) * y + 15. * std::pow(x, 4.) * std::pow(y, 3.) + 9 * std::pow(x, 2.) * std::pow(y, 5.) + std::pow(y, 7.));
            G4double B1y = G10 * x - 1. / 12. * G12 * (3. * std::pow(y, 2.) * x + std::pow(x, 3.)) + 1. / 384. * G14 * (5. * std::pow(y, 4.) * x + 6. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.)) - 1. / 23040. * G16 * (7. * std::pow(y, 6.) * x + 15. * std::pow(y, 4.) * std::pow(x, 3.) + 9 * std::pow(y, 2.) * std::pow(x, 5.) + std::pow(x, 7.));
            G4double B1z = G11 * x * y - 1. / 12. * G13 * (std::pow(x, 3.) * y + x * std::pow(y, 3.)) + 1. / 384. * G15 * (std::pow(x, 5.) * y + 2. * std::pow(x, 3.) * std::pow(y, 3.) + x * std::pow(y, 5.));

            // Hexapole
            G4double G20 = fG2 * S0(ss, par);
            G4double G21 = fG2 * S1(ss, par);
            G4double G22 = fG2 * S2(ss, par);
            G4double G23 = fG2 * S3(ss, par);
            // G4double G24 = fG2 * S4(ss, par);
            // G4double G25 = fG2 * S5(ss, par);
            G4double B2x = G20 * 2. * x * y - 1. / 48. * G22 * (12. * std::pow(x, 3.) * y + 4. * x * std::pow(y, 3.));
            G4double B2y = G20 * (std::pow(x, 2.) - std::pow(y, 2.)) - 1. / 48. * G22 * (3. * std::pow(x, 4.) + 6. * std::pow(x, 2.) * std::pow(y, 2.) - 5. * std::pow(y, 4.));
            G4double B2z = G21 * (std::pow(x, 2.) * y - std::pow(y, 3.) / 3.) - 1. / 48. * G23 * (3. * std::pow(x, 4.) * y + 2. * std::pow(x, 2.) * std::pow(y, 3.) - std::pow(y, 5.));

            // Octupole
            G4double G30 = fG3 * S0(ss, par);
            G4double G31 = fG3 * S1(ss, par);
            G4double G32 = fG3 * S2(ss, par);
            // G4double G33 = fG3 * S3(ss, par);
            // G4double G34 = fG3 * S4(ss, par);
            // G4double G35 = fG3 * S5(ss, par);
            G4double B3x = G30 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) - 1. / 80. * G32 * (20. * std::pow(x, 4.) * y - 4. * std::pow(y, 5.));
            G4double B3y = G30 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) - 1. / 80. * G32 * (4. * std::pow(x, 5.) - 20. * x * std::pow(y, 4.));
            G4double B3z = G31 * (std::pow(x, 3.) * y - x * std::pow(y, 3.));

            // Decapole
            G4double G40 = fG4 * S0(ss, par);
            G4double G41 = fG4 * S1(ss, par);
            // G4double G42 = fG4 * S2(ss, par);
            // G4double G43 = fG4 * S3(ss, par);
            // G4double G44 = fG4 * S4(ss, par);
            // G4double G45 = fG4 * S5(ss, par);
            G4double B4x = G40 * (4. * std::pow(x, 3.) * y - 4. * x * std::pow(y, 3.));
            G4double B4y = G40 * (std::pow(x, 4.) - 6. * std::pow(x, 2.) * std::pow(y, 2.) + std::pow(y, 4.));
            G4double B4z = G41 * (std::pow(x, 4.) * y - 2. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.) / 5.);

            // Dodecapole
            G4double G50 = fG5 * S0(ss, par);
            // G4double G51 = fG5 * S1(ss, par);
            // G4double G52 = fG5 * S2(ss, par);
            // G4double G53 = fG5 * S3(ss, par);
            // G4double G54 = fG5 * S4(ss, par);
            // G4double G55 = fG5 * S5(ss, par);
            G4double B5x = G50 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
            G4double B5y = G50 * (5. * std::pow(y, 4.) * x - 10. * std::pow(y, 2.) * std::pow(x, 3.) + std::pow(x, 5.));
            G4double B5z = 0.;

            G4ThreeVector bField;
            bField[0] = B1x + B2x + B3x + B4x + B5x;
            bField[1] = B1y + B2y + B3y + B4y + B5y;
            bField[2] = B1z + B2z + B3z + B4z + B5z;
            // bField[0] = B1x;
            // bField[1] = B1y;
            // bField[2] = B1z;
            bField *= 1. * tesla;

            return bField;
        }
    */
    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    G4double FirstMultipoleField::S0(G4double *x, G4double *par) const { return fEngeFunc->Eval(x[0]); }
    G4double FirstMultipoleField::S1(G4double *x, G4double *par) const { return fEngeFunc->Derivative(x[0]); }
    G4double FirstMultipoleField::S2(G4double *x, G4double *par) const { return fEngeFunc->Derivative2(x[0]); }
    G4double FirstMultipoleField::S3(G4double *x, G4double *par) const { return fEngeFunc->Derivative3(x[0]); }
    G4double FirstMultipoleField::S4(G4double *x, G4double *par) const
    {
        TF1 f1(
            "f1", [this](G4double *xx, G4double *params)
            { return S3(xx, params); },
            -kFirstMultipoleLength / kFirstMultipoleAperture, kFirstMultipoleLength / kFirstMultipoleAperture, 0);
        return f1.Derivative(x[0]);
    }
    G4double FirstMultipoleField::S5(G4double *x, G4double *par) const
    {
        TF1 f1(
            "f1", [this](G4double *xx, G4double *params)
            { return S4(xx, params); },
            -kFirstMultipoleLength / kFirstMultipoleAperture, kFirstMultipoleLength / kFirstMultipoleAperture, 0);
        return f1.Derivative(x[0]);
    }
    G4double FirstMultipoleField::S6(G4double *x, G4double *par) const
    {
        TF1 f1(
            "f1", [this](G4double *xx, G4double *params)
            { return S5(xx, params); },
            -kFirstMultipoleLength / kFirstMultipoleAperture, kFirstMultipoleLength / kFirstMultipoleAperture, 0);
        return f1.Derivative(x[0]);
    }
}
