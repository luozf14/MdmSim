#include "MdmFieldMapMagneticField.hh"

#include "Constants.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
    constexpr double kProbeTolerance = 2.0e-6;
    // RAYTRACE uses C = 3.0e10 cm/s exactly when converting B rho to field.
    // Keep the maps in physical Tesla and only compensate that convention
    // against Geant4's exact c_light.
    const G4double kRaytraceToGeantFieldScale = (300.0 * mm / ns) / c_light;

    double ToRadians(double degrees)
    {
        return degrees * M_PI / 180.0;
    }

    double HeaderDouble(const MdmFieldMap &map, const std::string &key, double fallback)
    {
        const auto item = map.h.extra.find(key);
        if (item == map.h.extra.end())
        {
            return fallback;
        }
        return std::stod(item->second);
    }

    bool NearlyEqual(double lhs, double rhs)
    {
        const double scale = std::max({1.0, std::abs(lhs), std::abs(rhs)});
        return std::abs(lhs - rhs) <= kProbeTolerance * scale;
    }
}

namespace MdmSim
{
    MdmFieldMapMagneticField *MdmFieldMapMagneticField::CreateMultipole(const std::string &path,
                                                                        G4double mdmAngle,
                                                                        G4ThreeVector position,
                                                                        G4double dipoleProbe,
                                                                        G4double multipoleProbe)
    {
        auto *field = new MdmFieldMapMagneticField(Mode::Multipole, mdmAngle, position);
        field->LoadMultipole(path, dipoleProbe, multipoleProbe);
        return field;
    }

    MdmFieldMapMagneticField *MdmFieldMapMagneticField::CreateDipole(const MdmFieldMapPaths &paths,
                                                                     G4double mdmAngle,
                                                                     G4double dipoleProbe,
                                                                     G4double multipoleProbe)
    {
        G4ThreeVector dipoleOrigin(0., 0., kFirstArmLength);
        G4RotationMatrix rotation;
        rotation.rotateY(mdmAngle);
        auto *field = new MdmFieldMapMagneticField(Mode::Dipole, mdmAngle, rotation(dipoleOrigin));
        field->LoadDipole(paths, dipoleProbe, multipoleProbe);
        return field;
    }

    MdmFieldMapMagneticField::MdmFieldMapMagneticField(Mode mode, G4double mdmAngle, G4ThreeVector position)
        : fMode(mode), fPosition(position)
    {
        fRotation.rotateY(mdmAngle);
        fInverseRotation = fRotation.inverse();
    }

    void MdmFieldMapMagneticField::LoadMultipole(const std::string &path,
                                                 G4double dipoleProbe,
                                                 G4double multipoleProbe)
    {
        fMultipoleMap.Load(path);
        ValidateMap(fMultipoleMap, "Multipole", "multipole_local_cartesian", dipoleProbe, multipoleProbe);
    }

    void MdmFieldMapMagneticField::LoadDipole(const MdmFieldMapPaths &paths,
                                              G4double dipoleProbe,
                                              G4double multipoleProbe)
    {
        fDipoleEntranceMap.Load(paths.dipoleEntrance);
        fDipoleSectorMap.Load(paths.dipoleSector);
        fDipoleExitMap.Load(paths.dipoleExit);

        ValidateMap(fDipoleEntranceMap, "DipoleEntrance", "dipole_entrance_frame", dipoleProbe, multipoleProbe);
        ValidateMap(fDipoleSectorMap, "DipoleSector", "dipole_sector_frame", dipoleProbe, multipoleProbe);
        ValidateMap(fDipoleExitMap, "DipoleExit", "dipole_exit_frame", dipoleProbe, multipoleProbe);
    }

    void MdmFieldMapMagneticField::ValidateMap(const MdmFieldMap &map,
                                               const std::string &expectedMagnet,
                                               const std::string &expectedCoordinateSystem,
                                               G4double dipoleProbe,
                                               G4double multipoleProbe) const
    {
        if (map.h.magnet != expectedMagnet)
        {
            throw std::runtime_error("field map magnet mismatch: expected " + expectedMagnet + ", got " + map.h.magnet);
        }
        if (map.h.coordinate_system != expectedCoordinateSystem)
        {
            throw std::runtime_error("field map coordinate-system mismatch for " + expectedMagnet);
        }
        ValidateProbe(map, dipoleProbe, multipoleProbe);
    }

