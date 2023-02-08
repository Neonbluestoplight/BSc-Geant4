/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Diode and Backing plate layers
/// stored in the hits collections.

/// \file EventAction.hh
/// \brief Definition of the B4c::EventAction class

#ifndef B4cEventAction_h
#define B4cEventAction_h 1

#include "G4UserEventAction.hh"
#include "CalorHit.hh"
#include "globals.hh"

namespace B4c
{
class EventAction : public G4UserEventAction
{
public:
  EventAction();
  ~EventAction() override;

  void  BeginOfEventAction(const G4Event* event) override;
  void  EndOfEventAction(const G4Event* event) override;

private:
  // methods
  CalorHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;
  void PrintEventStatistics(G4double diodeEdep, G4double diodeTrackLength, G4ThreeVector diodePos) const;

  // data members
  G4int fDioHCID = -1;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


