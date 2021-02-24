#include "lcio.h"
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "EVENT/MCParticle.h"
#include "EVENT/ReconstructedParticle.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/LCEvent.h"
#include "EVENT/Track.h"
#include "UTIL/LCTOOLS.h"
#include <UTIL/CellIDDecoder.h>

#include <vector>
#include <iostream>
#include <string.h>

#include <TFile.h>
#include <TNtupleD.h>
#include <TMath.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TClassTable.h>

int main(int argc, char* argv[]) {
  
  std::string inputFileName(argv[1]);

  std::string outputFileName("caloHitsInLayer.root");

  if(argc==3) outputFileName = argv[2];
  
  int nEvents = 0;
  int maxEvt = 10000;  // change as needed
  int skipEvent = 0; 

  //if(argc == 4) maxEvt = atoi(argv[3]);

  TFile* file = new TFile(outputFileName.c_str(), "RECREATE");    

  const char* tupleNames = "event:cellX:cellY:layer:energy";
  double energy;

  //--- create a ROOT file and an NTuple
  TNtupleD *tuple= new TNtupleD("tree", "", tupleNames);


  IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader() ;
  lcReader->open(inputFileName.c_str()) ;
  
  EVENT::LCEvent* evt = 0;

  lcReader->skipNEvents(skipEvent);


  //----------- the event loop -----------
  while( (evt = lcReader->readNextEvent()) != 0  && nEvents < maxEvt) 
  {
    //    UTIL::LCTOOLS::dumpEvent( evt ) ;
	//
	++nEvents;

	//const std::vector< std::string >* strVec = evt->getCollectionNames() ;
	int event = evt->getEventNumber();
	//if(event>=maxEvt) break;

	std::cout << "Event : " << event << std::endl;
	
	const std::string colName("SDHCAL_Proto_EndCap");

	IMPL::LCCollectionVec* col = 0;

	try 
	{
		col = (IMPL::LCCollectionVec*) evt->getCollection( colName ) ;
    } 
	catch(lcio::DataNotAvailableException& e) 
	{
		std::cout << "The collection " << colName << " is not available " << std::endl;
		continue;
	}

	if(col==0) continue;

	int nItem = col->getNumberOfElements();
  
	UTIL::CellIDDecoder<EVENT::SimCalorimeterHit> decoder(col);

	std::cout << "Hit #: " << nItem << std::endl;

	//std::set<EVENT::CalorimeterHit*> hitSet;
	
	std::map<int, float>layerEnergy;

    for(int iItem=0 ; iItem<nItem ; ++iItem)
	{
      const EVENT::SimCalorimeterHit* pSimHit = dynamic_cast<EVENT::SimCalorimeterHit*>(col->getElementAt(iItem));
	  if(pSimHit == NULL) continue;

	  float hitEnergy = pSimHit->getEnergy();
	  int cellX = decoder(pSimHit)["I"];
	  int cellY = decoder(pSimHit)["J"];
	  int layer = decoder(pSimHit)["K-1"];

	  layerEnergy[layer] += hitEnergy;

	  //std::cout << "hit energy: " << hitEnergy << ", " << cellX << ", " << cellY << ", " << layer << std::endl;
	  //tuple->Fill(event, cellX, cellY, layer, hitEnergy);
    }

	for(int layer = 0; layer < layerEnergy.size(); ++layer)
	{
		tuple->Fill(event, 0, 0, layer, layerEnergy[layer]);
	}
  }

  std::cout << "End of event loop" << std::endl;

  // -------- end of event loop -----------

  file->Write() ;
  file->Close() ;
  delete file ;
  
  lcReader->close() ;
  delete lcReader ;
}
