#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VSolid.hh"
#include "G4ParticleGun.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event *anEvent) override;
private:
    G4ParticleGun *m_newGun;
};