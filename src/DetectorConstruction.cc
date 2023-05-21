/// \file DetectorConstruction.cc
/// \brief Implementation of the B4c::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "CalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
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
  nistManager->FindOrBuildMaterial("G4_Al");

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
  //-----------------------------------------------------------------------------------------
  // THE WORLD (Position is (0,0,0) and cannot be changed)
  //-----------------------------------------------------------------------------------------
  G4ThreeVector worldSize = G4ThreeVector((150 *mm)/2,             //World Size X
                                          (150 *mm)/2,             //World Size Y
                                          (150 *mm)/2);            //World Size Z

  //-----------------------------------------------------------------------------------------
  // THE HAMAMATSU S3204-09 DETECTOR
  //-----------------------------------------------------------------------------------------
  G4double diodeThickness   = 0.3  *mm;
  G4double backingThickness = 1.34 *mm;
  G4double AlringThickness  = 1 *um;
  G4double extrusionThickness = 1.2 *mm;
  auto detectorThickness    = extrusionThickness + backingThickness;

  //
  // THE ENTIRE "CHIP" (Includes Al ring, Si Buffer and Al light shield)
  //
  
  // The diode
  G4ThreeVector diodeSize    = G4ThreeVector((18 *mm)/2,                  //X Size - diode
                                             (18 *mm)/2,                  //Y Size - diode
                                             (diodeThickness)/2);         //Z Size - diode

  G4ThreeVector diodePlace = G4ThreeVector((0. *mm),                              //X Position - diode
                                           (0. *mm),                              //Y Position - diode
                                           (-backingThickness+diodeThickness)/2); //Z Position - diode
  
  // The aluminium ring
  G4double AlringHoleLength = (18 *mm)/2-0.1*mm;
  G4double AlringWidth      = 0.1 *mm; 
  G4double AlringLength     = AlringHoleLength+AlringWidth;

  G4ThreeVector AlringSize     = G4ThreeVector(AlringLength,           //X Size - Al ring
                                               AlringLength,           //Y Size - Al ring
                                               (AlringThickness)/2);   //Z Size - Al ring
  
  G4ThreeVector AlringHoleSize = G4ThreeVector(AlringHoleLength,       //X Size - Al ring hole
                                               AlringHoleLength,       //Y Size - Al ring hole
                                               AlringSize[2]);         //Z Size - Al ring hole
  
  G4ThreeVector AlringPlace = G4ThreeVector((0. *mm),               //X,Y,Z Position - extrusion
                                            (0. *mm),             
                                            ((extrusionThickness-AlringThickness)/2));

  // The Silicon buffer
  G4double SiBuffWidth = 0.35 *mm;
  G4ThreeVector SiBuffSize   = G4ThreeVector(diodeSize[0]+SiBuffWidth,   //X Size - Si buffer
                                             diodeSize[1]+SiBuffWidth,   //Y Size - Si buffer
                                             diodeSize[2]);              //Z Size - Si buffer

  // The Aluminium light shield
  G4double AlshieldWidth = 0.65 *mm;
  G4ThreeVector AlShieldSize = G4ThreeVector(SiBuffSize[0]+AlshieldWidth, //X Size - Al light shield
                                             SiBuffSize[1]+AlshieldWidth, //Y Size - Al light shield
                                             diodeSize[2]);               //Z Size - Al light shield

  G4ThreeVector relPosition  = G4ThreeVector(0. *mm, 0. *mm, 0. *mm);     // Local position (0,0,0)

  //
  // THE BACKING
  //
  G4ThreeVector backSize   = G4ThreeVector((25.5 *mm)/2,           //X Size - backing plate
                                           (25.5 *mm)/2,           //Y Size - backing plate
                                           (backingThickness)/2);  //Z Size - backing plate

  G4ThreeVector backPlace  = G4ThreeVector((0. *mm),               //X Position -backing plate                
                                           (0. *mm),               //Y Position -backing plate
                                           (extrusionThickness/2));//Z Position -backing plate

  // THE CERAMIC EXTRUSION
  
  G4double extrusionWidth      = 1.2 *mm; 
  G4double extrusionLength     = (25.5 *mm)/2;
  G4double extrusionHoleLength = extrusionLength-extrusionWidth;

  G4ThreeVector extrusionSize     = G4ThreeVector(extrusionLength,        //X Size - extrusion
                                                  extrusionLength,        //Y Size - extrusion
                                                 (extrusionThickness)/2); //Z Size - extrusion
  
  G4ThreeVector extrusionHoleSize = G4ThreeVector(extrusionHoleLength,    //X Size - extrusion hole
                                                  extrusionHoleLength,    //Y Size - extrusion hole
                                                  extrusionSize[2]);      //Z Size - extrusion hole
  
  G4ThreeVector extrusionPlace    = G4ThreeVector((0. *mm),               //X Position - extrusion
                                                  (0. *mm),               //Y Position - extrusion
                                                  (-backingThickness)/2); //Z Position - extrusion

  //
  // THE DETECTOR
  //
  G4ThreeVector detectSize = G4ThreeVector((25.5 *mm)/2,            //X Size - detector
                                           (25.5 *mm)/2,            //Y Size - detector
                                           (detectorThickness)/2);  //Z Size - detector           
                 
  G4double detectZoffset  = detectorThickness/2;   // Z offset - detector
  G4double detectZdist    = 0 *mm;                 // Z distance - detector  
  G4double detectorOffset = 2*detectSize[0];       // Offsetting the detector to make detector sized hole 

  G4double angle      = 90. *deg;                                     // Angle of rotation
  G4double rotXoffset = -std::sin(angle)*detectSize[0];              // X-offset due to rotation
  G4double rotYoffset = detectSize[1]-std::cos(angle)*detectSize[1]; // Y-offset due to rotation

  G4double aperture = 2*detectSize[0];                         // The aperture of the hole in the middle of the array
  G4double apOffset = (aperture-detectorOffset)/2;    // Offsetting the detector by half the aperture distance

  G4double primaryOffset = detectorOffset+apOffset-rotYoffset+std::sin(angle)*detectZoffset; // The main placement vector with the offset
  G4double Zoffset       = detectZdist+std::cos(angle)*detectZoffset-rotXoffset;             // The Z-direction placement vector

  // UPPER DETECTOR
  G4ThreeVector detect1Place = G4ThreeVector(-(apOffset),      //X position - detector placement
                                             (primaryOffset),  //Y position - detector placement
                                             (Zoffset));       //Z position - detector placement

  // LOWER DETECTOR
  G4ThreeVector detect2Place = G4ThreeVector((apOffset),       //X position - detector placement
                                             -(primaryOffset), //Y position - detector placement
                                             (Zoffset));       //Z position - detector placement

  // RIGHT DETECTOR
  G4ThreeVector detect3Place = G4ThreeVector((primaryOffset),  //X position - detector placement 
                                             (apOffset),       //Y position - detector placement
                                             (Zoffset));       //Z position - detector placement

  // LEFT DETECTOR
  G4ThreeVector detect4Place = G4ThreeVector(-(primaryOffset), //X position - detector placement
                                             -(apOffset),      //Y position - detector placement
                                             (Zoffset));       //Z position - detector placement
  //-----------------------------------------------------------------------------------------
  
  // Rotation parameters of the detector
  G4RotationMatrix* detectRot1 = new G4RotationMatrix();
  G4RotationMatrix* detectRot2 = new G4RotationMatrix();
  G4RotationMatrix* detectRot3 = new G4RotationMatrix();
  G4RotationMatrix* detectRot4 = new G4RotationMatrix();

  detectRot1->rotateX(angle);      // rotate upper detector along X-axis by angle 
  detectRot2->rotateX(-angle);     // rotate lower detector along X-axis by angle in opposite direction
  detectRot3->rotateY(-angle);     // rotate right detector along Y-axis by angle in opposite direction
  detectRot4->rotateY(angle);      // rotate left detector along Y-axis by angle 

  //-----------------------------------------------------------------------------------------
  
  //-----------------------------------------------------------------------------------------
  // THE CANBERRA 450 mm^2 DETECTOR WITH 8 mm APERTURE (ANNULAR DETECTOR)
  //-----------------------------------------------------------------------------------------
  G4double anPhotoThickness     = 0.3 *mm;
  G4double anDetectorThickness  = 3.7 *mm;
  G4double anBackingThickness   = anDetectorThickness/2;
  G4double anEnclosingThickness = anDetectorThickness/2;
  
  G4double anInnerRadius = 8. *mm;

  //
  // PHOTOSENSITIVE REGION
  //
  G4double anPhotoOuterRadius = 23.9 *mm;

  G4ThreeVector anPhotoPlace  = G4ThreeVector((0. *mm),                  //X Position                
                                              (0. *mm),                  //Y Position 
                                              (-anPhotoThickness/2));    //Z Position 

  //
  // PHOTOSENSITIVE REGION
  //
  G4ThreeVector anEnclosingPlace  = G4ThreeVector((0. *mm),                  //X Position                
                                                  (0. *mm),                  //Y Position 
                                                  (-anBackingThickness/2));  //Z Position 

  //
  // BACKING
  //
  G4double anBackRadius = 30.5 *mm;
  G4ThreeVector anBackPlace  = G4ThreeVector((0. *mm),               //X Position                
                                             (0. *mm),               //Y Position 
                                             (anEnclosingThickness/2));  //Z Position 
  //
  // THE DETECTOR 
  // 
  G4double anDetectRadius  = 30.5 *mm;

  G4double anDetectZoffset = anDetectorThickness/2;     // Z offset 
  G4double anDetectZdist   = 2*detectSize[0];                     // Z distance
  G4double anZoffset       = anDetectZdist+anDetectZoffset; // The Z-direction placement vector 

  // PLACEMENT
  G4ThreeVector anDetectorPlace  = G4ThreeVector((0. *mm),       //X Position                
                                                 (0. *mm),       //Y Position 
                                                 (anZoffset));   //Z Position 
  //-----------------------------------------------------------------------------------------

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto siliMaterial    = G4Material::GetMaterial("G4_Si");
  auto ceramMaterial    = G4Material::GetMaterial("G4_ALUMINUM_OXIDE");
  auto alumMaterial    = G4Material::GetMaterial("G4_Al");


  if (! defaultMaterial|| ! siliMaterial|| ! ceramMaterial) 
  {
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

  //-----------------------------------------------------------------------------------------
  // THE HAMAMATSU S3204-09 DETECTOR
  //-----------------------------------------------------------------------------------------

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
  // The Ceramic extrusion
  // 
  auto extrusionS   = new G4Box("extrusion", extrusionSize[0], extrusionSize[1], extrusionSize[2]);
  auto extrusionLV  = new G4LogicalVolume(extrusionS, ceramMaterial, "extrusionLV");
  
  auto extrusionHoleS = new G4Box("extrusionHole", extrusionHoleSize[0], extrusionHoleSize[1], extrusionHoleSize[2]);
  auto extrusionHoleLV = new G4LogicalVolume(extrusionHoleS, defaultMaterial, "extrusionHoleLV");

  new G4PVPlacement(0, extrusionPlace, extrusionLV, "extrusion", detectLV, false, 0, fCheckOverlaps);
  new G4PVPlacement(0, relPosition, extrusionHoleLV, "extrusionHole", extrusionLV, false, 0, fCheckOverlaps);

  //
  // The Aluminium ring
  // 
  auto AlringS    = new G4Box("Alring", AlringSize[0], AlringSize[1], AlringSize[2]);
  auto AlringLV   = new G4LogicalVolume(AlringS, alumMaterial, "AlringLV");
  
  auto AlringHoleS = new G4Box("AlringHole", AlringHoleSize[0], AlringHoleSize[1], AlringHoleSize[2]);
  auto AlringHoleLV = new G4LogicalVolume(AlringHoleS, defaultMaterial, "AlringHoleLV");

  new G4PVPlacement(0, AlringPlace, AlringLV, "Alring", extrusionHoleLV, false, 0, fCheckOverlaps);
  new G4PVPlacement(0, relPosition, AlringHoleLV, "AlringHole", AlringLV, false, 0, fCheckOverlaps); 

  //
  // Backing
  //
  auto backS  = new G4Box("Backing", backSize[0], backSize[1], backSize[2]); 
  auto backLV = new G4LogicalVolume(backS, ceramMaterial, "backLV");     

  new G4PVPlacement(0, backPlace, backLV, "Backing", detectLV, false, 0, fCheckOverlaps);    

  //
  // The entire "chip" - Al ring, Si buffer, Al light shield
  // 
  auto AlShieldS  = new G4Box("AlShield", AlShieldSize[0], AlShieldSize[1], AlShieldSize[2]);
  auto AlShieldLV = new G4LogicalVolume(AlShieldS, defaultMaterial, "AlShieldLV");   

  auto SiBuffS    = new G4Box("SiBuff", SiBuffSize[0], SiBuffSize[1], SiBuffSize[2]);
  auto SiBuffLV   = new G4LogicalVolume(SiBuffS, siliMaterial, "SiBuffLV");

  auto diodeS  = new G4Box("Diode", diodeSize[0], diodeSize[1], diodeSize[2]);
  auto diodeLV = new G4LogicalVolume(diodeS, siliMaterial, "diodeLV");   

  new G4PVPlacement(0, diodePlace, AlShieldLV, "AlShield", backLV, false, 0, fCheckOverlaps); 
  new G4PVPlacement(0, relPosition, SiBuffLV, "SiBuff", AlShieldLV, false, 0, fCheckOverlaps); 
  new G4PVPlacement(0, relPosition, diodeLV, "Diode", SiBuffLV, false, 0, fCheckOverlaps); 
  //-----------------------------------------------------------------------------------------

  //-----------------------------------------------------------------------------------------
  // THE CANBERRA 450 mm^2 DETECTOR WITH 8 mm APERTURE (ANNULAR DETECTOR)
  //-----------------------------------------------------------------------------------------

  //
  // Detector
  //
  auto anDetectorS  = new G4Tubs("anDetector", 
                                  anInnerRadius, 
                                  anDetectRadius, 
                                  (anDetectorThickness/2), 
                                  0 *deg, 360 *deg);
  auto anDetectorLV = new G4LogicalVolume(anDetectorS, defaultMaterial, "anDetectorLV");

  new G4PVPlacement(0, anDetectorPlace, anDetectorLV, "anDetector", worldLV, false, 0, fCheckOverlaps);

  //
  // Photosensitive region
  //
  auto anPhotoRegionS  = new G4Tubs("anPhotoRegion", 
                                     anInnerRadius, 
                                     anPhotoOuterRadius, 
                                     (anPhotoThickness/2), 
                                     0 *deg, 360 *deg);
  auto anPhotoRegionLV = new G4LogicalVolume(anPhotoRegionS, siliMaterial, "anPhotoRegionLV");

  new G4PVPlacement(0, anPhotoPlace, anPhotoRegionLV, "anDetector", anDetectorLV, false, 0, fCheckOverlaps);

  //
  // Enclosing region
  //
  auto anEnclosingRegionS  = new G4Tubs("anEnclosingRegion", 
                                        anPhotoOuterRadius,
                                        anDetectRadius,
                                        (anEnclosingThickness/2), 
                                        0 *deg, 360 *deg);
  auto anEnclosingRegionLV = new G4LogicalVolume(anEnclosingRegionS, ceramMaterial, "anEnclosingRegionLV");

  new G4PVPlacement(0, anEnclosingPlace, anEnclosingRegionLV, "anDetector", anDetectorLV, false, 0, fCheckOverlaps);

  //
  // Backing
  // 
  auto anBackingS  = new G4Tubs("anBacking", 
                                 anInnerRadius, 
                                 anBackRadius, 
                                 (anBackingThickness/2), 
                                 0 *deg, 360 *deg);
  auto anBackingLV = new G4LogicalVolume(anBackingS, ceramMaterial, "anBackingLV");

  new G4PVPlacement(0, anBackPlace, anBackingLV, "anBacking", anDetectorLV, false, 0, fCheckOverlaps);

  //-----------------------------------------------------------------------------------------

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The detector is " << fNofLayers << " layers of: [ "
    << diodeThickness/mm << "mm of " << siliMaterial->GetName()
    << " + "
    << backingThickness/mm << "mm of " << ceramMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  auto whiteBoxVisAtt   = new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.9));    //rgb = (255,255,255)
  auto ceramicBoxVisAtt = new G4VisAttributes(G4Colour(0.88,0.87,0.82,0.9)); //rgb = (119,195,236)
  auto siliconBoxVisAtt = new G4VisAttributes(G4Colour(0.52,0.55,1.0,0.9));  //rgb = (0,0,139)
  auto aluminiumVisAtt  = new G4VisAttributes(G4Colour(0.52,0.53,0.54,0.9)); //rgb = (132,135,137)
  auto aluminiumRingVisAtt = new G4VisAttributes(G4Colour(1.0,0.0,0.0));     //rgb = (255,0,0)
  auto extrusionVisAtt  = new G4VisAttributes(G4Colour(0.0,1.0,0.0));        //rgb = (0,255,0)

  whiteBoxVisAtt      ->SetVisibility(true);
  ceramicBoxVisAtt    ->SetVisibility(true);
  siliconBoxVisAtt    ->SetVisibility(true);
  aluminiumVisAtt     ->SetVisibility(true);
  aluminiumRingVisAtt ->SetForceWireframe(true);
  extrusionVisAtt     ->SetForceWireframe(true);

  worldLV  ->SetVisAttributes(G4VisAttributes::GetInvisible()); //world : Invisible
  detectLV ->SetVisAttributes(G4VisAttributes::GetInvisible()); //full detector : Invisible

  //HAMAMATSU 18x18 DETECTOR
  diodeLV     ->SetVisAttributes(siliconBoxVisAtt); //diode : Silicon colour
  SiBuffLV    ->SetVisAttributes(whiteBoxVisAtt);   //Si buffer : Silicon colour

  AlringLV    ->SetVisAttributes(aluminiumRingVisAtt);  //Al ring : Red
  AlringHoleLV->SetVisAttributes(aluminiumRingVisAtt);  //Al ring hole : Red
  AlShieldLV  ->SetVisAttributes(aluminiumVisAtt);      //Al shield : Aluminium colour 

  backLV          ->SetVisAttributes(ceramicBoxVisAtt); //backing plate : Ceramic colour
  extrusionLV     ->SetVisAttributes(extrusionVisAtt); //extrusion : Green
  extrusionHoleLV ->SetVisAttributes(extrusionVisAtt); //extrusion hole : Green

  //CANBERRA ANNULAR DETECTOR
  auto AnceramicBoxVisAtt = new G4VisAttributes(G4Colour(0.88,0.87,0.82));  //rgb = (119,195,236)
  auto AnsiliconBoxVisAtt = new G4VisAttributes(G4Colour(0.52,0.55,1.0));   //rgb = (0,0,139)

  AnceramicBoxVisAtt ->SetVisibility(true);
  AnsiliconBoxVisAtt ->SetVisibility(true);

  anDetectorLV        ->SetVisAttributes(G4VisAttributes::GetInvisible()); //detector : Invisible
  anPhotoRegionLV     ->SetVisAttributes(AnsiliconBoxVisAtt);       //photosensitive : Silicon color
  anBackingLV         ->SetVisAttributes(AnceramicBoxVisAtt);       //backing plate : Ceramic color
  anEnclosingRegionLV ->SetVisAttributes(AnceramicBoxVisAtt);       //enclosure : Ceramic color

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

  auto annularSD = new CalorimeterSD("annularSD", "AnnularHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(annularSD);
  SetSensitiveDetector("anPhotoRegionLV",annularSD);
  //
  // Magnetic field - Create global magnetic field messenger.
  //
  G4ThreeVector fieldValue;   // Uniform magnetic field created if the field value is not zero.
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  G4AutoDelete::Register(fMagFieldMessenger); // Register the field messenger for deleting
}

}
