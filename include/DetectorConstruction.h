#pragma once
#include "G4VUserDetectorConstruction.hh"
#include "G4MaterialPropertiesTable.hh"

class G4GDMLParser;

class Detector : public G4VUserDetectorConstruction
{
  public:
    Detector(const G4GDMLParser& parser);

    ~Detector();

    virtual void ConstructSDandField();
    virtual G4VPhysicalVolume *Construct();

  private:
    const G4GDMLParser& fParser;
    G4VPhysicalVolume* World;
    G4MaterialPropertiesTable* LAr_MPT;
};
