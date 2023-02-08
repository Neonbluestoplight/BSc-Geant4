/// Primary generator action class (particle gun).
///
/// It defines a single particle which hits the calorimeter
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class

#ifndef B4PrimaryGeneratorAction_h
#define B4PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

namespace B4
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

  // set methods
  void SetRandomFlag(G4bool value);

private:
  G4ParticleGun* fParticleGun = nullptr; // G4 particle gun
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
