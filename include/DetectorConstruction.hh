/// Detector construction class
///
/// A detector is a box made of a given number of layers. A layer consists
/// of an detector of silicon and of a ceramic backplate. The layer is replicated.
///
/// Four parameters define the geometry of the detector :
///
/// - the thickness of the silicon detector
/// - the thickness of the backing plate
/// - the number of layers
/// - the transverse size of the detector (the input face is a square).
///
/// In ConstructSDandField() sensitive detectors of DetectorSD type are 
/// created and associated with the Diode and Backing plate volumes. In addition a 
/// transverse uniform magnetic field is defined via G4GlobalMagFieldMessenger class.
 

/// \file DetectorConstruction.hh
/// \brief Definition of the B4c::DetectorConstruction class

#ifndef B4cDetectorConstruction_h
#define B4cDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

namespace B4c
{

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; // magnetic field messenger

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
    G4int  fNofLayers = -1;       // number of layers
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

