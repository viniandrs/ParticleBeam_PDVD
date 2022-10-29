#pragma once

#include "G4VUserActionInitialization.hh"
#include "G4GDMLParser.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    ~ActionInitialization();
    
    virtual void BuildForMaster() const;
    virtual void Build() const;

private:
};