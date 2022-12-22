#pragma once

#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"
#include "G4String.hh"

#include <vector>
#include <map>

class SteppingAction : public G4UserSteppingAction
{

public:
    SteppingAction();
    ~SteppingAction();

    virtual void UserSteppingAction(const G4Step *);

private:
    std::map<G4String, G4int> arapuca_codes;
};