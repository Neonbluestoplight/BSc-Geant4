//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
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
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Silicon material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Si");

  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
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

  // Size parameters
  G4double calorSizeX    = 12.7 *mm;
  G4double calorSizeY    = 14.5 *mm;
  G4double absoSizeXY    = 10   *mm;
  
  // Thickness parameters
  G4double absoThickness = 0.3  *mm;
  G4double gapThickness  = 1.41 *mm;
  auto layerThickness = absoThickness + gapThickness;
  auto calorThickness = fNofLayers * layerThickness;
  auto worldSizeXYZ = 5 * absoSizeXY;

  // Position in world parameters
  G4double calorXpos     = 0. *mm;
  G4double calorYpos     = -0.85 *mm;

  // Position in world vectors
  G4ThreeVector worldPlace = G4ThreeVector();  // Cannot adjust this
  G4ThreeVector calPlace   = G4ThreeVector(calorXpos, calorYpos, 0.);
  G4ThreeVector absPlace   = G4ThreeVector(0., -calorYpos/2, -gapThickness/2);
  G4ThreeVector gapPlace   = G4ThreeVector(0., 0., absoThickness/2);

  // Size vector parameters
  G4ThreeVector worldSize = G4ThreeVector(worldSizeXYZ/2, worldSizeXYZ/2, worldSizeXYZ/2);
  G4ThreeVector calorSize = G4ThreeVector(calorSizeX/2, calorSizeY/2, calorThickness/2);
  G4ThreeVector layerSize = G4ThreeVector(calorSizeX/2, calorSizeY/2, layerThickness/2);
  G4ThreeVector absSize   = G4ThreeVector(absoSizeXY/2, absoSizeXY/2, absoThickness/2);
  G4ThreeVector gapSize   = G4ThreeVector(calorSizeX/2, calorSizeY/2, gapThickness/2);

  // Rotation parameters of the calorimeter
  G4double rotX = 0. *deg;
  G4double rotY = 0. *deg;
  G4double rotZ = 0. *deg;
  G4RotationMatrix* worldRot = new G4RotationMatrix(); //Cannot adjust this
  G4RotationMatrix* calorRot = new G4RotationMatrix();
  calorRot->rotateX(rotX);
  calorRot->rotateY(rotY);
  calorRot->rotateZ(rotZ);

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_Si");
  auto gapMaterial = G4Material::GetMaterial("G4_Si");


  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths
  // G4LogicalVolume(<solid name>,<material>,<title>)
  // G4PVPlacement(<rotation matrix>,<position vector>,<logical volume>,
  //              <title>,<mother volume>,<boolean>,<copy number>,<check overlaps>)
  auto worldS = new G4Box("World", worldSize[0], worldSize[1], worldSize[2]); 

  auto worldLV = new G4LogicalVolume(worldS, defaultMaterial, "World");         

  auto worldPV = new G4PVPlacement(worldRot, worldPlace, worldLV, 
                                   "World", 0, false, 0, fCheckOverlaps);  

  //
  // Calorimeter
  //
  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths
  // G4LogicalVolume(<solid name>,<material>,<title>)
  // G4PVPlacement(<rotation matrix>,<position vector>,<logical volume>,
  //              <title>,<mother volume>,<boolean>,<copy number>,<check overlaps>)
  auto calorimeterS = new G4Box("Calorimeter", calorSize[0], calorSize[1], calorSize[2]);

  auto calorLV = new G4LogicalVolume(calorimeterS, defaultMaterial, "Calorimeter");

  new G4PVPlacement(calorRot, calPlace, calorLV, 
                   "Calorimeter", worldLV, false, 0, fCheckOverlaps);  

  //
  // Layer
  //
  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths 
  // G4LogicalVolume(<solid name>,<material>,<title>)
  // G4VReplica(<title>,<logical volume>,<mother volume>,<axis of replication>,<number of replicas>,<replica width>)
  auto layerS = new G4Box("Layer", layerSize[0], layerSize[1], layerSize[2]);

  auto layerLV = new G4LogicalVolume(layerS, defaultMaterial, "Layer");    

  new G4PVReplica("Layer", layerLV, calorLV, 
                  kZAxis, fNofLayers, layerThickness);

  //
  // Absorber
  // 
  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths
  // G4LogicalVolume(<solid name>,<material>,<title>)
  // G4PVPlacement(<rotation matrix>,<position vector>,<logical volume>,
  //              <title>,<mother volume>,<boolean>,<copy number>,<check overlaps>)
  auto absorberS = new G4Box("Abso", absSize[0], absSize[1], absSize[2]);

  auto absorberLV = new G4LogicalVolume(absorberS, absorberMaterial, "AbsoLV");        

   new G4PVPlacement(0, absPlace, absorberLV, 
                     "Abso", layerLV, false, 0, fCheckOverlaps); 

  //
  // Gap
  //
  // G4Box(<name>,<sizeX>,<sizeY>,<sizeZ>); where size is in half lengths
  // G4LogicalVolume(<solid name>,<material>,<title>)
  // G4PVPlacement(<rotation matrix>,<position vector>,<logical volume>,
  //              <title>,<mother volume>,<boolean>,<copy number>,<check overlaps>)
  auto gapS = new G4Box("Gap", gapSize[0], gapSize[1], gapSize[2]); 

  auto gapLV = new G4LogicalVolume(gapS, gapMaterial, "GapLV");         

  new G4PVPlacement(0, gapPlace, gapLV,            
                    "Gap", layerLV, false, 0, fCheckOverlaps);  

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << fNofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName()
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

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
  auto absoSD
    = new CalorimeterSD("AbsorberSD", "AbsorberHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
  SetSensitiveDetector("AbsoLV",absoSD);

/*
  auto gapSD
    = new CalorimeterSD("GapSD", "GapHitsCollection", fNofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(gapSD);
  SetSensitiveDetector("GapLV",gapSD);
*/

  //
  // Magnetic field
  //
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
