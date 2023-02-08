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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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

  G4double a;  // mass of a mole;
  G4double z;  // z = mean number of protons;
  G4double density;

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density, 
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  fNofLayers = 1;

  // Thickness parameters
  G4double diodeThickness   = 0.3  *mm;
  G4double backingThickness = 1.34 *mm;
  auto detectorThickness    = diodeThickness + backingThickness;
  auto layerThickness       = detectorThickness;

  // THE DETECTOR
  G4double detectSizeX   = 25.5 *mm,
           detectSizeY   = 25.5 *mm,
           detectSizeZ   = detectorThickness;

  G4double detectZoffset = (diodeThickness+backingThickness)/2;

  G4double detectPosX    = 0 *mm,
           detectPosY    = 0 *mm,
           detectPosZ    = 0+detectZoffset;

  // THE BACKING
  G4double backSizeX    = 25.5 *mm,
           backSizeY    = 25.5 *mm,
           backSizeZ    = backingThickness;

  G4double backPosX     = 0. *mm,
           backPosY     = 0. *mm,
           backPosZ     = diodeThickness/2;

  // THE DIODE
  G4double diodeSizeX  = 18  *mm,
           diodeSizeY  = 18  *mm,
           diodeSizeZ  = 0.3 *mm;

  G4double diodePosX   = 0. *mm,
           diodePosY   = 0. *mm,
           diodePosZ   = -backingThickness/2;  

  // THE WORLD (Position is (0,0,0) and cannot be changed)
  G4double worldSizeXYZ    = 5*diodeSizeX;
  G4ThreeVector worldPlace = G4ThreeVector();  // Cannot adjust this

  // THE LAYER (Position is relative to mother volume which is the detector)
  G4double layerSizeX  = detectSizeX,
           layerSizeY  = detectSizeY,
           layerSizeZ  = layerThickness;

  // Rotation parameters of the calorimeter
  G4double rotX = 0. *deg,
           rotY = 0. *deg,
           rotZ = 0. *deg;

  G4RotationMatrix* worldRot = new G4RotationMatrix(); //Cannot adjust this
  G4RotationMatrix* detectRot = new G4RotationMatrix();
  detectRot->rotateX(rotX);
  detectRot->rotateY(rotY);
  detectRot->rotateZ(rotZ);

  // Position in world vectors
  G4ThreeVector detectPlace = G4ThreeVector(detectPosX, detectPosY, detectPosZ);
  G4ThreeVector diodePlace  = G4ThreeVector(diodePosX, diodePosY, diodePosZ);
  G4ThreeVector backPlace   = G4ThreeVector(backPosX, backPosY, backPosZ);

  // Size vector parameters
  G4ThreeVector detectSize = G4ThreeVector(detectSizeX/2, detectSizeY/2, detectSizeZ/2);
  G4ThreeVector diodeSize  = G4ThreeVector(diodeSizeX/2, diodeSizeY/2, diodeSizeZ/2);
  G4ThreeVector backSize   = G4ThreeVector(backSizeX/2, backSizeY/2, backSizeZ/2);

  G4ThreeVector worldSize  = G4ThreeVector(worldSizeXYZ/2, worldSizeXYZ/2, worldSizeXYZ/2);
  G4ThreeVector layerSize  = G4ThreeVector(layerSizeX/2, layerSizeY/2, layerSizeZ/2);

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto diodeMaterial   = G4Material::GetMaterial("G4_Si");
  auto backMaterial    = G4Material::GetMaterial("G4_ALUMINUM_OXIDE");


  if (! defaultMaterial|| ! diodeMaterial|| ! backMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
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

  auto worldPV = new G4PVPlacement(worldRot, worldPlace, worldLV, "World", 0, false, 0, fCheckOverlaps);  

  //
  // Detector
  //
  auto detectorS = new G4Box("Detector", detectSize[0], detectSize[1], detectSize[2]);
  auto detectLV   = new G4LogicalVolume(detectorS, defaultMaterial, "Detector");

  new G4PVPlacement(detectRot, detectPlace, detectLV, "Detector", worldLV, false, 0, fCheckOverlaps);  

  //
  // Layer
  //
  auto layerS  = new G4Box("Layer", layerSize[0], layerSize[1], layerSize[2]);
  auto layerLV = new G4LogicalVolume(layerS, defaultMaterial, "Layer");    

  new G4PVReplica("Layer", layerLV, detectLV, kZAxis, fNofLayers, layerThickness);

  //
  // Diode
  // 
  auto diodeS = new G4Box("Diode", diodeSize[0], diodeSize[1], diodeSize[2]);
  auto diodeLV = new G4LogicalVolume(diodeS, diodeMaterial, "diodeLV");   

  new G4PVPlacement(0, diodePlace, diodeLV, "Diode", layerLV, false, 0, fCheckOverlaps); 

  //
  // Backing
  //
  auto backS = new G4Box("Backing", backSize[0], backSize[1], backSize[2]); 
  auto backLV = new G4LogicalVolume(backS, backMaterial, "backLV");     

  new G4PVPlacement(0, backPlace, backLV, "Backing", layerLV, false, 0, fCheckOverlaps);  

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << fNofLayers << " layers of: [ "
    << diodeThickness/mm << "mm of " << diodeMaterial->GetName()
    << " + "
    << backingThickness/mm << "mm of " << backMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  detectLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
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
  // Magnetic field
  //
  // Create global magnetic field messenger.
  
  G4ThreeVector fieldValue;   // Uniform magnetic field created if the field value is not zero.
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
