#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "ReactionPhysics.hh"
#include "nlohmann/json.hpp"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "G4VModularPhysicsList.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include <filesystem>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>

using namespace MdmSim;
using json = nlohmann::json;

namespace
{
    std::filesystem::path ResolvePath(const std::filesystem::path &base, const std::filesystem::path &path)
    {
        if (path.is_absolute())
        {
            return path.lexically_normal();
        }
        return (base / path).lexically_normal();
    }

    std::string ResolveMapPath(const json &config,
                               const char *key,
                               const char *defaultName,
                               const std::filesystem::path &mapDirectory)
    {
        const std::filesystem::path configured = config.contains(key) ? config[key].get<std::string>() : defaultName;
        return ResolvePath(mapDirectory, configured).string();
    }

    G4double RequiredReactionValue(const json &config, const char *key)
    {
        if (!config.contains(key))
        {
            throw std::runtime_error(std::string("ReactionEnabled requires config key: ") + key);
        }
        return config.at(key).get<G4double>();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char **argv)
{
    // Detect interactive mode (if no arguments) and define UI session
    //
    if (argc != 2)
    {
        G4cerr << "--->Error: wrong input parameters!"
               << "\n--->Usage: ./MdmSim <config.json>" << G4endl;
        return 0;
    }

    // Parse the configure JSON file
    //
    std::string configFile = argv[1];
    std::ifstream configStream(configFile.c_str());
    json config = json::parse(configStream);
    const std::filesystem::path configDirectory = std::filesystem::absolute(std::filesystem::path(configFile)).parent_path();
    const std::filesystem::path fieldMapDirectory = ResolvePath(configDirectory, config.value("FieldMapDirectory", std::string("../field")));

    G4bool interactive = config["Interactive"].get<G4bool>();
    G4String macroName = config["MarcoName"].get<std::string>();
    G4int processNumber = config["ProcessNumber"].get<G4int>();
    G4bool reactionEnabled = config.value("ReactionEnabled", false);

    std::map<std::string, G4double> primaryParameters; // parameters for PrimaryGeneratorAction
    primaryParameters["BeamZ"] = config.value("BeamZ", 6.0);
    primaryParameters["BeamA"] = config.value("BeamA", 12.0);
    primaryParameters["BeamCharge"] = config.value("BeamCharge", primaryParameters["BeamZ"]);
    primaryParameters["BeamEnergyInMeV"] = config.value("BeamEnergyInMeV", 15.0);
    primaryParameters["MdmAngleInDeg"] = config["MdmAngleInDeg"].get<G4double>();

    std::map<std::string, G4double> detectorParameters; // parameters for DectectorConstruction
    detectorParameters["TargetRotationAngleInDeg"] = config["TargetRotationAngleInDeg"].get<G4double>();
    detectorParameters["TargetThicknessInMgCm2"] = config["TargetThicknessInMgCm2"].get<G4double>();
    detectorParameters["UseDeltaE"] = config["UseDeltaE"].get<G4bool>();
    detectorParameters["SiDetectorAngleInDeg"] = config["SiDetectorAngleInDeg"].get<G4double>();
    detectorParameters["MdmAngleInDeg"] = config["MdmAngleInDeg"].get<G4double>();
    detectorParameters["BeamZ"] = primaryParameters["BeamZ"];
    detectorParameters["BeamA"] = primaryParameters["BeamA"];
    detectorParameters["BeamCharge"] = primaryParameters["BeamCharge"];
    detectorParameters["ReactionEnabled"] = reactionEnabled ? 1.0 : 0.0;
    detectorParameters["FirstMultipoleProbe"] = config["FirstMultipoleProbe"].get<G4double>();
    detectorParameters["DipoleProbe"] = config["DipoleProbe"].get<G4double>();
    detectorParameters["PpacVacuumInTorr"] = config["PpacVacuumInTorr"].get<G4double>();
    detectorParameters["PpacLengthInCm"] = config["PpacLengthInCm"].get<G4double>();

    MdmFieldMapPaths fieldMapPaths;
    fieldMapPaths.multipole = ResolveMapPath(config, "MultipoleFieldMap", "Multipole.bin", fieldMapDirectory);
    fieldMapPaths.dipoleEntrance = ResolveMapPath(config, "DipoleEntranceFieldMap", "DipoleEntrance.bin", fieldMapDirectory);
    fieldMapPaths.dipoleSector = ResolveMapPath(config, "DipoleSectorFieldMap", "DipoleSector.bin", fieldMapDirectory);
    fieldMapPaths.dipoleExit = ResolveMapPath(config, "DipoleExitFieldMap", "DipoleExit.bin", fieldMapDirectory);

    std::map<std::string, G4double> eventParameters; // parameters for EventAction
    eventParameters["SiDetectorEnergyResolution"] = config["SiDetectorEnergyResolution"].get<G4double>();
    eventParameters["TdcResolutionInNs"] = config["TdcResolutionInNs"].get<G4double>();
    eventParameters["MdmAngleInDeg"] = config["MdmAngleInDeg"].get<G4double>();
    eventParameters["FirstMultipoleProbe"] = config["FirstMultipoleProbe"].get<G4double>();
    eventParameters["DipoleProbe"] = config["DipoleProbe"].get<G4double>();
    eventParameters["BeamCharge"] = primaryParameters["BeamCharge"];

    std::map<std::string, G4double> reactionParameters;
    if (reactionEnabled)
    {
        reactionParameters["BeamZ"] = primaryParameters["BeamZ"];
        reactionParameters["BeamA"] = primaryParameters["BeamA"];
        reactionParameters["BeamCharge"] = primaryParameters["BeamCharge"];
        reactionParameters["ReactionProbability"] = config.value("ReactionProbability", 1.0);
        reactionParameters["TargetZ"] = RequiredReactionValue(config, "ReactionTargetZ");
        reactionParameters["TargetA"] = RequiredReactionValue(config, "ReactionTargetA");
        reactionParameters["LightProductZ"] = RequiredReactionValue(config, "ReactionLightZ");
        reactionParameters["LightProductA"] = RequiredReactionValue(config, "ReactionLightA");
        reactionParameters["LightProductCharge"] = RequiredReactionValue(config, "ReactionLightCharge");
        reactionParameters["LightProductExMeV"] = config.value("ReactionLightExMeV", 0.0);
        reactionParameters["HeavyProductZ"] = RequiredReactionValue(config, "ReactionHeavyZ");
        reactionParameters["HeavyProductA"] = RequiredReactionValue(config, "ReactionHeavyA");
        reactionParameters["HeavyProductCharge"] = RequiredReactionValue(config, "ReactionHeavyCharge");
        reactionParameters["HeavyProductExMeV"] = config.value("ReactionHeavyExMeV", 0.0);

        detectorParameters["ReactionLightZ"] = reactionParameters["LightProductZ"];
        detectorParameters["ReactionLightA"] = reactionParameters["LightProductA"];
        detectorParameters["ReactionLightCharge"] = reactionParameters["LightProductCharge"];
        detectorParameters["ReactionLightExMeV"] = reactionParameters["LightProductExMeV"];
        detectorParameters["ReactionHeavyZ"] = reactionParameters["HeavyProductZ"];
        detectorParameters["ReactionHeavyA"] = reactionParameters["HeavyProductA"];
        detectorParameters["ReactionHeavyCharge"] = reactionParameters["HeavyProductCharge"];
        detectorParameters["ReactionHeavyExMeV"] = reactionParameters["HeavyProductExMeV"];
    }

    std::map<std::string, std::variant<G4int, std::map<std::string, G4double>>> actionInitParameters; // parameters for ActionInitialization
    actionInitParameters["ProcessNumber"] = processNumber;
    actionInitParameters["PrimaryParameters"] = primaryParameters;
    actionInitParameters["EventParameters"] = eventParameters;

    // Optionally: choose a different Random engine...
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    // set random seed with system time
    G4long seed = time(NULL);
    seed += 473879 * processNumber;
    G4Random::setTheSeed(seed);

    // use G4SteppingVerboseWithUnits
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);

    // Construct the default run manager
    //
    auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);

    // Set mandatory initialization classes
    //
    // Detector construction
    DetectorConstruction *detector = new DetectorConstruction();
    detector->SetFieldMapPaths(fieldMapPaths);
    detector->ParseParams(detectorParameters);
    runManager->SetUserInitialization(detector);

    // Physics list
    G4VModularPhysicsList *physicsList = new QBBC;
    physicsList->SetVerboseLevel(0);
    if (reactionEnabled)
    {
        auto *reactionPhysics = new ReactionPhysics;
        reactionPhysics->SetReactionParams(reactionParameters);
        physicsList->RegisterPhysics(reactionPhysics);
    }
    runManager->SetUserInitialization(physicsList);

    // User action initialization
    ActionInitialization *action = new ActionInitialization();
    action->SetParameters(actionInitParameters);
    runManager->SetUserInitialization(action);

    // Initialize visualization
    //
    G4VisManager *visManager = new G4VisExecutive;
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    //
    if (!interactive)
    {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = macroName;
        UImanager->ApplyCommand(command + fileName);
    }
    else
    {
        // interactive mode
        G4UIExecutive *ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    delete visManager;
    delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
