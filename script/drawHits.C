void drawHits()
{
	const string drawOption = "lego";
	//const string drawOption = "colz";

	TFile* f = new TFile("caloHits.root");

	TTree* inTree = (TTree*)gDirectory->Get("ce");

	int evtIndex;
	int hitSize;

	std::vector<int>* cellVecX = 0;
	std::vector<int>* cellVecY = 0;
	std::vector<int>* cellVecZ = 0;

	std::vector<float>* posVecX = 0;
	std::vector<float>* posVecY = 0;
	std::vector<float>* posVecZ = 0;

	std::vector<float>* energyVec = 0;

	inTree->SetBranchAddress("evtIndex", &evtIndex);
	inTree->SetBranchAddress("hitSize",  &hitSize);

	inTree->SetBranchAddress("cellVecX", &cellVecX);
	inTree->SetBranchAddress("cellVecY", &cellVecY);
	inTree->SetBranchAddress("cellVecZ", &cellVecZ);

	inTree->SetBranchAddress("posVecX", &posVecX);
	inTree->SetBranchAddress("posVecY", &posVecY);
	inTree->SetBranchAddress("posVecZ", &posVecZ);

	inTree->SetBranchAddress("energyVec", &energyVec);

	///////////////////////
	
    TCanvas* c1 = new TCanvas("c1","Hits",10, 10, 600, 600);

	//TH2F* hist = new TH2F("hitsHist", "calo hits", 100, 350, 450, 100, 250, 350);
	TH2F* hist = new TH2F("hitsHist", "calo hits", 60, 170, 230, 60, 170, 230);

	int maxEvt = 10000;

	if(maxEvt > inTree->GetEntries()) maxEvt = inTree->GetEntries();

	for(unsigned int evt = 0; evt < maxEvt; ++evt)
	{
		inTree->GetEntry(evt);

		for(int i = 0; i < hitSize; ++i)
		{
			int cellx = cellVecX->at(i);
			int celly = cellVecY->at(i);
			float energy = energyVec->at(i);

			hist->Fill(cellx, celly, energy);
		}
	
		hist->Draw(drawOption.c_str());

		c1->Update();
		c1->Draw();
		c1->Update();

		std::cout << "evt: " << evt << ", hit #: " << hitSize << ", press q to quit, any other key to continue..."  << std::endl;

		char c = getchar();
		
		if(c == 'q')
			break;
		else
			hist->Reset();
	}
		
}
