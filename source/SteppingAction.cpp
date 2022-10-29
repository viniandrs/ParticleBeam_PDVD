#include "../include/SteppingAction.h"
#include "../include/RunAction.h"
#include "../include/EventInformation.h"

#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "g4root.hh"

#include <unistd.h>

SteppingAction::SteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4Track *track = aStep->GetTrack();
    G4String particle = track->GetDefinition()->GetParticleName();

    //PreStep volume info
    G4StepPoint *aPrePoint = aStep->GetPreStepPoint();
    G4VPhysicalVolume *aPrePV = aPrePoint->GetPhysicalVolume();
    G4String PreVolName = "";
    if (aPrePV)
        PreVolName = aPrePV->GetName();

    //PostStep volume info
    G4StepPoint *aPostPoint = aStep->GetPostStepPoint();
    G4VPhysicalVolume *aPostPV = aPostPoint->GetPhysicalVolume();
    G4String PostVolName = "";
    if (aPostPV)
        PostVolName = aPostPV->GetName();

    //Alpha spectrum
    if (particle == "alpha")
    {
        //Registering the particle starting position
        G4VUserEventInformation *eventInformationBaseClass = G4EventManager::GetEventManager()->GetUserInformation();
        EventInformation *eventInformation = static_cast<EventInformation *>(eventInformationBaseClass);

        //eventInformation->x0 = x0;
        //eventInformation->y0 = y0;
        //eventInformation->z0 = z0;

        if (PreVolName == "aluminumAlphaSource" && PostVolName == "Argon")
        {
            /* Get the kinetic energy an alpha particle has when it leaves the aluminum and
            register it on the spectrum */
            G4double kinEnergy = track->GetKineticEnergy();
            analysisManager->FillH1(1, kinEnergy);

            G4VUserEventInformation *eventInformationBaseClass = G4EventManager::GetEventManager()->GetUserInformation();
            EventInformation *eventInformation = static_cast<EventInformation *>(eventInformationBaseClass);

            eventInformation->AlphaIsValid();
        }
    }

    //Counting the photons that entered the Arapuca and registering its alpha's energy
    if (particle == "opticalphoton")
    {
        //Checking if the particle passed from the Argon to one of the detector screens
        if ((PreVolName == "Argon" && PostVolName == "detector2") ||
            (PreVolName == "Argon" && PostVolName == "detector1"))
        {
            G4VUserEventInformation *eventInformationBaseClass = G4EventManager::GetEventManager()->GetUserInformation();
            EventInformation *eventInformation = static_cast<EventInformation *>(eventInformationBaseClass);

            eventInformation->IncreasePhotonsDetected();
        }
    }
}