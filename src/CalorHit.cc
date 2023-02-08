/// \file CalorHit.cc
/// \brief Implementation of the B4c::CalorHit class

#include "CalorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace B4c
{

G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHit::CalorHit()
{}

CalorHit::~CalorHit() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CalorHit::operator==(const CalorHit& right) const
{
  return ( this == &right ) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorHit::Print()
{
  G4cout
     << "Edep: "
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " track length: "
     << std::setw(7) << G4BestUnit( fTrackLength,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
