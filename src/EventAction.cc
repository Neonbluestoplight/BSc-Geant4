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

/*

void EventAction::PrintEventStatistics(G4double diodeEdep, G4double diodeTrackLength, G4ThreeVector diodePos) const
{
  // print event statistics

  G4cout
     << "   Diode: total energy: "
     << std::setw(7) << G4BestUnit(diodeEdep, "Energy")
     << "       total track length: "
     << std::setw(7) << G4BestUnit(diodeTrackLength, "Length")
     << "       event position: "
     << std::setw(7) << diodePos
     << G4endl;

}

*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // Get hits collections IDs (only once)
  if ( fDioHCID == -1 ) {
    fDioHCID = G4SDManager::GetSDMpointer()->GetCollectionID("DiodeHitsCollection");
  }

  // Get hits collections
  auto diodeHC = GetHitsCollection(fDioHCID, event);

  // Get hit with total values
  auto diodeHit = (*diodeHC)[diodeHC->entries()-1];

  // Print per event (modulo n)
  
  /*

  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();

  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;
    PrintEventStatistics(diodeHit->GetEdep(), diodeHit->GetTrackLength(), diodeHit->GetPosition());
  }
  
  */

  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, diodeHit->GetEdep());

  analysisManager->FillH1(1, diodeHit->GetTrackLength());

  analysisManager->FillH1(2, diodeHit->GetPosition()[0]);
  analysisManager->FillH1(3, diodeHit->GetPosition()[1]);
  analysisManager->FillH1(4, diodeHit->GetPosition()[2]);

  // fill ntuple
  analysisManager->FillNtupleDColumn(0, diodeHit->GetEdep());

  analysisManager->FillNtupleDColumn(1, diodeHit->GetTrackLength());

  analysisManager->FillNtupleDColumn(2, diodeHit->GetPosition()[0]);
  analysisManager->FillNtupleDColumn(3, diodeHit->GetPosition()[1]);
  analysisManager->FillNtupleDColumn(4, diodeHit->GetPosition()[2]);

  analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
