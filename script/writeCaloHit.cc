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

  std::string outputFileName("caloHits.root");

  if(argc==3) outputFileName = argv[2];
  
  int nEvents = 0;
  int maxEvt = 10000;  // change as needed
  int skipEvent = 0; 

  TFile* file = new TFile(outputFileName.c_str(), "RECREATE");    

  const char* tupleNames = "event:cellX:cellY:layer:energy";
  double energy;
 
  ///////////////////////

  // for record the events in new format
  TTree* outTree = new TTree("ce", "ROOTCaloEvent");

  std::vector<int> cellVecX;
  std::vector<int> cellVecY;
  std::vector<int> cellVecZ;
 
  std::vector<float> posVecX;
  std::vector<float> posVecY;
  std::vector<float> posVecZ;
 
  std::vector<float> energyVec;

  int evtIndex;
  int hitSize;

  ///////////////////////

  outTree->Branch("evtIndex", &evtIndex);
  outTree->Branch("hitSize",  &hitSize);
 
  outTree->Branch("cellVecX", &cellVecX);
  outTree->Branch("cellVecY", &cellVecY);
  outTree->Branch("cellVecZ", &cellVecZ);
 
  outTree->Branch("posVecX", &posVecX);
  outTree->Branch("posVecY", &posVecY);
  outTree->Branch("posVecZ", &posVecZ);
 
  outTree->Branch("energyVec", &energyVec);

  //--- create a ROOT file and an NTuple
  TNtupleD *tuple= new TNtupleD("tree", "", tupleNames);


  IO::LCReader* lcReader = IOIMPL::LCFactory::getInstance()->createLCReader() ;
  lcReader->open(inputFileName.c_str()) ;
  
  EVENT::LCEvent* evt = 0;

  lcReader->skipNEvents(skipEvent);


  //----------- the event loop -----------
  while( (evt = lcReader->readNextEvent()) != 0  && nEvents < maxEvt) 
  {
	++nEvents;

	int event = evt->getEventNumber();

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

	float eventEnergy = 0.;

	evtIndex = event;
	hitSize = 0;

    for(int iItem=0 ; iItem<nItem ; ++iItem)
	{
      const EVENT::SimCalorimeterHit* pSimHit = dynamic_cast<EVENT::SimCalorimeterHit*>(col->getElementAt(iItem));
	  if(pSimHit == NULL) continue;

	  float hitEnergy = pSimHit->getEnergy();

	  int cellX = decoder(pSimHit)["I"];
	  int cellY = decoder(pSimHit)["J"];
	  int layer = decoder(pSimHit)["K-1"];


	  cellVecX.push_back(cellX);
	  cellVecY.push_back(cellY);
	  cellVecZ.push_back(layer);


	  // make a shift of the detector
	  const float detShift = 1800.;
	  const float layerThicknell1 = 2.1 + 0.525;
	  //const float layerThicknell2 = 4.2 + 0.525;
	  float layerPos = layer * layerThicknell1 + detShift;

	  if(layer > 19) layerPos += 2.1 * (layer - 19);

	  posVecX.push_back( (cellX - 200) * 5. );
	  posVecY.push_back( (cellY - 200) * 5. );
	  posVecZ.push_back( layerPos           );

	  energyVec.push_back(hitEnergy);

	  eventEnergy += hitEnergy;

	  ++hitSize;

	  //std::cout << "hit energy: " << hitEnergy << ", " << cellX << ", " << cellY << ", " << layer << std::endl;
    }

	tuple->Fill(event, 0, 0, 0, eventEnergy);
	outTree->Fill();

	cellVecX.clear();
	cellVecY.clear();
	cellVecZ.clear();

	posVecX.clear();
	posVecY.clear();
	posVecZ.clear();

	energyVec.clear();
  }

  std::cout << "End of event loop" << std::endl;

  // -------- end of event loop -----------

  file->Write() ;
  file->Close() ;
  delete file ;
  
  lcReader->close() ;
  delete lcReader ;
}
