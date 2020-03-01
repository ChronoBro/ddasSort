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

using namespace std;

//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int GetGlobalChannelNum(ddaschannel *dchan, int firstSlot){
  return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();
}

int loadBar(double progress){

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

  int barWidth = 100;//70;

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
   
   BUILDING LIST OF EVENTS FROM DATA

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
    outRootfile << "run_" << runStart << "-" << runEnd << "_processed.root";
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
      cout << "adding " << infile.str() << " to TChain..." << endl;
      }
      else{break;}
    } 

  }
  
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
  
  chain->SetBranchAddress("ddasevent", &pDDASEvent); //this setups pointer to address of events in tree dchan, with the structure of a ddasevent
  
  //int lastEntry=chain->GetEntries()-1; //set default to be end of loop so it stops. Need to update entry in list so that events don't get double counted if they get grouped into an event
  int lastEntry = 0;

  for(int iEntry=0;iEntry<chain->GetEntries();iEntry=lastEntry){
  
    double progress0 =  (double)iEntry/(double)chain->GetEntries();
    loadBar(progress0);

    chain->GetEntry(iEntry);  
    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //case and point from below comment

    //cout << iEntry << endl;
    //pChannels = pDDASEvent->GetData();
    ddaschannel *curChannel = pDDASEvent->GetData()[0]; //only looking at first event in whole DDASEvent, presumably there is only one per size because Andy "unbuilt" it
    //ddaschannel *curChannel = pChannels[0];
    

    int curChanNo = GetGlobalChannelNum(curChannel,2);
    unsigned int curEnergy = curChannel->GetEnergy();
    double  curTime       = curChannel->GetCoarseTime();
    double  curCFDTime    = curChannel->GetTime();

    // Fill a histo
    Histo->h_raw_summary->Fill(curChanNo,curChannel->GetEnergy());


    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    double fTrigCh         =     0;        // Build events around a central trigger channel. (IGNORE FOR DSSD)
    double fCoinWindow     =  5000;//2000;        // Time (ns)
    double fPromptWindow   =  2000;
    double fCoinGammaWindow=  1000;        // Time (ns)
    double fCorrWindow     =  10*(1e9*0.1);        // Time (ns)  now 1000ms
    //  fCorrWindow     =  10*(1e9*0.020);        // Time (ns)  now 200ms
    double fDSSDThreshold  =     0;

    
    int fSearchEntry            = iEntry;
  
    int fImplantEFMaxStrip = -1;
    int fImplantEBMaxStrip = -1;
    int fImplantEFMaxERaw  = 0;
    int fImplantEBMaxERaw  = 0;
    int nChEFLow  = 0;
    int nChEFHigh = 0;
    int nChEBLow  = 0;
    int nChEBHigh = 0;


    // Initialize variables
    ddaschannel * serChannel = new ddaschannel;
    double delay = 0;
    double nextTDiff    = 0;    
    int serChanNo = 0;
    double curTOF = 0;
    int fNextEventEntry = 0;
    double CFDTimeDiff = 0;

    // TRIGGER SEARCH ------
    if( curChanNo != fTrigCh) { 
      fSearchEntry++;
      lastEntry= fSearchEntry;
      continue;
    }
    //if(curChanNo < 64 || curChanNo > 223) return true;

    // COINCIDENCE SEARCH AND/OR EVENT BUILDING------
    // Next, search for events within the coincidence window.
    // ...

    // Go to beginning of window.
    bool foundCoinWinBegin = false;
    if(iEntry==0){ fSearchEntry = 0; foundCoinWinBegin = true;}
    else          fSearchEntry = iEntry-1;
    while(!foundCoinWinBegin){
      chain->GetTree()->GetEntry(fSearchEntry);
      serChannel = pDDASEvent->GetData()[0];
      serChanNo   = GetGlobalChannelNum(serChannel,2);
      delay      = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
    
      if(-nextTDiff > fCoinWindow/2) {foundCoinWinBegin = true; fSearchEntry++;}
      else                           fSearchEntry--;
      if(fSearchEntry<1)             foundCoinWinBegin = true;

    }


    int     nCh   = 0;
    int nEvents=0;
    while(fSearchEntry+1<fNEntries){
      chain->GetTree()->GetEntry(fSearchEntry);
      serChannel  = pDDASEvent->GetData()[0];
      serChanNo   = GetGlobalChannelNum(serChannel,2);
      delay       = fh_ChannelDelays->GetBinContent(serChanNo+1); // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff   = (serChannel->GetCoarseTime()-delay) - curTime;
      CFDTimeDiff = (serChannel->GetTime()-delay)       - curCFDTime;

      // Update the entry for the close of the window.
      fNextEventEntry = fSearchEntry + 1;
    
      // Check that the event is within the coincidence window.
      if(abs(nextTDiff)+fCoinWindow/2>fCoinWindow) break;
    
      nEvents++;
      // Found a signal.
      // Check that the channel is not the trigger channel.
      serChanNo = GetGlobalChannelNum(serChannel,2);
    
      if     (serChanNo == fTrigCh && fSearchEntry == iEntry){
	// Do nothing . . .
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
	  //        fImplantEFMaxStrip = serChanNo-104;
	  fImplantEFMaxStrip = 40 - (serChanNo-104);

	}
	nCh++;
	nChEFLow++;
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
	nChEBLow++;
      }
      else{
	// Found something else . . .
	//cerr << "ERROR: Unaccounted channel " << serChanNo << endl;
      }
      fSearchEntry++;

    }
    
    Histo->h_mult->Fill(nCh);

    if(fSearchEntry > iEntry){
    lastEntry = fSearchEntry;
    }
    else{
      lastEntry = iEntry++;
    }
    //h_mult_F->Fill(nChEFLow);
    //h_mult_B->Fill(nChEBLow);
    //  h_raw_DSSD_hits   ->Fill(fImplantEBMaxStrip,    fImplantEFMaxStrip);
    //  h_raw_DSSD_hitsMap->Fill(fImplantEBMaxStrip,   -1*(fImplantEFMaxStrip-39));

  
    if(!fGate->IsInside(curTOF,curEnergy)){ 
      continue;
      } //only look at events that are inside gate

    Histo->hImplantXY->Fill(fImplantEBMaxStrip-20,(-1*(fImplantEFMaxStrip-39))-20);
    Histo->h_PID->Fill(curTOF,curEnergy);
    //cout << "Found Ion" << endl;
    // Now search for decay events. ------------------------------------
    fSearchEntry            = iEntry;
    int fDecayEFMaxStrip = -1;
    int fDecayEBMaxStrip = -1;
    int fDecayEFMaxERaw  = 0;
    int fDecayEBMaxERaw  = 0;
    int fDecayEFMaxTraceAmp = 0;
    nChEFLow  = 0;
    nChEFHigh = 0;
    nChEBLow  = 0;
    nChEBHigh = 0;
    double decayTime = 0;
  
    while(fSearchEntry+1<nEvents){
      cout << endl<< "Made it to second while loop" << endl;
      chain->GetTree()->GetEntry(fSearchEntry);
      serChannel  = pDDASEvent->GetData()[0];
      serChanNo   = GetGlobalChannelNum(serChannel,2);
      delay       = fh_ChannelDelays->GetBinContent(serChanNo+1); // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff   = (serChannel->GetCoarseTime()-delay) - curTime;
      CFDTimeDiff = (serChannel->GetTime()-delay)       - curCFDTime;

      // Update the entry for the close of the window.
      //fNextEventEntry = fSearchEntry + 1;

      // Check that the event is within the correlation window.
      if(abs(nextTDiff)>fCorrWindow) break;

      if(nextTDiff<000){
	continue;
	// cont.
      }
      else if(serChanNo > 64 && serChanNo <= 103){ // IGNORE CH 0
	// DSSD Fronts high gain
	if(serChannel->GetEnergy()>fDecayEFMaxERaw) {
	  fDecayEFMaxERaw = serChannel->GetEnergy();
	  fDecayEFMaxStrip = serChanNo-64;
	  // Get the trace
	  // for(Int_t iTrace=0; iTrace<500; iTrace++) fDecayEFMaxTrace[iTrace] = serChannel->GetTrace()[iTrace];
	  // //        Int_t decayEFMaxs[3] = {0,0,0};
	  // for(Int_t iTrace=0; iTrace<500; iTrace++){
	  //   if(fDecayEFMaxTraceAmp<fDecayEFMaxTrace[iTrace]) fDecayEFMaxTraceAmp = fDecayEFMaxTrace[iTrace];
	  //}
	}
	Histo->h_raw_DSSD[serChanNo - 64]->Fill(serChannel->GetEnergy());
	decayTime = serChannel->GetCoarseTime();
	nCh++;
	nChEFHigh++;
      }
      else if(serChanNo >= 144 && serChanNo <= 183){
	// DSSD Backs high gain
	if(serChannel->GetEnergy()>fDecayEBMaxERaw) {
	  fDecayEBMaxERaw = serChannel->GetEnergy();
	  fDecayEBMaxStrip = serChanNo-144;
	}
	Histo->h_raw_DSSD[serChanNo - 104]->Fill(serChannel->GetEnergy());
	nCh++;
	nChEBHigh++;
      }
      else{
	// Found something else . . .
	//cerr << "ERROR: Unaccounted channel " << serChanNo << endl;
      }

      if(fImplantEFMaxStrip > -1 && fImplantEBMaxStrip >-1 &&
	 fDecayEFMaxStrip   > -1 && fDecayEBMaxStrip   >-1 &&
	 (TMath::Abs(fImplantEFMaxStrip-fDecayEFMaxStrip)<2) &&
	 (TMath::Abs(fImplantEBMaxStrip-fDecayEBMaxStrip)<2) &&
	 nextTDiff>=000) {
      
	Histo->hDecayXY->Fill(fImplantEBMaxStrip-20,(-1*(fDecayEFMaxStrip-39))-20);

	Histo->hDecayTime->Fill(nextTDiff);
	TF1 *fDSSDHighCal = (TF1*)fDSSDCalibFunc->At(fDecayEFMaxStrip);
	TRandom  * fRand          = new TRandom();
	Double_t DSSDEvalue = fDSSDHighCal->Eval(fDecayEFMaxERaw+fRand->Uniform());
	Double_t DSSDEvalueAmp = fDSSDHighCal->Eval(fDecayEFMaxTraceAmp+fRand->Uniform());
	Histo->hDecayEnergy->Fill(DSSDEvalue);
	Histo->hDecayEnergyAmp->Fill(DSSDEvalueAmp);

      }
      fSearchEntry++;
    }

  }



  Histo->write(); // this forces the histrograms to be read out to file

  return 1;
}
