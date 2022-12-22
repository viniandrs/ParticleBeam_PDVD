#include "SteppingAction.h"
#include "RunAction.h"
#include "EventInformation.h"

#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4PhysicalVolumeStore.hh"
#include "g4root.hh"

#include <unistd.h>

using namespace std;

SteppingAction::SteppingAction()
{
    G4PhysicalVolumeStore *pvols = G4PhysicalVolumeStore::GetInstance();
    G4String volume_name;
    G4int i = 0;

    // iterating on every volume in pv store
    for (G4VPhysicalVolume *vol : *pvols)
    {
        // assigning an integer for each arapuca
        volume_name = vol->GetName();
        if (volume_name.find("volOpDetSensitive_Arapuca") != string::npos)
        {
            // checking if is the key already registered
            if (arapuca_codes.count(volume_name) == 1)
                continue;

            arapuca_codes[volume_name] = i;
            i++;
        }
    }
}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4Track *track = aStep->GetTrack();
    G4String particle = track->GetDefinition()->GetParticleName(); // gamma, e+, e-, opticalphoton

    if (particle != "opticalphoton")
        return;

    auto current_vol = track->GetVolume();

    if (!current_vol)
        return;

    auto current_vol_name = current_vol->GetName();

    if (current_vol_name.find("volOpDetSensitive_Arapuca") != string::npos)
    {
        G4int hv_id = analysisManager->GetH1Id("hv");                    // get histogram int identifier, searched by histogram name
        analysisManager->FillH1(hv_id, arapuca_codes[current_vol_name]); // fill histogram with volume code values
        track->SetTrackStatus(fStopAndKill);
    }
}