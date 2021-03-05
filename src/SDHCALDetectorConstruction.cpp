#include "SDHCALDetectorConstruction.h"

#include <G4NistManager.hh>
#include <G4Box.hh>

#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>

#include <G4Region.hh>
#include <G4RegionStore.hh>
#include <G4VisAttributes.hh>

#include "json.hpp"

#include "SDHCALMaterials.h"
#include "SDHCALRPC.h"


G4double SDHCALDetectorConstruction::sizeX ;
G4double SDHCALDetectorConstruction::sizeZ ;

SDHCALDetectorConstruction::SDHCALDetectorConstruction(G4String jsonFileName)
{
	if ( jsonFileName == G4String("") )
	{
		std::cout << "ERROR : no json file provided" << std::endl ;
		std::terminate() ;
	}

	std::ifstream file(jsonFileName) ;
	auto json = nlohmann::json::parse(file) ;

	if ( json.count("detectorConfig") )
	{
		auto detectorConfig = json.at("detectorConfig") ;

		if ( detectorConfig.count("rpcType") )
		{
			auto type = detectorConfig.at("rpcType").get<G4String>() ;

			if ( type == "normal")
				rpcType = kNormalRPC ;
			else if ( type == "withScintillator")
				rpcType = kWithScintillatorRPC ;
			else
			{
				std::cout << "ERROR : RPC Type unknown" << std::endl ;
				std::terminate() ;
			}
		}

		if ( detectorConfig.count("oldConfig") )
			oldConfig = detectorConfig.at("oldConfig").get<G4bool>() ;
	}
}

G4VPhysicalVolume* SDHCALDetectorConstruction::Construct()
{
	G4int nLayers = 30 ; // 30
	G4int nPadX = 400 ;
	G4int nPadY = 400 ;
	G4double padSize = 5.*CLHEP::mm ;

	G4double caloSizeX = nPadX*padSize ;
	G4double caloSizeY = nPadY*padSize ;

	G4double worldSize = 12*CLHEP::m ;
	buildSDHCALMaterials() ;

	G4NistManager* man = G4NistManager::Instance() ;
	G4Material* defaultMaterial = man->FindOrBuildMaterial("G4_Galactic") ;
	G4Material* airMaterial = man->FindOrBuildMaterial("G4_AIR") ;
	G4Material* absorberMaterial = man->FindOrBuildMaterial("G4_W") ;

	//G4Material* absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L" , true) ;
	//if ( oldConfig ) //to reproduce old results
	//	absorberMaterial = G4Material::GetMaterial("SDHCAL_Steel304L_Old" , true) ;

	if(absorberMaterial == nullptr) G4cout << "Error: absorberMaterial is null" << G4endl;

	// World
	G4Box* solidWorld = new G4Box("World", worldSize/2 , worldSize/2 , worldSize/2) ;
	G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld , defaultMaterial , "World") ;
	G4VPhysicalVolume* physiWorld = new G4PVPlacement(nullptr , G4ThreeVector() ,  logicWorld , "World" , nullptr , false , 0 , true) ;

	std::vector<SDHCALRPC*> rpcVec ;
	std::vector<G4double> absorberStructureSizeZVec ;

	G4double totalAbsorberStructureSizeZ = 0.;

	for ( G4int i = 0 ; i < nLayers ; ++i )
	{
		if ( rpcType == kNormalRPC )
		{
			if ( oldConfig )
				rpcVec.push_back( SDHCALRPC::buildOldStandardRPC(i , nPadX , nPadY , padSize) ) ;
			else
				rpcVec.push_back( SDHCALRPC::buildStandardRPC(i , nPadX , nPadY , padSize) ) ;		
		}
		else if ( rpcType == kWithScintillatorRPC )
			rpcVec.push_back( SDHCALRPC::buildWithScintillatorRPC(i , nPadX , nPadY , padSize) ) ;

		const G4double tungRadlen = 3.5;

		if(i < 10)
		{
			absorberStructureSizeZVec.push_back(0.6 * tungRadlen); // radiation length
			totalAbsorberStructureSizeZ += 0.6 * tungRadlen;
		}
		else if(i < 20)
		{
			absorberStructureSizeZVec.push_back(0.6 * tungRadlen);
			totalAbsorberStructureSizeZ += 0.6 * tungRadlen;
		}
		else
		{
			absorberStructureSizeZVec.push_back(1.2 * tungRadlen); 
			totalAbsorberStructureSizeZ += 1.2 * tungRadlen;
		}
	}

	G4double RPCSizeZ = rpcVec.at(0)->getSizeZ() ;

	G4double airGapSizeZ = 0.001*CLHEP::mm ;
	if ( oldConfig ) //to reproduce old results
		airGapSizeZ = 0*CLHEP::mm ;

	G4double caloSizeZ = totalAbsorberStructureSizeZ + nLayers*( 2*airGapSizeZ + RPCSizeZ ) ;

	sizeX = caloSizeX ;
	sizeZ = caloSizeZ ;

	//calorimeter volume
	G4Box* solidCalorimeter = new G4Box("Calorimeter" , caloSizeX/2 , caloSizeY/2 , caloSizeZ/2) ;
	G4LogicalVolume* logicCalorimeter = new G4LogicalVolume(solidCalorimeter , airMaterial , "Calorimeter") ;

	G4double currentPos = -caloSizeZ/2 ;
	for ( unsigned int i = 0 ; i < static_cast<unsigned int>( nLayers ) ; ++i )
	{
		G4double absorberStructureSizeZ = absorberStructureSizeZVec.at(i);

		currentPos += absorberStructureSizeZ/2 ;
		G4Box* solidAbsorberStructure = new G4Box("AbsorberStructure" , caloSizeX/2 , caloSizeY/2 , absorberStructureSizeZ/2) ;
		G4LogicalVolume* logicAbsorberStructure = new G4LogicalVolume(solidAbsorberStructure , absorberMaterial , "AbsorberStructure") ;

		G4VisAttributes * visAtt = new G4VisAttributes();
		visAtt->SetColour(G4Colour(G4Colour(0.4, 0.8, 1.0, 0.2)));
        visAtt->SetForceSolid(true);
        visAtt->SetVisibility(true); 

		logicAbsorberStructure->SetVisAttributes(visAtt);

		new G4PVPlacement(nullptr , G4ThreeVector(0,0,currentPos) , logicAbsorberStructure , "AbsorberStructure" , logicCalorimeter , false , 0 , true) ;

		currentPos += absorberStructureSizeZ/2 + airGapSizeZ + RPCSizeZ/2 ;

		std::stringstream name ; name << "Cassette" << i ;
		rpcVec.at(i)->createPhysicalVolume(nullptr , G4ThreeVector(0,0,currentPos) , logicCalorimeter) ;

		currentPos += RPCSizeZ/2 + airGapSizeZ ;
	}

	G4VPhysicalVolume* calorimeter = new G4PVPlacement( nullptr , G4ThreeVector(0 , 0 , 0.5*caloSizeZ) , logicCalorimeter , "Calorimeter" , logicWorld , false , 0 , true) ;

	//for stepping action
	G4Region* regionCalor = G4RegionStore::GetInstance()->FindOrCreateRegion("RegionCalorimeter") ;
	regionCalor->AddRootLogicalVolume(logicCalorimeter) ;

	SDHCALSteppingAction::Instance()->setInterestedRegion(regionCalor) ;
	SDHCALSteppingAction::Instance()->setPhysicalVolume(calorimeter) ;

	return physiWorld ;
}
