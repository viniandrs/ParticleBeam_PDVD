#include "SensitiveDetector.h"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name) {}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::Initialize(G4HCofThisEvent*) {}

G4bool SensitiveDetector::ProcessHits(G4Step*, G4TouchableHistory*)
{
  G4cout << "Processing hits ...." << G4endl; 
  return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {}
