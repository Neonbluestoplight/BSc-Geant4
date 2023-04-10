/// \file DetectorConstruction.cc
/// \brief Implementation of the B4c::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "CalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

namespace B4c
{
G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  DefineMaterials();      // Define materials
  return DefineVolumes(); // Define volumes
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Silicon material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Si");
  nistManager->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

  G4double a;       // mass of a mole
  G4double z;       // z = mean number of protons
  G4double density; // density

  // Vacuum
  new G4Material("Galactic",                     // Name
                 z=1.,                           // Mean number of protons
                 a=1.01*g/mole,                  // Mass of a mole
                 density= universe_mean_density, // density
                 kStateGas,                      // State of material - Gaseous
                 2.73*kelvin,                    // Temperature
                 3.e-18*pascal);                 // Pressure

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  fNofLayers = 1;

  G4double diodeThickness   = 0.3  *mm;
  G4double backingThickness = 1.34 *mm;
  auto detectorThickness    = diodeThickness + backingThickness;

  //-----------------------------------------------------------------------------------------
  // THE WORLD (Position is (0,0,0) and cannot be changed)
  //-----------------------------------------------------------------------------------------
  G4ThreeVector worldSize = G4ThreeVector((150 *mm)/2,             //World Size X
                                          (150 *mm)/2,             //World Size Y
                                          (150 *mm)/2);            //World Size Z

  //-----------------------------------------------------------------------------------------
  // THE DIODE
  //-----------------------------------------------------------------------------------------
  
  G4ThreeVector diodeSize  = G4ThreeVector((18 *mm)/2,             //X Size - diode
                                           (18 *mm)/2,             //Y Size - diode
                                           (0.3 *mm)/2);           //Z Size - diode

  G4ThreeVector diodePlace = G4ThreeVector((0. *mm),               //X Position - diode
                                           (0. *mm),               //Y Position - diode
                                           -backingThickness/2);   //Z Position - diode

  //-----------------------------------------------------------------------------------------
  // THE BACKING
  //-----------------------------------------------------------------------------------------
  
  G4ThreeVector backSize   = G4ThreeVector((25.5 *mm)/2,           //X Size - backing plate
                                           (25.5 *mm)/2,           //Y Size - backing plate
                                           (backingThickness)/2);  //Z Size - backing plate

  G4ThreeVector backPlace  = G4ThreeVector((0. *mm),               //X Position -backing plate                
                                           (0. *mm),               //Y Position -backing plate
                                           (diodeThickness/2));    //Z Position -backing plate

  //-----------------------------------------------------------------------------------------
  // THE DETECTOR
  //-----------------------------------------------------------------------------------------
  G4ThreeVector detectSize = G4ThreeVector((25.5 *mm)/2,            //X Size - detector
                                           (25.5 *mm)/2,            //Y Size - detector
                                           (detectorThickness)/2);  //Z Size - detector           
                 
  G4double detectZoffset = detectorThickness/2;  // Z offset - detector
  G4double detectZdist   = 0 *mm;                // Z distance - detector  
  G4double TheOffset     = 2*detectSize[0];      // Offsetting the detector to make detector sized hole 

  G4double angle      = 0. *deg;                                     // Angle of rotation
  G4double rotXoffset = -std::sin(angle)*detectSize[0];              // X-offset due to rotation
  G4double rotYoffset = detectSize[1]-std::cos(angle)*detectSize[1]; // Y-offset due to rotation

  G4double aperture   = 8 *mm;                   // The aperture of the hole in the middle of the array
  G4double apOffset   = (aperture - TheOffset)/2; // Offsetting the detector by half the aperture distance

  // UPPER DETECTOR
  G4ThreeVector detect1Place = G4ThreeVector((0. *mm),                                                      //X position - detector
                                             (TheOffset+apOffset-rotYoffset+std::sin(angle)*detectZoffset), //Y position - detector
                                             (detectZdist+std::cos(angle)*detectZoffset-rotXoffset));       //Z position - detector

  // LOWER DETECTOR
  G4ThreeVector detect2Place = G4ThreeVector((0. *mm),                                                       //X position - detector
                                            -(TheOffset+apOffset-rotYoffset+std::sin(angle)*detectZoffset),  //Y position - detector
                                             (detectZdist+std::cos(angle)*detectZoffset-rotXoffset));        //Z position - detector

  // RIGHT DETECTOR
  G4ThreeVector detect3Place = G4ThreeVector((TheOffset-rotYoffset+std::sin(angle)*detectZoffset),           //X position - detector
                                             (0. *mm),                                                       //Y position - detector
                                             (detectZdist+std::cos(angle)*detectZoffset-rotXoffset));        //Z position - detector

  // LEFT DETECTOR
  G4ThreeVector detect4Place = G4ThreeVector(-(TheOffset-rotYoffset+std::sin(angle)*detectZoffset),          //X position - detector
                                             (0. *mm),                                                       //Y position - detector
                                             (detectZdist+std::cos(angle)*detectZoffset-rotXoffset));        //Z position - detector
  //-----------------------------------------------------------------------------------------

  
  // Rotation parameters of the detector

