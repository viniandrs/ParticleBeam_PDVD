#include "DetectorConstruction.h"
#include "SensitiveDetector.h"

#include "G4SDManager.hh"
#include "G4GDMLParser.hh"

#include "G4MTRunManager.hh"
#include "G4GeometryManager.hh"

#include "CLHEP/Units/SystemOfUnits.h"
using namespace CLHEP;

Detector::Detector(const G4GDMLParser &parser) : G4VUserDetectorConstruction(), fParser(parser), World(fParser.GetWorldVolume())
{
  // creating the LAr MPT
  LAr_MPT = new G4MaterialPropertiesTable();

  // scintillation properties
  G4double scintillation_photon_energies[3] = {9.7 * eV, 9.76377 * eV, 9.8 * eV};
  G4double scintillation_fast_component[3] = {0.913594, 1, 0.971252};
  G4double scintillation_slow_component[3] = {0.913594, 1, 0.971252};

  LAr_MPT->AddProperty("FASTCOMPONENT", scintillation_photon_energies, scintillation_fast_component, 3);
  LAr_MPT->AddProperty("SLOWCOMPONENT", scintillation_photon_energies, scintillation_slow_component, 3);
  LAr_MPT->AddConstProperty("SCINTILLATIONYIELD", /*3621.0*/ 36 / MeV); // reduzido em 10x - photons/energia
  LAr_MPT->AddConstProperty("RESOLUTIONSCALE", 1.);
  LAr_MPT->AddConstProperty("FASTTIMECONSTANT", 6. * ns);
  LAr_MPT->AddConstProperty("SLOWTIMECONSTANT", 1.8 * us);
  LAr_MPT->AddConstProperty("YIELDRATIO", 0.25);

  // refraction Index
  G4double RIndex_photon_energy[72] = {1.88901692613609*eV, 1.915491549763*eV, 1.94434453495496*eV, 1.9740800365654*eV, 2.00473917327901*eV,
                                        2.03636565851099*eV, 2.0690060083582*eV, 2.1027097698752*eV, 2.1375297720313*eV, 2.17352240202191*eV,
                                        2.21074790997382*eV, 2.24927074550798*eV, 2.28915993011439*eV, 2.33048946986518*eV, 2.37333881365753*eV,
                                        2.41779336295621*eV, 2.46394503991691*eV, 2.51189292184303*eV, 2.56174395119099*eV, 2.61361373183203*eV,
                                        2.66762742404865*eV, 2.72392075285046*eV, 2.78264114670945*eV, 2.84394902682875*eV, 2.90801927068415*eV,
                                        2.97504287795441*eV, 3.04522887226339*eV, 3.11880647861692*eV, 3.19602762431763*eV, 3.27716982084588*eV,
                                        3.36253949617556*eV, 3.45247586185779*eV, 3.54735541774646*eV, 3.64759722049586*eV, 3.75366907130329*eV,
                                        3.86609481562225*eV, 3.98546299517591*eV, 4.11243715385428*eV, 4.24776817847434*eV, 4.39230915909235*eV,
                                        4.54703339014745*eV, 4.71305631519082*eV, 4.89166246132417*eV, 5.08433873911506*eV, 5.29281593505014*eV,
                                        5.51912084854449*eV, 5.76564240171605*eV, 6.03521629502331*eV, 6.33123457629323*eV, 6.65778911807759*eV,
                                        7.01986191167459*eV, 7.42358102512033*eV, 7.80768693883725*eV, 8.18166617653237*eV, 8.49578904787588*eV,
                                        8.75179837892604*eV, 8.97145190403002*eV, 9.21112941189563*eV, 9.40734062594689*eV, 9.56190983757659*eV,
                                        9.71915769371575*eV, 9.88023992289941*eV, 9.99798584772925*eV, 10.0930619775564*eV, 10.1644798618311*eV,
                                        10.2587258039568*eV, 10.3321720936895*eV, 10.3923239488175*eV, 10.4560982471211*eV, 10.4827908882082*eV,
                                        10.5288181909777*eV, 10.5783779318939*eV};
  G4double RIndex_LAr[72] = {1.2310665394518083, 1.2311200318147684, 1.2312727482978456, 1.2313496348295065, 1.2313506914097514, 1.2314023012909403, 
                              1.2321538166889967, 1.231909947460871, 1.2321384938953646, 1.2323923169803301, 1.232443926861519, 1.2328668458034384,
                              1.233095392237932, 1.233053736213583, 1.233509772502325, 1.2335361057330418, 1.2334866090123424, 1.2338668153496684,
                              1.2343481282888824, 1.2344502914710154, 1.2346282846045646, 1.2350337675923626, 1.2354898038811046, 1.2358874461725522,
                              1.2361238333033957, 1.2364787629902494, 1.2371038951359457, 1.2374666655191495, 1.237720488604115, 1.2384292913975776,
                              1.2388853276863196, 1.2393413639750617, 1.2402271033218288, 1.2409106294648191, 1.2417458155106422, 1.2425810015564651,
                              1.2435678475051206, 1.2446558000556642, 1.245844859208096, 1.2472614082147766, 1.2488296171242892, 1.2505242092861624,
                              1.2525221212537003, 1.2548739063278473, 1.257579564508603, 1.2607654790483278, 1.2644822032479663, 1.269108886864599,
                              1.2747719131505861, 1.2816987719601767, 1.2910690403154002, 1.302368759656658, 1.316713517166515, 1.3329411664124329,
                              1.3493804523854165, 1.365926455672306, 1.3820018039345536, 1.4010549636291998, 1.423580491960391, 1.442600693158671,
                              1.46103487273757, 1.4894895758980782, 1.508978836972031, 1.5300372520419365, 1.5496431051408859, 1.5737334642006038,
                              1.5996151986767673, 1.6187496758472137, 1.6361783106957755, 1.6540241603412935, 1.672490509546729, 1.69156768392026};
  LAr_MPT->AddProperty("RINDEX", RIndex_photon_energy, RIndex_LAr, 72);

  // absorption
  G4double abs_photon_energies[6] = { 2.0*eV, 2.341*eV, 2.757*eV, 3.353*eV, 4.136*eV, 10.0*eV };
  G4double abs_LAr[6] = {20*m,  20*m,  20*m,  20*m, 20*m, 20*m };
  LAr_MPT->AddProperty("ABSLENGTH", abs_photon_energies, abs_LAr, 6);  

  // rayleigh scattering
  G4double ray_e_lAr[21] = { 1.18626*eV, 1.68626*eV, 2.18626*eV, 2.68626*eV, 3.18626*eV, 3.68626*eV, 4.18626*eV, 4.68626*eV, 5.18626*eV, 5.68626*eV, 6.18626*eV,
                            6.68626*eV, 7.18626*eV, 7.68626*eV, 8.18626*eV, 8.68626*eV, 9.18626*eV, 9.68626*eV, 10.1863*eV, 10.6863*eV, 11.1863*eV};
  G4double ray_s_lAr[21] = { 1200800*cm, 390747*cm, 128633*cm, 54969.1*cm, 27191.8*cm, 14853.7*cm, 8716.9*cm, 5397.42*cm, 3481.37*cm, 2316.51*cm, 1577.63*cm,
                              1092.02*cm, 763.045*cm, 534.232*cm, 371.335*cm, 252.942*cm, 165.38*cm, 99.9003*cm, 51.2653*cm, 17.495*cm, 0.964341*cm };
  LAr_MPT->AddProperty("RAYLEIGH", ray_e_lAr, ray_s_lAr, 21);
}

