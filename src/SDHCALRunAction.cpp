#include "SDHCALRunAction.h"
#include "SDHCALRun.h"
#include "SDHCALLcioWriter.h"

#include <globals.hh>
#include <ctime>

//SDHCALRunAction::SDHCALRunAction()
//	: G4UserRunAction()
//{
//}

bool SDHCALRunAction::createGlobalWriter = false;

G4Run* SDHCALRunAction::GenerateRun()
{
	return new SDHCALRun ;
}

SDHCALRunAction::SDHCALRunAction(G4String fileName)
	: G4UserRunAction::G4UserRunAction(), 
	  lcioFileName(fileName), rootFileName(fileName + ".root")
{
	if(createGlobalWriter)
	{
		G4UserRunAction();
		
		writer = new SDHCALLcioWriter( lcioFileName ) ;
	    SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	    rootWriter->createRootFile( rootFileName ) ;

		G4cout << "========== name: " << lcioFileName << " : " << rootFileName << G4endl;
	
		G4cout << "Create Global Writer" << G4endl;
	}
}

SDHCALRunAction::~SDHCALRunAction()
{
	if(createGlobalWriter)
	{
		SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	    rootWriter->closeRootFile() ;

	    SDHCALRootWriter::deleteInstance() ;

	    if (writer) delete writer ;
		writer = nullptr ;

		G4cout << "Delete Global Writer" << G4endl;
	}

	G4UserRunAction::~G4UserRunAction();
}

void SDHCALRunAction::BeginOfRunAction(const G4Run*)
{
	if(!createGlobalWriter)
	{
	    writer = new SDHCALLcioWriter( lcioFileName ) ;

	    SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	    rootWriter->createRootFile( rootFileName ) ;
	    
		G4cout << "Create Writer" << G4endl;
	}
}

void SDHCALRunAction::EndOfRunAction(const G4Run*)
{
	if(!createGlobalWriter)
	{
	    SDHCALRootWriter* rootWriter = SDHCALRootWriter::Instance() ;
	    rootWriter->closeRootFile() ;

	    SDHCALRootWriter::deleteInstance() ;

	    if (writer)
	    	delete writer ;

	    writer = nullptr ;
	
		G4cout << "Delete Writer" << G4endl;
	}
}

