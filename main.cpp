#include <iostream>
using namespace std;

#include "G4Types.hh"

// User Action classes
#include "include/ActionInitialization.h"

// User Initialization classes
#include "include/DetectorConstruction.h"
#include "include/PrimaryGeneratorAction.h"

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4ScoringManager.hh"

// User Interface classes
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

// Visualization managers
#define G4VIS_USE_OPENGLX
#define G4VIS_USE_OPENGLQT
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

// Physics configuration
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"

// GDML
#include "G4GDMLParser.hh"
#include "G4LogicalVolumeStore.hh"

int main(int argc, char **argv)
{
  G4MTRunManager *runManager = new G4MTRunManager();

  // GDML load
  G4GDMLParser parser;
  //parser.SetOverlapCheck(true);
  parser.Read("geo.gdml");

  // Physics list setup
  G4VModularPhysicsList *physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option2());
  runManager->SetUserInitialization(physicsList);

  G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
  opticalPhysics->Configure(kScintillation, true);

  physicsList->RegisterPhysics(new G4OpticalPhysics());

  // User initialization classes
  runManager->SetUserInitialization(new Detector(parser));
  runManager->SetUserInitialization(new ActionInitialization());

  // Initializing the visualization manager
  auto *visManager = new G4VisExecutive();
  visManager->Initialize();

  // User interface
  auto *uiExecutive = new G4UIExecutive(argc, argv, "Qt");
  auto *uiManager = G4UImanager::GetUIpointer();

  // Running the macro file passed as parameter
  uiManager->ApplyCommand("/control/execute " + G4String(argv[1]));
  uiExecutive->SessionStart();

  delete visManager;
  delete uiExecutive;
  delete runManager;
  return 0;
}
