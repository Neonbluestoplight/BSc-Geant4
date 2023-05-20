/// \file EventAction.cc
/// \brief Implementation of the B4c::EventAction class

#include "EventAction.hh"
#include "CalorimeterSD.hh"
#include "CalorHit.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

namespace B4c
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
{}

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHitsCollection*
EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
  auto hitsCollection = static_cast<CalorHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("EventAction::GetHitsCollection()", "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // Get hits collections IDs (only once)
  if ( fDioHCID == -1 ) {
    fDioHCID = G4SDManager::GetSDMpointer()->GetCollectionID("DiodeHitsCollection");
    fAnnHCID = G4SDManager::GetSDMpointer()->GetCollectionID("AnnularHitsCollection");
  }

  // Get hits collections
  auto diodeHC   = GetHitsCollection(fDioHCID, event);
  auto annularHC = GetHitsCollection(fAnnHCID, event);

  // Get hit with total values
  auto diodeHit   = (*diodeHC)[diodeHC->entries()-1];
  auto annularHit = (*annularHC)[annularHC->entries()-1];

  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, diodeHit->GetEdep());
  analysisManager->FillH1(1, annularHit->GetEdep());

  analysisManager->FillH1(2, diodeHit->GetTrackLength());
  analysisManager->FillH1(3, annularHit->GetTrackLength());

  // fill ntuple
  analysisManager->FillNtupleDColumn(0, diodeHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, annularHit->GetEdep());

  analysisManager->FillNtupleDColumn(2, diodeHit->GetTrackLength());
  analysisManager->FillNtupleDColumn(3, annularHit->GetTrackLength());

  analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
