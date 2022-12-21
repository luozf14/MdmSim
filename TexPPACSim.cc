#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "nlohmann/json.hpp"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"

#include <map>
#include <variant>

using namespace TexPPACSim;
using json = nlohmann::json;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char **argv)
{
    // Detect interactive mode (if no arguments) and define UI session
    //
    if (argc != 2)
    {
        G4cerr << "--->Error: wrong input parameters!"
               << "\n--->Usage: ./TexPPACSim <config.json>" << G4endl;
        return 0;
    }

    // Parse the configure JSON file
    //
    std::string configFile = argv[1];
    std::ifstream configStream(configFile.c_str());
    json config = json::parse(configStream);

    G4bool interactive = config["Interactive"].get<G4bool>();
    G4String macroName = config["MarcoName"].get<std::string>();
    G4int processNumber = config["ProcessNumber"].get<G4int>();

    std::map<std::string, G4double> detectorParameters; // parameters for DectectorConstruction
    detectorParameters["TargetRotationAngleInDeg"] = config["TargetRotationAngleInDeg"].get<G4double>();
    detectorParameters["TargetThicknessInMgCm2"] = config["TargetThicknessInMgCm2"].get<G4double>();
    detectorParameters["UseDeltaE"] = config["UseDeltaE"].get<G4bool>();
    detectorParameters["SiDetectorAngleInDeg"] = config["SiDetectorAngleInDeg"].get<G4double>();
    detectorParameters["MdmAngleInDeg"] = config["MdmAngleInDeg"].get<G4double>();
    detectorParameters["FirstMultipoleProbe"] = config["FirstMultipoleProbe"].get<G4double>();
    detectorParameters["DipoleProbe"] = config["DipoleProbe"].get<G4double>();


    std::map<std::string, G4double> eventParameters; // parameters for EventAction
    eventParameters["SiDetectorEnergyResolution"] = config["SiDetectorEnergyResolution"].get<G4double>();
    eventParameters["TdcResolutionInNs"] = config["TdcResolutionInNs"].get<G4double>();

    std::map<std::string, std::variant<G4int, std::map<std::string, G4double>>> actionInitParameters; // parameters for ActionInitialization
    actionInitParameters["ProcessNumber"] = processNumber;
    actionInitParameters["EventParameters"] = eventParameters;

    // Optionally: choose a different Random engine...
    G4Random::setTheEngine(new CLHEP::MTwistEngine);
    // set random seed with system time
    G4long seed = time(NULL);
    seed += 473879 * processNumber;
    G4Random::setTheSeed(seed);

    // use G4SteppingVerboseWithUnits
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);

    // Construct the default run manager
    //
    auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

    // Set mandatory initialization classes
    //
    // Detector construction
    DetectorConstruction *detector = new DetectorConstruction();
    detector->ParseParams(detectorParameters);
    runManager->SetUserInitialization(detector);

    // Physics list
    G4VModularPhysicsList *physicsList = new QBBC;
    physicsList->SetVerboseLevel(0);
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
