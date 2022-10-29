#include <iostream>
using namespace std;

// User Action classes
#include "include/SteppingAction.h"
#include "include/RunAction.h"
#include "include/TrackingAction.h"
#include "include/EventAction.h"
#include "include/ActionInitialization.h"

// User Initialization classes
#include "include/DetectorConstruction.h"
#include "include/PrimaryGeneratorAction.h"

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
//#include "G4ScoringManager.hh"

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

void print_aux(const G4GDMLAuxListType *auxInfoList, G4String prepend = "|")
{
  for (std::vector<G4GDMLAuxStructType>::const_iterator
           iaux = auxInfoList->begin();
       iaux != auxInfoList->end(); iaux++)
  {
    G4String str = iaux->type;
    G4String val = iaux->value;
    G4String unit = iaux->unit;

    G4cout << prepend << str << " : " << val << " " << unit << G4endl;

    if (iaux->auxList)
      print_aux(iaux->auxList, prepend + "|");
  }
  return;
}

int main(int argc, char **argv)
{
#ifdef G4MULTITHREADED
  G4MTRunManager *runManager = new G4MTRunManager();
#else
  G4RunManager *runManager = new G4RunManager();
#endif

  // GDML load
  G4GDMLParser parser;
  parser.SetOverlapCheck(true);
  parser.Read("../geo.gdml");

  // Physics list setup
  G4VModularPhysicsList *physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option2());
  runManager->SetUserInitialization(physicsList);

  // User action and initialization classes
  runManager->SetUserInitialization(new Detector(parser));

  runManager->SetUserInitialization(new ActionInitialization());
  // runManager->Initialize();

  // Initializing the visualization manager
  auto *visManager = new G4VisExecutive();
  visManager->Initialize();

  ///////////////////////////////////////////////////////////////////////
  //
  // Example how to retrieve Auxiliary Information
  //

  G4cout << std::endl;

  const G4LogicalVolumeStore *lvs = G4LogicalVolumeStore::GetInstance();
  std::vector<G4LogicalVolume *>::const_iterator lvciter;
  for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++)
  {
    G4GDMLAuxListType auxInfo = parser.GetVolumeAuxiliaryInformation(*lvciter);

    if (auxInfo.size() > 0)
      G4cout << "Auxiliary Information is found for Logical Volume :  "
             << (*lvciter)->GetName() << G4endl;

    print_aux(&auxInfo);
  }

  // now the 'global' auxiliary info
  G4cout << std::endl;
  G4cout << "Global auxiliary info:" << std::endl;
  G4cout << std::endl;

  print_aux(parser.GetAuxList());

  G4cout << std::endl;

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