Detector::~Detector()
{
  delete LAr_MPT;
  delete World;
}

G4VPhysicalVolume *FindVolume(G4VPhysicalVolume *pMother, G4String target)
{
  // iterating over the enclosure's daughter volumes until the cryostat volume is found
  G4VPhysicalVolume *volTarget = nullptr;
  for (int i = 0; volTarget == nullptr; i++)
  {
    G4VPhysicalVolume *pDaughter = pMother->GetLogicalVolume()->GetDaughter(i);

    // if (!pDaughter)
    // {
    //   G4cout << "ERROR ITERATING ON DAUGHTER VOLUME LIST" << G4endl;
    //   break;
    // }

    G4String vol_name = pDaughter->GetName();
    if (vol_name.find(target) != std::string::npos)
    {
      return pDaughter;
    }
  }
  return nullptr;
}

G4VPhysicalVolume *Detector::Construct()
{
  // Getting the LAr object
  auto detEnclosure = World->GetLogicalVolume()->GetDaughter(0);
  auto cryostat = FindVolume(detEnclosure, "Cryostat");
  auto TPC = FindVolume(cryostat, "TPC");

  // Set the MPT
  G4Material *LAr = TPC->GetLogicalVolume()->GetMaterial();
  LAr->SetMaterialPropertiesTable(LAr_MPT);
  LAr->GetIonisation()->SetBirksConstant(0.0000001 * mm / MeV);

  // G4cout << "Dump LAr table:\n"
  //        << G4endl;
  // LAr->GetMaterialPropertiesTable()->DumpTable();
  return World;
}

void Detector::ConstructSDandField()
{

  G4cout << "\n\nBUILDING SENSITIVE DETECTORS\n\n"
         << G4endl;
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
