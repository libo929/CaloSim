#include "SDHCALDetectorConstruction.h"
#include "SDHCALPrimaryGeneratorAction.h"
#include "SDHCALRunAction.h"
#include "SDHCALEventAction.h"
#include "SDHCALTrackingAction.h"
#include "SDHCALStackingAction.h"

#include <G4PhysListFactory.hh>

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <string>

#include "json.hpp"


struct Params
{
		G4int seed = 0 ;
		G4int nEvent = 1 ;
		G4String physicsList = "FTFP_BERT" ;
		G4String outputFileName = "output" ;
		G4bool killNeutrons = false ;
		G4int runID = -1 ;
} ;

Params readJsonFile(G4String jsonFileName)
{
	Params params ;

	if ( jsonFileName == G4String("") )
	{
		std::cout << "ERROR : no json file provided" << std::endl ;
		std::terminate() ;
	}

	std::ifstream file(jsonFileName) ;
	auto json = nlohmann::json::parse(file) ;

	if ( json.count("outputFileName") )
		params.outputFileName = json.at("outputFileName").get<G4String>() ;
	if ( json.count("physicsList") )
		params.physicsList = json.at("physicsList").get<G4String>() ;
	if ( json.count("nEvents") )
		params.nEvent = json.at("nEvents").get<G4int>() ;
	if ( json.count("seed") )
		params.seed = json.at("seed").get<G4int>() ;
	if ( json.count("killNeutrons") )
		params.killNeutrons = json.at("killNeutrons").get<G4bool>() ;
	if ( json.count("runID") )
		params.runID= json.at("runID").get<G4int>() ;

	return params ;
}

int main(int argc , char** argv)
{
	if ( argc != 2 )
	{
		G4cerr << "Error with arguments passed for the program" << G4endl ;
		std::terminate() ;
	}
	
	G4String jsonFileName = argv[1] ;

	Params params = readJsonFile( jsonFileName ) ;

	CLHEP::HepRandom::setTheSeed(params.seed) ;

	G4RunManager* runManager = new G4RunManager ;

	bool useUI = false ;

	if(params.runID < 0) useUI = true ;

        if(!useUI)
        { 
        	runManager->SetRunIDCounter(params.runID);
        }

	// Detector construction
	runManager->SetUserInitialization( new SDHCALDetectorConstruction(jsonFileName) ) ;

	// Physics list
	G4PhysListFactory* physFactory = new G4PhysListFactory() ;
	runManager->SetUserInitialization( physFactory->GetReferencePhysList(params.physicsList) ) ;

	// Primary generator action
	runManager->SetUserAction( new SDHCALPrimaryGeneratorAction( jsonFileName ) ) ;

	SDHCALRunAction* runAction = nullptr ;

        if(useUI)
	{
		SDHCALRunAction::setCreateGlobleWriter();
		runAction = new SDHCALRunAction(params.outputFileName) ;
	}
	else
	{
		runAction = new SDHCALRunAction() ;
	}

        if(!useUI)
	{
		runAction->setLcioFileName( params.outputFileName + G4String(".slcio") ) ;
		runAction->setRootFileName( params.outputFileName + G4String(".root") ) ;
	}

	runManager->SetUserAction( runAction ) ;
	runManager->SetUserAction( new SDHCALEventAction(runAction) ) ;

	runManager->SetUserAction( SDHCALSteppingAction::Instance() ) ;
	runManager->SetUserAction( SDHCALTrackingAction::Instance() ) ;
	runManager->SetUserAction( SDHCALStackingAction::Instance() ) ;

	SDHCALStackingAction::Instance()->setKillNeutrons(params.killNeutrons) ;

	runManager->Initialize() ;

       if(useUI)
       {
		G4VisManager* visManager = new G4VisExecutive("Quiet");
		visManager->Initialize();

		G4UIExecutive* ui = new G4UIExecutive(argc, argv);

		G4UImanager::GetUIpointer()->ApplyCommand("/control/macroPath ./");
		G4UImanager::GetUIpointer()->ApplyCommand("/control/execute init_vis.mac");

		if (ui->IsGUI()) {
			G4UImanager::GetUIpointer()->ApplyCommand("/control/execute gui.mac");
		}

		ui->SessionStart();

		delete ui;
		delete visManager;
	}
	else
	{
		runManager->BeamOn(params.nEvent) ;
	}

	delete runManager ;

	return 0 ;
}
