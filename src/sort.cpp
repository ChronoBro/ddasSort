#include "/opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h" 
#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include "histo.h"
#include "TChain.h"
#include "TF1.h"
#include "TList.h"
#include "TCutG.h"
#include "TRandom.h"
#include <vector>

using namespace std;

//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int GetGlobalChannelNum(ddaschannel *dchan, int firstSlot){
  return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();
}

void loadBar(double progress){

  fputs("\e[?25l", stdout); /* hide the cursor */

  ostringstream percent;

  if(progress*100 < 0.1){
    percent << setprecision(1) << progress*100.;
  }
  else if(progress*100 < 1){
    percent << setprecision(2) << progress*100.;
  }
  else if(progress*100 < 10){
    percent << setprecision(3) << progress*100.;
  }
  else{
    percent << setprecision(4) << progress*100.;
  }

  int barWidth = 50;//100;//70;

  std::cout << " [";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) std::cout << "=";
    else if (i == pos) std::cout << ">";
    else std::cout << " ";
  }
  std::cout << "] " << percent.str() << " %                        \r";
  std::cout.flush();

  fputs("\e[?25h", stdout); /* show the cursor */

}

int main(int argc,char *argv[]){



  cout << endl << "NSCL DDAS sorting for unbuilt Events" << endl << endl;



  /*****************************************
   
   OPENING TCHAIN OF EVENTS FROM DATA

   ****************************************/


  int runStart=99;
  int runEnd=99;
  ostringstream outRootfile;

  if(argc==1){
    outRootfile << "run_" << runStart << "-" << runEnd << "_processed.root";
  }
  else if(argc==2){
    runStart = atoi(argv[1]);
    runEnd=runStart;
    outRootfile << "run_" << runStart << "_processed.root";
  }
  else if(argc==3){
    runStart = atoi(argv[1]);
    runEnd = atoi(argv[2]);
    outRootfile << "run_" << runStart << "-" << runEnd << "_processed.root";
  }
  else if(argc==4){
    runStart = atoi(argv[1]);
    runEnd = atoi(argv[2]);
    string s_outRootfile(argv[3]);
    outRootfile << s_outRootfile << ".root";
  }


  histo * Histo = new histo(outRootfile.str());

  int maxSubRun = 9;

  TChain * chain = new TChain("dchan"); //name of tree has to be same in each root file 

  for(int iFile=runStart;iFile<=runEnd;iFile++){
 
    for(int subRun = 0;subRun <= maxSubRun;subRun++){

      ostringstream infile;      

      if(iFile>= 100){
	infile << "remote/root-files/run-0" << iFile << "-0" << subRun << "_NoBuild.root";
      }
      else{
	infile << "remote/root-files/run-00" << iFile << "-0" << subRun << "_NoBuild.root";
      }   

      if( exists_test(infile.str()) ){

      chain->AddFile(infile.str().c_str());
      cout << "adding " << infile.str() << " to list...                           " << endl; //have to use carriage return for flush

      }
      else{
	//cout << "Couldn't find file " << infile.str() << "                        " << endl; 
	break;
      }
    } 

  }

  //cout << "adding " << infile.str() << " to TChain..." << endl;
  
  cout << endl;

  int fNEntries = chain->GetEntries();

  cout << "Number of entries in TChain = " << chain->GetEntries() << endl;
  cout << endl;



  /*****************************************
   
   LOADING CALIBRATION FILES

   ****************************************/


  TFile          *fDSSDCalFile; //!
  TFile          *fSeGACalFile; //!
  
  TFile    *fChannelDelays;          //! File containing delays for each channel.
  TH1D     *fh_ChannelDelays;        //!

  TList *fDSSDCalibFunc; //! List of calibration functions.
  TList *fSeGACalibFunc; //! List of calibration functions.
  TCutG *fGate;          //! PID Gate


  char name[500],title[500];

  fDSSDCalibFunc = new TList();
  fSeGACalibFunc = new TList();


  ifstream alphaCalib("alphaCalib.dat");

  for(Int_t i=0; i<80; i++){
    sprintf(name,"f_cal_%02i_sp",i);
    int chan=0.;
    double slope;
    double offset;
    alphaCalib >> chan >> offset >> slope; 
    TF1 *fCal = new TF1(name,"pol1",0,35000);
    fCal->SetParameter(0, offset);
    fCal->SetParameter(1, slope);
    fDSSDCalibFunc->Add(fCal);
  }
  alphaCalib.close();

  // Load SeGA Calibrartions
  fSeGACalFile = new TFile("Calibrations/SeGACalibrations.root","READ");
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    TF1 *fCalG = new TF1(name,"pol1",0,35000);
    fCalG->SetParameter(0, funG->GetParameter(0));
    fCalG->SetParameter(1, funG->GetParameter(1));
    fSeGACalibFunc->Add(fCalG);
    delete funG;
  }
  fSeGACalFile->Close();
  
  // Open channel delays file.
  fChannelDelays = new TFile("Calibrations/ChannelDelays.root","READ");
  if(!fChannelDelays->IsZombie()){
    fh_ChannelDelays = (TH1D*)fChannelDelays->FindObjectAny("h_Delays");
    //cout << fh_ChannelDelays->GetBinContent(4) << endl;
  }else{
    cerr << "WARNING: Could not find channel delay file. Delays set to ZERO." << endl;
    fh_ChannelDelays = new TH1D("h_Delays","",300,0,300);
  }
  
  // Load PID gate
  TFile *fGateFile = new TFile("PIDGates.root","READ");
  //  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
  //  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));

  cout << "--> Loaded Gate: " << fGate->GetName() << endl << endl;
  fGateFile->Close();


  /*****************************************
   
   LOOP OVER DDAS EVENTS IN TCHAIN

   ****************************************/

  cout << " Processing Events..." << endl; 



  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  //std::vector<ddaschannel*> pChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  ddaschannel * curChannel = new ddaschannel;
  ddaschannel * serChannel =new ddaschannel; //If you call new ddaschannels in a loop it will cause a memory leak. 

  
  
  chain->SetBranchAddress("ddasevent", &pDDASEvent); //this setups pointer to address of events in tree dchan, with the structure of a ddasevent
  
  //int lastEntry=chain->GetEntries()-1; //set default to be end of loop so it stops. Need to update entry in list so that events don't get double counted if they get grouped into an event
  int lastEntry = 0;

  //below would be used for filling a tree with only events involving ION of interest
  //Would also need to include "correlated" events as well
  //Should cut down on processing time tremendously if done

  // TFile * outFile = new TFile("ions.root","recreate");
  // TTree * oTree = new TTree("dchan","list of ions of interest");
  // DDASEvent * ionEvent = new DDASEvent;
  // //std::vector<ddaschannel*> ionChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  // oTree->Branch("ddasEvent",&ionEvent);


  for(int iEntry=0;iEntry<chain->GetEntries();iEntry=lastEntry){
  
    double progress0 =  (double)iEntry/(double)chain->GetEntries();
    loadBar(progress0);
    
    cout << iEntry;

    chain->GetEntry(iEntry);  
    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //case and point from below comment

    //cout << iEntry << endl;
    //pChannels = pDDASEvent->GetData();
    //ddaschannel *curChannel = pChannels[0]; //only useful if we're DDAS built events
    
    ddaschannel *curChannel = pDDASEvent->GetData()[0]; //only looking at first event in whole DDASEvent, presumably there is only one per size because Andy "unbuilt" it
    int trigChan =0; //channel for PIN1 at front of stack
    int curChanNo = -1;
    curChanNo = GetGlobalChannelNum(curChannel,2);
    unsigned int curEnergy = curChannel->GetEnergy();
    double  curTime       = curChannel->GetCoarseTime();
    double  curCFDTime    = curChannel->GetTime();
    double curTOF = 0.;
    double PIN1energy = 0;
    int nCh = 0;



    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    //only want to create events where PIN1 is the trigger
    if(!curChanNo==trigChan){ 
      lastEntry = iEntry + 1;
      continue;
    }


    double coinWindow     =  5000;//2000;        // Time (ns)
    double promptWindow   =  2000;
    double coinGammaWindow=  1000;        // Time (ns)
    double corrWindow     =  10*(1e9*0.1);        // Time (ns)  now 1000ms
    double nextTDiff = 1E9;
    bool foundCoin = false;
    bool foundIonOfInterest = false;
    vector<int> channelsCoin; //create a list of entries that are in Coin with beginning of window
    vector<int> channelsCorr;
    channelsCoin.push_back(iEntry); //a channel is alwasy in coincidence with itself

    int fImplantEFMaxStrip = -1;
    int fImplantEBMaxStrip = -1;
    unsigned int fImplantEFMaxERaw  = 0;
    unsigned int fImplantEBMaxERaw  = 0;
    double decaytime = 0;

    int fDecayEFMaxStrip = -1;
    int fDecayEBMaxStrip = -1;
    unsigned int fDecayEFMaxERaw  = 0;
    unsigned int fDecayEBMaxERaw  = 0;
    int fDecayEFMaxTraceAmp = 0;


    
    for(int iSearch = iEntry+1;iSearch <= fNEntries;iSearch++){
      chain->GetEntry(iSearch);
      //ddaschannel * serChannel = new ddaschannel;
      serChannel = pDDASEvent->GetData()[0];
      int serChanNo   = GetGlobalChannelNum(serChannel,2);
      double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
  
      //cout << nextTDiff << endl;

      if(abs(nextTDiff) > coinWindow){
	lastEntry = iSearch;
	break;
      }
      else{
	channelsCoin.push_back(iSearch);
	lastEntry = iSearch;
      }
      //delete[] serChannel;
    }    

      if(channelsCoin.size() > 100){
	cout << endl<< "something must have gone wrong..." << endl;
	cout << "Coincidences = " <<channelsCoin.size() << endl;
      }
      
      if(channelsCoin.size() > 500){
	cout << endl<< "something must have REALLY gone wrong..." << endl;
	cout << "Coincidences = " <<channelsCoin.size() << endl;
	channelsCoin.clear();
	continue;
      }
  
      

    //loop over events found in Coincidence
    //for (std::vector<int>::iterator it = channelsCoin.begin() ; it != channelsCoin.end(); ++it){
      for(auto &it : channelsCoin){
	chain->GetEntry(it);
	//ddaschannel * serChannel = new ddaschannel;
	serChannel = pDDASEvent->GetData()[0];
	int serChanNo   = GetGlobalChannelNum(serChannel,2);


	if(serChanNo == 0){
	  //We found PIN1;
	  PIN1energy = serChannel->GetEnergy();
	  nCh++;
	}
	else if(serChanNo == 1){
	  // We found PIN1-PIN2 in coincidence.
	  nCh++;
	}
	else if(serChanNo == 2){
	  // We found PIN1-RF TAC in coincidence.
	  nCh++;
	}
	else if(serChanNo == 3){
	  // We found PIN2-RF TAC in coincidence.
	  nCh++;
	}
	else if(serChanNo == 4){
	  // We found PIN1-XFP TAC in coincidence.
	  nCh++;
	}
	else if(serChanNo == 5){
	  // We found PIN2-XFP TAC in coincidence.
	  curTOF = serChannel->GetEnergy();
	  nCh++;
	}
	else if(serChanNo == 6){
	  // We found XFP in coincidence.
	  nCh++;
	}
	else if(serChanNo == 15){
	  // We found scintillator in coincidence.
	  nCh++;
	}
	else if(serChanNo >= 32 && serChanNo <= 47){
	  // SSSD
	  nCh++;
	}
	else if(serChanNo >= 48 && serChanNo <= 63){
	  // SSSD low gain
	  nCh++;
	}
	else if(serChanNo >= 64 && serChanNo <= 103){
	  // DSSD Fronts high gain
	  nCh++;
	}
	else if(serChanNo >= 104 && serChanNo <= 143){
	  // DSSD Fronts low gain
	  if(serChannel->GetEnergy()>fImplantEFMaxERaw) {
	    fImplantEFMaxERaw = serChannel->GetEnergy();
	    // //        fImplantEFMaxStrip = serChanNo-104;
	    fImplantEFMaxStrip = 40 - (serChanNo-104);

	  }
	  nCh++;
	  //nChEFLow++;
	}
	else if(serChanNo >= 144 && serChanNo <= 183){
	  // DSSD Backs high gain
	  nCh++;
	}
	else if(serChanNo >= 184 && serChanNo <= 223){
	  // DSSD Backs low gain
	  if(serChannel->GetEnergy()>fImplantEBMaxERaw) {
	    fImplantEBMaxERaw = serChannel->GetEnergy();
	    //        fImplantEBMaxStrip = serChanNo-184;
	    fImplantEBMaxStrip = 40 - (serChanNo-184);
	  }
	  nCh++;
	  //nChEBLow++;
	}
	else{
	  // Found something else . . .
	  //cerr << "ERROR: Unaccounted channel " << serChanNo << endl;
	}

	Histo->h_raw_summary->Fill(serChanNo,serChannel->GetEnergy());

	
	//delete[] serChannel;

	//could map list of channels (for instance front of dssd) to one number and still use switch below (better looking than if-else series)

	// switch(serChanNo){
	// case 0 :
	//   nCh++;
	//   PIN1energy = serChannel->GetEnergy();
	// case 1:
	//   // We found PIN1-PIN2 in coincidence
	//   nCh++;
	// case 2:
	//   //We found PIN1-RF TAC in coincidence
	//   nCh++;
	// case 3:
	//   //We found PIN2-RF TAC in coincidence.
	//   nCh++;
	// case 4:
	//   //We found PIN1-XFP TAC in coincidence
	//   nCh++;
	// case 5:
	//   //We found PIN2-XFP TAC in coincidence
	//   curTOF = serChannel->GetEnergy();
	//   nCh++;
	// case 6:
	//   //We found XFP in coincidence
	//   nCh++;
	// case 15:
	//   //We found scintillator in coincidence
	//   nCh++;
	//   case 
	  
	// }

	
      } //end of loop over coincidences

      
      Histo->h_mult->Fill(nCh);
      Histo->h_PID->Fill(curTOF,PIN1energy);
      
      if( fGate->IsInside(curTOF,PIN1energy) ){ 
	foundIonOfInterest = true;
	
	
      }
      

      channelsCoin.clear();
      
      if(!foundIonOfInterest){ 	//only look at events that are inside gate
	continue;
      }
      
      cout << endl << "Found Ion!" << endl;

      //now look for decay correlations


      for(int iSearch = iEntry+1;iSearch <= fNEntries;iSearch++){
	chain->GetEntry(iSearch);
	//ddaschannel * serChannel = new ddaschannel;
	serChannel = pDDASEvent->GetData()[0];
	int serChanNo   = GetGlobalChannelNum(serChannel,2);
	double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
	nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
  
	//cout << nextTDiff << endl;

	if(abs(nextTDiff) > corrWindow){
	  break;
	}
	else{
	  channelsCorr.push_back(iSearch);
	}
	//delete[] serChannel;
      }    

      nCh = 0;
      for(auto &it : channelsCorr){
	chain->GetEntry(it);
	//ddaschannel * serChannel = new ddaschannel;
	serChannel = pDDASEvent->GetData()[0];
	int serChanNo   = GetGlobalChannelNum(serChannel,2);

	if(serChanNo >= 64 && serChanNo <= 103){
	  // DSSD Fronts high gain
	  if(serChannel->GetEnergy()>fDecayEFMaxERaw) {
	    fDecayEFMaxERaw = serChannel->GetEnergy();
	    // //        fImplantEFMaxStrip = serChanNo-104;
	    fDecayEFMaxStrip = serChanNo = 64;
	    //double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
	    //nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
	  }
	  nCh++;
	  decaytime = (double)serChannel->GetCoarseTime();
	}
	else if(serChanNo >= 144 && serChanNo <= 183){
	  // DSSD Backs high gain
	  if(serChannel->GetEnergy()>fDecayEBMaxERaw) {
	    fDecayEBMaxERaw = serChannel->GetEnergy();
	    fDecayEBMaxStrip = serChanNo-144;
	    //        fImplantEBMaxStrip = serChanNo-184;
	    //fImplantEBMaxStrip = serChanNo-184;
	  }
	  nCh++;
	}
	else{
	  // Found something else . . .
	  //cerr << "ERROR: Unaccounted channel " << serChanNo << endl;
	}

	//Histo->h_raw_summary->Fill(serChanNo,serChannel->GetEnergy());
      //delete[] serChannel;

      } //end of loop over correlations

      channelsCorr.clear();

      if(fImplantEFMaxStrip > -1 && fImplantEBMaxStrip >-1 &&
	 fDecayEFMaxStrip   > -1 && fDecayEBMaxStrip   >-1 &&
	 (abs(fImplantEFMaxStrip-fDecayEFMaxStrip)<2) &&
	 (abs(fImplantEBMaxStrip-fDecayEBMaxStrip)<2) &&
	 nextTDiff>=000) {

	cout << "Found decay!" << endl;
      
	Histo->hDecayXY->Fill(fImplantEBMaxStrip-20,(-1*(fDecayEFMaxStrip-39))-20);

	//Histo->hDecayTime->Fill(nextTDiff);
	TF1 *fDSSDHighCal = (TF1*)fDSSDCalibFunc->At(fDecayEBMaxStrip);
	TRandom  * fRand          = new TRandom();
	Double_t DSSDEvalue = fDSSDHighCal->Eval(fDecayEBMaxERaw+fRand->Uniform());
	//Double_t DSSDEvalueAmp = fDSSDHighCal->Eval(fDecayEFMaxTraceAmp+fRand->Uniform());
	Histo->hDecayEnergy->Fill(DSSDEvalue);
	Histo->hDecayTime->Fill(decaytime);
	//Histo->hDecayEnergyAmp->Fill(DSSDEvalueAmp);
	//delete fRand;
	//delete fDSSDHighCal;
      }


      //could fill a tree with only DDASEvents that comprise of specific isotope
      //could make class that does this so long as there is a specified PID file and PID Gate file

  } //end of loop over entries

  //cout << endl << "made it out of loop of events" << endl;

  Histo->write(); // this forces the histrograms to be read out to file

  return 1;
}
