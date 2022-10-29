#include "../include/PrimaryGeneratorAction.h"
#include "../include/EventInformation.h"

#include "G4EventManager.hh"
#include "Randomize.hh"
#include "G4ParticleTable.hh"
#include "g4root.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Navigator.hh"

using namespace CLHEP;
using namespace std;

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction(), 
m_newGun(new G4ParticleGun)
{
    G4ThreeVector tpc_center(0, 0, 150 * cm);

    //the initial position of the particle
    m_newGun->SetParticlePosition(tpc_center);

    // defining the particle as an electron
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    auto electron = particleTable->FindParticle("e-");
    m_newGun->SetParticleDefinition(electron);

    //------------------- initial particle energy -------------------
    m_newGun->SetParticleEnergy(4 * GeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete m_newGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
    //------------------- Generating a 4 GeV electron at the middle of the volume -------------------
    
    //Momentum vector with a random direction
    G4double phi2 = 2 * pi * G4UniformRand();
    G4double teta2 = pi * G4UniformRand();

    G4double costeta2 = cos(teta2);
    G4double sinteta2 = sin(teta2);
    G4double cosphi2 = cos(phi2);
    G4double sinphi2 = sin(phi2);

    m_newGun->SetParticleMomentumDirection(G4ThreeVector(sinteta2 * cosphi2, costeta2, sinteta2 * sinphi2));

    //Generating the event
    m_newGun->GeneratePrimaryVertex(anEvent);
}