  G4RotationMatrix* detectRot1 = new G4RotationMatrix();
  G4RotationMatrix* detectRot2 = new G4RotationMatrix();
  G4RotationMatrix* detectRot3 = new G4RotationMatrix();
  G4RotationMatrix* detectRot4 = new G4RotationMatrix();

  detectRot1->rotateX(angle);         // rotate upper detector along X-axis by angle 
  detectRot2->rotateX(-angle);        // rotate lower detector along X-axis by angle in opposite direction
  detectRot3->rotateY(-angle);        // rotate right detector along Y-axis by angle in opposite direction
  detectRot4->rotateY(angle);         // rotate left detector along Y-axis by angle 

 
  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto diodeMaterial   = G4Material::GetMaterial("G4_Si");
  auto backMaterial    = G4Material::GetMaterial("G4_ALUMINUM_OXIDE");


  if (! defaultMaterial|| ! diodeMaterial|| ! backMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()", "MyCode0001", FatalException, msg);
  }

  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths
  //
  // G4LogicalVolume(<solid name>,<material>,<title>)
  //
  // G4PVPlacement(<rotation matrix>,<position vector>,<logical volume>,
  //               <title>,<mother volume>,<boolean>,<copy number>,<check overlaps>)
  //
  // G4VReplica(<title>,<logical volume>,<mother volume>,
  //            <axis of replication>,<number of replicas>,<replica width>)

  //
  // World
  //
  auto worldS  = new G4Box("World", worldSize[0], worldSize[1], worldSize[2]); 
  auto worldLV = new G4LogicalVolume(worldS, defaultMaterial, "World");         
  auto worldPV = new G4PVPlacement(0, G4ThreeVector(), worldLV, "World", 0, false, 0, fCheckOverlaps);  

  //
  // Detector
  //
  auto detectorS  = new G4Box("Detector", detectSize[0], detectSize[1], detectSize[2]);
  auto detectLV   = new G4LogicalVolume(detectorS, defaultMaterial, "Detector");

  new G4PVPlacement(detectRot1, detect1Place, detectLV, "Detector", worldLV, false, 0, fCheckOverlaps); // upper detector placement
  new G4PVPlacement(detectRot2, detect2Place, detectLV, "Detector", worldLV, false, 0, fCheckOverlaps); // lower detector placement 
  new G4PVPlacement(detectRot3, detect3Place, detectLV, "Detector", worldLV, false, 0, fCheckOverlaps); // right detector placement
  new G4PVPlacement(detectRot4, detect4Place, detectLV, "Detector", worldLV, false, 0, fCheckOverlaps); // left detector placement

  //
  // Diode
  // 
  auto diodeS  = new G4Box("Diode", diodeSize[0], diodeSize[1], diodeSize[2]);
  auto diodeLV = new G4LogicalVolume(diodeS, diodeMaterial, "diodeLV");   

  new G4PVPlacement(0, diodePlace, diodeLV, "Diode", detectLV, false, 0, fCheckOverlaps); 

  //
  // Backing
  //
  auto backS  = new G4Box("Backing", backSize[0], backSize[1], backSize[2]); 
  auto backLV = new G4LogicalVolume(backS, backMaterial, "backLV");     

  new G4PVPlacement(0, backPlace, backLV, "Backing", detectLV, false, 0, fCheckOverlaps);  

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The detector is " << fNofLayers << " layers of: [ "
    << diodeThickness/mm << "mm of " << diodeMaterial->GetName()
    << " + "
    << backingThickness/mm << "mm of " << backMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  auto whiteBoxVisAtt   = new G4VisAttributes(G4Colour(1.0,1.0,1.0));     //rgb = (255,255,255)
  auto ceramicBoxVisAtt = new G4VisAttributes(G4Colour(0.88,0.87,0.82));  //rgb = (119,195,236)
  auto siliconBoxVisAtt = new G4VisAttributes(G4Colour(0.58,0.60,0.64));  //rgb = (149,153,165)

  whiteBoxVisAtt->SetVisibility(true);
  ceramicBoxVisAtt->SetVisibility(true);
  siliconBoxVisAtt->SetVisibility(true);

  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());   //world         : Invisible
  detectLV->SetVisAttributes(G4VisAttributes::GetInvisible());  //full detector : Invisible
  diodeLV->SetVisAttributes(siliconBoxVisAtt);                  //diode         : Silicon colour
  backLV->SetVisAttributes(ceramicBoxVisAtt);                   //backing plate : Ceramic colour

  return worldPV; // Always return the physical World
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //
  auto diodeSD = new CalorimeterSD("diodeSD", "DiodeHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(diodeSD);
  SetSensitiveDetector("diodeLV",diodeSD);

  //
  // Magnetic field - Create global magnetic field messenger.
  //

  G4ThreeVector fieldValue;   // Uniform magnetic field created if the field value is not zero.
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  G4AutoDelete::Register(fMagFieldMessenger); // Register the field messenger for deleting
}

}
