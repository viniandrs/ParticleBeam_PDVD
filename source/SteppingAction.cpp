#include "SteppingAction.h"
#include "RunAction.h"
#include "EventInformation.h"

#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4PhysicalVolumeStore.hh"
#include "g4root.hh"

#include <unistd.h>

using namespace std;

pair<int, int> SteppingAction::VolumeCode(string name)
{
    G4String wname = "World";

    if (name == "OutOfWorld")
    {
        pair<int, int> aux(-1, -1);
        return aux;
    }
    else if (name == wname.c_str())
    {
        pair<int, int> aux(0, -1);
        return aux;
    }

    // vname is an array of strings containing the names of the volumes
    for (int i = 1; i < (int)vname.size(); i++)
    {
        if (name == vname[i])
        {
            pair<int, int> aux(i, 0);
            return aux;
        }
    }

    pair<int, int> aux(-3, -4);
    return aux;
}

SteppingAction::SteppingAction()
{
    G4String avname = "";
    G4PhysicalVolumeStore *pvols = G4PhysicalVolumeStore::GetInstance();
    G4int n_vols = (int)pvols->size();

    for (int i = 0; i < n_vols; i++)
    {
        avname = ((*pvols)[i])->GetName();
        if (!(find(vname.begin(), vname.end(), avname) != vname.end()))
        {
            vname.push_back(avname);
        }
    }
}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4Track *track = aStep->GetTrack();
    G4String particle = track->GetDefinition()->GetParticleName(); // gamma, e+, e-, opticalphoton
    
    auto next_vol = aStep->GetTrack()->GetNextVolume();
    if (next_vol != nullptr)
    {
        auto next_vol_name = next_vol->GetName(); 
        pair<int, int> aux = VolumeCode(next_vol_name);
        G4int hv_id = analysisManager->GetH1Id("hv"); // get histogram int identifier, searched by histogram name
        analysisManager->FillH1(hv_id, aux.first);    // fill histogram with volume code values
    }
    else
    {
        pair<int, int> aux = VolumeCode("OutOfWorld");
        G4int hv_id = analysisManager->GetH1Id("hv");
        analysisManager->FillH1(hv_id, aux.first);
        G4cout << "Going out the world!" << G4endl;
    }
}