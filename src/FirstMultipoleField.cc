#include "FirstMultipoleField.hh"
#include "Constants.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

namespace TexPPACSim
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    FirstMultipoleField::FirstMultipoleField()
    {
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    FirstMultipoleField::~FirstMultipoleField()
    {
    }

    void FirstMultipoleField::GetFieldValue(const G4double PositionAndTime[4], G4double *bField) const
    {
        G4double multipoleHallProbe = 2780.84;
        G4double fBQR = -1. * multipoleHallProbe * 1e-4 * kJeffParameters[5];
        G4double fBHR = fBQR * kJeffParameters[1] / kJeffParameters[0];
        G4double fBOR = fBQR * kJeffParameters[2] / kJeffParameters[0];
        G4double fBDR = fBQR * kJeffParameters[3] / kJeffParameters[0];
        G4double fBDDR = fBQR * kJeffParameters[4] / kJeffParameters[0];

        G4double G1 = fBQR / std::pow(kFirstMultipoleAperture, 1.);
        G4double G2 = fBHR / std::pow(kFirstMultipoleAperture, 2.);
        G4double G3 = fBOR / std::pow(kFirstMultipoleAperture, 3.);
        G4double G4 = fBDR / std::pow(kFirstMultipoleAperture, 4.);
        G4double G5 = fBDDR / std::pow(kFirstMultipoleAperture, 5.);

        G4double x = -PositionAndTime[0];
        G4double y = PositionAndTime[1];

        bField[0] = G1 * y + G2 * (2. * x * y) + G3 * (3. * std::pow(x, 2.) * y - std::pow(y, 3.)) + G4 * 4. * (std::pow(x, 3.) * y - x * std::pow(y, 3.)) + G5 * (5. * std::pow(x, 4.) * y - 10. * std::pow(x, 2.) * std::pow(y, 3.) + std::pow(y, 5.));
        bField[1] = G1 * x + G2 * (std::pow(x, 2.) - std::pow(y, 2.)) + G3 * (std::pow(x, 3.) - 3. * x * std::pow(y, 2.)) + G4 * (std::pow(x, 4.) - 6. * std::pow(x * y, 2.) + std::pow(y, 4.)) + G5 * (std::pow(x, 5.) - 10. * std::pow(x, 3.) * std::pow(y, 2.) + 5. * x * std::pow(y, 4.));
        bField[2] = 0.;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
