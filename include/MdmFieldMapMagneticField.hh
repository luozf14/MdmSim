#ifndef MdmSimMdmFieldMapMagneticField_h
#define MdmSimMdmFieldMapMagneticField_h 1

#include "G4MagneticField.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "MdmFieldMap.h"

#include <string>

namespace MdmSim
{
    struct MdmFieldMapPaths
    {
        std::string multipole;
        std::string dipoleEntrance;
        std::string dipoleSector;
        std::string dipoleExit;
    };

    class MdmFieldMapMagneticField : public G4MagneticField
    {
    public:
        static MdmFieldMapMagneticField *CreateMultipole(const std::string &path,
                                                         G4double mdmAngle,
                                                         G4ThreeVector position,
                                                         G4double dipoleProbe,
                                                         G4double multipoleProbe);
        static MdmFieldMapMagneticField *CreateDipole(const MdmFieldMapPaths &paths,
                                                      G4double mdmAngle,
                                                      G4double dipoleProbe,
                                                      G4double multipoleProbe);

        void GetFieldValue(const G4double point[4], G4double *bField) const override;

    private:
        enum class Mode
        {
            Multipole,
            Dipole
        };

        MdmFieldMapMagneticField(Mode mode, G4double mdmAngle, G4ThreeVector position);

        void LoadMultipole(const std::string &path, G4double dipoleProbe, G4double multipoleProbe);
        void LoadDipole(const MdmFieldMapPaths &paths, G4double dipoleProbe, G4double multipoleProbe);

        void ValidateMap(const MdmFieldMap &map,
                         const std::string &expectedMagnet,
                         const std::string &expectedCoordinateSystem,
                         G4double dipoleProbe,
                         G4double multipoleProbe) const;
        void ValidateProbe(const MdmFieldMap &map, G4double dipoleProbe, G4double multipoleProbe) const;

        Vec3 GetMultipoleFieldTesla(const G4ThreeVector &globalPos) const;
        Vec3 GetDipoleFieldTesla(const G4ThreeVector &globalPos) const;
        bool InMapBounds(const MdmFieldMap &map, double x, double y, double z) const;

        Mode fMode;
        G4ThreeVector fPosition;
        G4RotationMatrix fRotation;
        G4RotationMatrix fInverseRotation;

        MdmFieldMap fMultipoleMap;
        MdmFieldMap fDipoleEntranceMap;
        MdmFieldMap fDipoleSectorMap;
        MdmFieldMap fDipoleExitMap;
    };
}

#endif
