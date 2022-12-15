#pragma once

#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"
#include "G4String.hh"

#include <vector>

class SteppingAction : public G4UserSteppingAction
{

public:
    SteppingAction();
    ~SteppingAction();

    virtual void UserSteppingAction(const G4Step *);

private:
    std::pair<int, int> VolumeCode(std::string name);
    std::vector<G4String> vname;
};