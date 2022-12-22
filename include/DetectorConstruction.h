#pragma once
#include "G4VUserDetectorConstruction.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4VPhysicalVolume.hh"

#include <vector>

class G4GDMLParser;

class Detector : public G4VUserDetectorConstruction
{
  public:
    Detector(const G4GDMLParser& parser);
    ~Detector();

    virtual G4VPhysicalVolume *Construct();

  private:
    const G4GDMLParser& fParser;
    G4VPhysicalVolume* World;

    G4MaterialPropertiesTable* LAr_MPT;
    G4MaterialPropertiesTable* surface_lar_al_MPT;
    G4MaterialPropertiesTable* surface_lar_steel_MPT;
};