    void MdmFieldMapMagneticField::ValidateProbe(const MdmFieldMap &map,
                                                 G4double dipoleProbe,
                                                 G4double multipoleProbe) const
    {
        if (!NearlyEqual(map.h.mdm_dipole_probe, dipoleProbe) ||
            !NearlyEqual(map.h.mdm_multipole_probe, multipoleProbe))
        {
            std::ostringstream message;
            message << std::setprecision(17);
            message << "field map probe metadata mismatch for " << map.h.magnet
                    << ": map dipole=" << map.h.mdm_dipole_probe
                    << ", map multipole=" << map.h.mdm_multipole_probe
                    << ", requested dipole=" << dipoleProbe
                    << ", requested multipole=" << multipoleProbe;
            throw std::runtime_error(message.str());
        }
    }

    void MdmFieldMapMagneticField::GetFieldValue(const G4double point[4], G4double *bField) const
    {
        const G4ThreeVector globalPos(point[0], point[1], point[2]);
        const Vec3 localTesla = fMode == Mode::Multipole ? GetMultipoleFieldTesla(globalPos) : GetDipoleFieldTesla(globalPos);
        const G4ThreeVector localField(localTesla.x * kRaytraceToGeantFieldScale * tesla,
                                       localTesla.y * kRaytraceToGeantFieldScale * tesla,
                                       localTesla.z * kRaytraceToGeantFieldScale * tesla);
        const G4ThreeVector globalField = fRotation(localField);
        bField[0] = globalField.x();
        bField[1] = globalField.y();
        bField[2] = globalField.z();
    }

    Vec3 MdmFieldMapMagneticField::GetMultipoleFieldTesla(const G4ThreeVector &globalPos) const
    {
        const G4ThreeVector localMm = fInverseRotation(globalPos - fPosition);
        return fMultipoleMap.FieldTesla(localMm.x() / cm, localMm.y() / cm, localMm.z() / cm);
    }

    Vec3 MdmFieldMapMagneticField::GetDipoleFieldTesla(const G4ThreeVector &globalPos) const
    {
        const G4ThreeVector localMm = fInverseRotation(globalPos - fPosition);
        const double x = localMm.x() / cm;
        const double y = localMm.y() / cm;
        const double z = localMm.z() / cm;

        const double alpha = ToRadians(HeaderDouble(fDipoleEntranceMap, "dipole_alpha_deg", 0.0));
        const double beta = ToRadians(HeaderDouble(fDipoleExitMap, "dipole_beta_deg", 0.0));
        const double phiDeg = HeaderDouble(fDipoleSectorMap, "dipole_sector_angle_deg", 100.0);
        const double radius = HeaderDouble(fDipoleSectorMap, "dipole_reference_radius_cm", kDipoleFieldRadius / cm);
        const double rotation = ToRadians(phiDeg) - alpha - beta;
        const double cosAlpha = std::cos(alpha);
        const double sinAlpha = std::sin(alpha);
        const double cosRot = std::cos(rotation);
        const double sinRot = std::sin(rotation);
        const double cosPb = std::cos(ToRadians(0.5 * phiDeg) - beta);
        const double sinPb = std::sin(ToRadians(0.5 * phiDeg) - beta);
        const double sinHalfPhi = std::sin(ToRadians(0.5 * phiDeg));
        const double tx = 2.0 * radius * sinHalfPhi * sinPb;
        const double tz = 2.0 * radius * sinHalfPhi * cosPb;

        const double xB = -x * cosAlpha - z * sinAlpha;
        const double zB = x * sinAlpha - z * cosAlpha;
        if (InMapBounds(fDipoleEntranceMap, xB, y, zB))
        {
            return fDipoleEntranceMap.FieldTesla(xB, y, zB);
        }

        const double xC = -zB * sinRot - xB * cosRot - tx;
        const double zC = -zB * cosRot + xB * sinRot - tz;
        if (InMapBounds(fDipoleExitMap, xC, y, zC))
        {
            return fDipoleExitMap.FieldTesla(xC, y, zC);
        }

        const double radialDistance = std::sqrt((x + radius) * (x + radius) + z * z);
        const double dr = radialDistance - radius;
        const double theta = std::atan2(z, x + radius);
        const double s = radius * theta;
        if (InMapBounds(fDipoleSectorMap, dr, y, s))
        {
            return fDipoleSectorMap.FieldTesla(dr, y, s);
        }
        return {};
    }

    bool MdmFieldMapMagneticField::InMapBounds(const MdmFieldMap &map, double x, double y, double z) const
    {
        const auto within = [](double coord, double origin, double spacing, int count)
        {
            const double maxCoord = origin + spacing * static_cast<double>(count - 1);
            const double tolerance = 1.0e-9;
            return coord >= origin - tolerance && coord <= maxCoord + tolerance;
        };
        return within(x, map.h.origin_cm.x, map.h.step_cm.x, map.h.nx) &&
               within(y, map.h.origin_cm.y, map.h.step_cm.y, map.h.ny) &&
               within(z, map.h.origin_cm.z, map.h.step_cm.z, map.h.nz);
    }
}
