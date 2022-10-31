#include "DetectorConstruction.h"
#include "SensitiveDetector.h"

#include "G4SDManager.hh"
#include "G4GDMLParser.hh"

#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"

#include "CLHEP/Units/SystemOfUnits.h"
using namespace CLHEP;

Detector::Detector(const G4GDMLParser &parser) : G4VUserDetectorConstruction(), fParser(parser), World(fParser.GetWorldVolume()) {}

Detector::~Detector() {}

void AddLArProps(G4Material *LAr)
{
  G4MaterialPropertiesTable *LAr_MPT = new G4MaterialPropertiesTable();

  // scintillation properties
  G4double scintillation_photon_energies[3] = {9.7 * eV, 9.76377 * eV, 9.8 * eV};
  G4double scintillation_fast_component[3] = {0.913594, 1, 0.971252};
  G4double scintillation_slow_component[3] = {0.913594, 1, 0.971252};

  LAr_MPT->AddProperty("FASTCOMPONENT", scintillation_photon_energies, scintillation_fast_component, 3);
  LAr_MPT->AddProperty("SLOWCOMPONENT", scintillation_photon_energies, scintillation_slow_component, 3);
  LAr_MPT->AddConstProperty("SCINTILLATIONYIELD", 3621.0 / MeV); // reduzido em de 10x - photons/energia
  LAr_MPT->AddConstProperty("RESOLUTIONSCALE", 1.);
  LAr_MPT->AddConstProperty("FASTTIMECONSTANT", 6. * ns);
  LAr_MPT->AddConstProperty("SLOWTIMECONSTANT", 1.8 * us);
  LAr_MPT->AddConstProperty("YIELDRATIO", 0.25);

  // refraction Index

  G4double RIndex_photon_energy[10] = {7.3367183961 * eV, 8.1468599034 * eV, 8.7942508537 * eV, 9.3735970883 * eV, 9.7402642614 * eV, 10.0346587111 * eV,
                                       10.2939327045 * eV, 10.401432965 * eV, 10.5669602239 * eV, 10.7726825649 * eV};
  G4double RIndex_LAr[10] = {1.28143463770843, 1.301258737209988, 1.3225062859789587, 1.345444112684444, 1.364484425660028, 1.3856545275590553, 1.4069737146827237,
                             1.4279201887447894, 1.4510731241315424, 1.4716468851320057};

  LAr_MPT->AddProperty("RINDEX", RIndex_photon_energy, RIndex_LAr, 10);

  // Absorption
  G4double abs_photon_energies[5] = {2.034 * eV, 2.341 * eV, 2.757 * eV, 3.353 * eV, 4.136 * eV};
  G4double abs_LAr[5] = {3448 * m, 4082 * m, 6329 * m, 9174 * m, 12346 * m};
  LAr_MPT->AddProperty("ABSLENGTH", abs_photon_energies, abs_LAr, 5);

  LAr->SetMaterialPropertiesTable(LAr_MPT);
  LAr->GetIonisation()->SetBirksConstant(0.0000001 * mm / MeV);
}

void WorldBuild(G4VPhysicalVolume *World)
{

  G4VPhysicalVolume *detEnclosure = World->GetLogicalVolume()->GetDaughter(0);

  // iterating over the enclosure's daughter volumes until the cryostat volume is found
  G4VPhysicalVolume *cryostat = nullptr;
  for (int i = 0; cryostat == nullptr; i++)
  {
    G4VPhysicalVolume *V = detEnclosure->GetLogicalVolume()->GetDaughter(i);

    if (!V)
    {
      G4cout << "ERROR ITERATING ON DAUGHTER VOLUME LIST" << G4endl;
      exit(-1);
    }

    G4String vol_name = V->GetName();
    if (vol_name.find("Cryostat") != std::string::npos)
    {
      cryostat = V;
    }
  }

  // iterating over the cryostat's daughter volumes until the TPC volume is found
  G4VPhysicalVolume *TPC = nullptr;
  for (int i = 0; TPC == nullptr; i++)
  {
    G4VPhysicalVolume *V = cryostat->GetLogicalVolume()->GetDaughter(i);

    if (!V)
    {
      G4cout << "ERROR ITERATING ON DAUGHTER VOLUME LIST" << G4endl;
      exit(-1);
    }

    G4String vol_name = V->GetName();
    if (vol_name.find("TPC") != std::string::npos)
    {
      TPC = V;

      // editing lar props
      G4Material *LAr = TPC->GetLogicalVolume()->GetMaterial();
      AddLArProps(LAr);
      G4cout << "Dump LAr table:\n" << G4endl;
      LAr->GetMaterialPropertiesTable()->DumpTable();
    }
  }
}

G4VPhysicalVolume *Detector::Construct()
{
  G4GeometryManager *geo = G4GeometryManager::GetInstance();

  geo->OpenGeometry(World);
  WorldBuild(World);
  geo->CloseGeometry(World);
  return World;
}

void Detector::ConstructSDandField()
{

  G4cout << "\n\nBUILDING SENSITIVE DETECTORS\n\n" << G4endl;
  //------------------------------------------------
  // Sensitive detectors
  //------------------------------------------------

  G4SDManager *SDman = G4SDManager::GetSDMpointer();

  G4String trackerChamberSDname = "Tracker";
  SensitiveDetector *aTrackerSD = new SensitiveDetector(trackerChamberSDname);
  SDman->AddNewDetector(aTrackerSD);

  ///////////////////////////////////////////////////////////////////////
  //
  // Example how to retrieve Auxiliary Information for sensitive detector
  //
  const G4GDMLAuxMapType *auxmap = fParser.GetAuxMap();
  G4cout << "Found " << auxmap->size()
         << " volume(s) with auxiliary information."
         << G4endl << G4endl;
  for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
       iter != auxmap->end(); iter++)
  {
    G4cout << "Volume " << ((*iter).first)->GetName()
           << " has the following list of auxiliary information: "
           << G4endl << G4endl;
    for (G4GDMLAuxListType::const_iterator vit = (*iter).second.begin();
         vit != (*iter).second.end(); vit++)
    {
      G4cout << "--> Type: " << (*vit).type
             << " Value: " << (*vit).value << G4endl;
    }
  }
  G4cout << G4endl;

  // The same as above, but now we are looking for
  // sensitive detectors setting them for the volumes

  for (G4GDMLAuxMapType::const_iterator iter = auxmap->begin();
       iter != auxmap->end(); iter++)
  {
    G4cout << "Volume " << ((*iter).first)->GetName()
           << " has the following list of auxiliary information: "
           << G4endl << G4endl;
    for (G4GDMLAuxListType::const_iterator vit = (*iter).second.begin();
         vit != (*iter).second.end(); vit++)
    {
      if ((*vit).type == "SensDet")
      {
        G4cout << "Attaching sensitive detector " << (*vit).value
               << " to volume " << ((*iter).first)->GetName()
               << G4endl << G4endl;

        G4VSensitiveDetector *mydet =
            SDman->FindSensitiveDetector((*vit).value);
        if (mydet)
        {
          G4LogicalVolume *myvol = (*iter).first;
          myvol->SetSensitiveDetector(mydet);
        }
        else
        {
          G4cout << (*vit).value << " detector not found" << G4endl;
        }
      }
    }
  }
}
