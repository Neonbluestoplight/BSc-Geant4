/// \file RunAction.cc
/// \brief Implementation of the B4::RunAction class

#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager

  // The choice of the output format is done via the specified file extension.
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true); // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple

  // Creating histograms
  analysisManager->CreateH1("Ediode","Edep in diode", 1000, 0., 10*MeV);

  analysisManager->CreateH1("Ldiode","trackL in diode", 1000, 0., 1*mm);

  analysisManager->CreateH1("diodeX","diode X position", 1000, -10.*mm, 10*mm);
  analysisManager->CreateH1("diodeY","diode Y position", 1000, -10.*mm, 10*mm);
  analysisManager->CreateH1("diodeZ","diode Z position", 1000, -10.*mm, 10*mm);

  // Creating ntuple
  analysisManager->CreateNtuple("B4", "Edep and TrackL");

  analysisManager->CreateNtupleDColumn("Ediode");

  analysisManager->CreateNtupleDColumn("Ldiode");

  analysisManager->CreateNtupleDColumn("diodeX");
  analysisManager->CreateNtupleDColumn("diodeY");
  analysisManager->CreateNtupleDColumn("diodeZ");

  analysisManager->FinishNtuple();
}

RunAction::~RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open output file of different types

  G4String fileName = "B4.root";          // Root
  // G4String fileName = "B4.csv";        // CSV file
  // G4String fileName = "B4.hdf5";       // HDF5 file
  // G4String fileName = "B4.xml";        // XML file

  analysisManager->OpenFile(fileName);
  G4cout << "Using " << analysisManager->GetType() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl;
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl;
    }

    G4cout << " EDiode : mean = "
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;

    G4cout << " LDiode : mean = "
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;
  }

  // save histograms & ntuple
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
