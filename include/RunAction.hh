/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfRunAction() for the following
/// physics quantities:
/// - Edep in diode
/// - Edep in backing plate
/// - Track length in diode
/// - Track length in backing plate
///
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// according to a specified file extension.
///
/// In EndOfRunAction(), the accumulated statistic and computed
/// dispersion is printed.
///

/// \file RunAction.hh
/// \brief Definition of the B4::RunAction class

#ifndef B4RunAction_h
#define B4RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

namespace B4
{

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void   EndOfRunAction(const G4Run*) override;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

