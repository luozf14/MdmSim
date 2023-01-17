This program is under developing!
# MdmSim
This program provides simulation of using the MDM sprectrometer and a focal plane detector. The MDM spectrometer' s geometry and fields are precisely migrated from the original RAYTRACE ``rayin.dat``.

<img src="https://github.com/luozf14/MdmSim/raw/main/Demo2.jpg" width="800" alt="Demo"/><br/>

## Prerequisites
- Geant4 v11 (v10 does not work!)
- ROOT V6
- C++17
- CMake>=3.16

## How to use
### Compile
Make a build directory and enter it.
```
$ mkdir build && cd build
```

cmake & make 
```
$ cmake ../ && make
```

### Configuration
This program takes JSON file as configuration (it is mandatory!). The example config file is ``config/config.json``. Here below shows the content of this JSON file.
```
{
    "Interactive": true,
    "MarcoName": "run.mac",
    "ProcessNumber": 0,
    "TargetRotationAngleInDeg": 0.0,
    "TargetThicknessInMgCm2": 159.0,
    "TdcResolutionInNs": 0.25,
    "UseDeltaE": false,
    "SiDetectorAngleInDeg": 90.0,
    "SiDetectorEnergyResolution": 0.1,
    "MdmAngleInDeg": 0.0,
    "FirstMultipoleProbe": 2780.84,
    "DipoleProbe": 3916.68,
    "PpacVacuumInTorr": 4.0,
    "PpacLengthInCm": 42.0
}
```
- Interactive: false - batch mode, true - interactive mode.
- MarcoName: The ``.mac`` file defines how many particles you want to fire. Only valid for batch mode.
- ProcessNumber: For example, ProcessNumner=0 will give ``SimData~0.root``. Integer.
- TargetRotationAngleInDeg: Rotation (rotate along Y-axis using left-hand rule) angle of the target. Double.
- TargetThicknessInMgCm2: Target thickness in mg/cm<sup>2</sup>. Double.
- TdcResolutionInNs: The TDC resolution in nano second. Double.
- UseDeltaE: false - no ΔE silicon detector, true - add ΔE silicon detector.
- SiDetectorAngleInDeg: Rotation (rotate along Y-axis using left-hand rule) angle of the silicon detectors. Double.
- SiDetectorEnergyResolution: Energy resulution of the silicon detector in %. Double.
- MdmAngleInDeg: MDM rotation (rotate along Y-axis using right-hand rule) angle. Double.
- FirstMultipoleProbe: Multipole hall probe value. Double.
- DipoleProbe: Dipole hall probe value. Double.
- PpacVacuumInTorr: PPAC chamber pressure in Torr. Double.
- PpacLengthInCm: Distance between the two PPACs in cm. Double.

### Run
```
$ ./MdmSim ../config/config.json
```

### Output
The output data file is ``SimData~<ProcessNumber>.root``. 
