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
//#include "ddasDet.h"
#include "ddasArray.h"

using namespace std;


struct DSSDevent{
  //can make this more complicated later, keeping things simple for now

  double energy = 0;  //high gain MeV
  unsigned int energyR = -1; //Raw signal
  int channel = -1;
  int strip = -1;
  double time = 0;
  long long int entry = 0;
  
};

long long int coincidenceSearch(TChain &dataChain, long long int trigEntry, double coinWindow, double curTime, ddaschannel * serChannel, DDASEvent * eventPointer, vector<ddasDet> &activeDets, ddasDet &useful){

  long long int fNEntries = dataChain.GetEntries();
  long long int lastEntry = trigEntry;

  for(long long int iSearch = trigEntry-1;iSearch < fNEntries;iSearch--){
    if(trigEntry ==0 || iSearch < 0 || iSearch == trigEntry){
      break; //don't want iSearch to go below zero
    }
    dataChain.GetEntry(iSearch);
    ddasDet checkTime(-1);
    //serChannel = eventPointer->GetData()[0];
    checkTime.fillEvent(serChannel, eventPointer);
    double nextTDiff  = checkTime.getFillerEvent().time - curTime;
    //lastEntry = iSearch;

    if(abs(nextTDiff) < coinWindow/2){
      useful.fillEvent(serChannel,eventPointer);
      for(auto &arrays : activeDets){
	if(arrays.fillEvent(serChannel, eventPointer)){;}//cout << "wtf" << endl;}
      }

    }
    else{
      break; //as long as events are time ordered this shouldn't be a problem
    }

  }    

  //now look for coincidence in entries after trigger
  for(long long int iSearch = trigEntry+1;iSearch<fNEntries;iSearch++){
    dataChain.GetEntry(iSearch);

    ddasDet checkTime(-1);
    checkTime.fillEvent(serChannel, eventPointer);
    double nextTDiff  = checkTime.getFillerEvent().time - curTime;

    lastEntry = iSearch;

    if(abs(nextTDiff) < coinWindow/2){
      useful.fillEvent(serChannel,eventPointer);
      for(auto &arrays : activeDets){
	if(arrays.fillEvent(serChannel, eventPointer)){;}//cout << "wtf" << endl;}
      }

    }
    else{
      break; //as long as events are time ordered this shouldn't be a problem
    }

  }

  return lastEntry;

}


//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int GetGlobalChannelNum(ddaschannel *dchan, int firstSlot){
  return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();
}

void loadBar(double progress){

  //being silly and using cerr to show loading bar while piping a log to a file

  fputs("\e[?25l", stderr); /* hide the cursor */

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

  std::cerr << " [";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) std::cerr << "=";
    else if (i == pos) std::cerr << ">";
    else std::cerr << " ";
  }
  std::cerr << "] " << percent.str() << " %                      \r";
  std::cerr.flush();

  fputs("\e[?25h", stderr); /* show the cursor */

}

int main(int argc,char *argv[]){



  cerr << endl << "NSCL DDAS sorting for unbuilt Events" << endl << endl;



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

  TChain dataChain("dchan");
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
      dataChain.AddFile(infile.str().c_str());
      //cout << "adding " << infile.str() << " to TChain...                           " << endl; //have to use carriage return for flush

      }
      else{
	////cout << "Couldn't find file " << infile.str() << "                        " << endl; 
	break;
      }
    } 

  }

  ////cout << "adding " << infile.str() << " to TChain..." << endl;
  
  //cout << endl;

  long long int fNEntries = chain->GetEntries(); //64bit int, because I was using int for the entries I wasn't able to access long lists, hence sort failing for a long list of data files

  //cout << "Number of entries in TChain = " << chain->GetEntries() << endl;
  //cout << endl;



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



  ifstream alphaCalib("Calibrations/alphaCalib.dat");

  fDSSDCalFile = new TFile("Calibrations/Alpha/AlphaCalib_293.root","READ");
  vector<int> frontChannels;
  vector<int> backChannels;
  vector< vector<double> > frontCalib;
  vector< vector<double> > backCalib;

  vector<ddasDet> DSSDtest;
  vector<ddasDet> DSSDtest2;
  for(Int_t i=0; i<80; i++){
    sprintf(name,"f_cal_%02i_sp",i);
    int chan=0.;
    double slope;
    double offset;
    alphaCalib >> chan >> offset >> slope; 

    ddasDet fillDet(chan);
    vector<double> params;
    params.push_back(offset);
    params.push_back(slope);
    fillDet.setCalibration(params);
    //DSSDtest.push_back(fillDet);
    vector<double> paramsFront;
    vector<double> paramsBack;
    
    if(i<40){
      frontChannels.push_back(chan);paramsFront.push_back(offset);paramsFront.push_back(slope);frontCalib.push_back(paramsFront);
      DSSDtest.push_back(fillDet);
    }
    if(i>=40){backChannels.push_back(chan);paramsBack.push_back(offset);paramsBack.push_back(slope);backCalib.push_back(paramsBack);DSSDtest2.push_back(fillDet);}

    TF1 *fun = (TF1*)fDSSDCalFile->FindObjectAny(name);
    TF1 *fCal = new TF1(name,"pol1",0,35000);
    
    
    fCal->SetParameter(0, offset); //Dan Calib
    fCal->SetParameter(1, slope);

    // fCal->SetParameter(0, fun->GetParameter(0)); //Chris Calib
    // fCal->SetParameter(1, fun->GetParameter(1));

    fDSSDCalibFunc->Add(fCal);
  }
  alphaCalib.close();

  ddasArray DSSDfront(frontChannels);
  DSSDfront.setCalibration(frontCalib);
  ddasArray DSSDback(backChannels);
  DSSDback.setCalibration(backCalib);

  vector<ddasArray> DSSD;
  DSSD.push_back(DSSDfront);
  DSSD.push_back(DSSDback);

  // Load SeGA Calibrartions
  int SeGAchannel = 16;
  //ddasDet *SEGA[16];
  fSeGACalFile = new TFile("Calibrations/SeGACalibrations.root","READ");
  vector<int> SEGAchannels;
  vector< vector<double> > SEGAparamList;
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    TF1 *fCalG = new TF1(name,"pol1",0,35000);

    SEGAchannels.push_back(SeGAchannel);
    
    vector<double> params;
    params.push_back(funG->GetParameter(0));
    params.push_back(funG->GetParameter(1));
    SEGAparamList.push_back(params);

    SeGAchannel++;

    fCalG->SetParameter(0, funG->GetParameter(0));
    fCalG->SetParameter(1, funG->GetParameter(1));
    fSeGACalibFunc->Add(fCalG);

    delete funG;
  }
  fSeGACalFile->Close();

  ddasArray SEGAarray(SEGAchannels);
  SEGAarray.setCalibration(SEGAparamList);
  
  // Open channel delays file.
  fChannelDelays = new TFile("Calibrations/ChannelDelays.root","READ");
  if(!fChannelDelays->IsZombie()){
    fh_ChannelDelays = (TH1D*)fChannelDelays->FindObjectAny("h_Delays");
    ////cout << fh_ChannelDelays->GetBinContent(4) << endl;
  }else{
    cerr << "WARNING: Could not find channel delay file. Delays set to ZERO." << endl;
    fh_ChannelDelays = new TH1D("h_Delays","",300,0,300);
  }
  
  // Load PID gate

  TFile *fGateFile = new TFile("PIDGates.root","READ");
    fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
  //  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));

  //cout << "--> Loaded Gate: " << fGate->GetName() << endl << endl;
  fGateFile->Close();


  /*****************************************
   
   LOOP OVER DDAS EVENTS IN TCHAIN

   ****************************************/

  cerr << " Processing Events..." << endl; 



  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  //std::vector<ddaschannel*> pChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  ddaschannel * curChannel = new ddaschannel;
  ddaschannel * serChannel =new ddaschannel; //If you call new ddaschannels in a loop it will cause a memory leak. 


  dataChain.SetBranchAddress("ddasevent", &pDDASEvent);
  long long int lastEntry = 0; 

  //below would be used for filling a tree with only events involving ION of interest
  //Would also need to include "correlated" events as well
  //Should cut down on processing time tremendously if done

  // TFile * outFile = new TFile("ions.root","recreate");
  // TTree * oTree = new TTree("dchan","list of ions of interest");
  // DDASEvent * ionEvent = new DDASEvent;
  // //std::vector<ddaschannel*> ionChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  // oTree->Branch("ddasEvent",&ionEvent);

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry){
    //for(long long int iEntry=0;iEntry<chain->GetEntries();iEntry=lastEntry){

   
    double progress0 =  (double)iEntry/(double)dataChain.GetEntries();
    if(iEntry % 10000 == 0){   
      loadBar(progress0);    
    }   
    
    dataChain.GetEntry(iEntry);
    //chain->GetEntry(iEntry);  
    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //case and point from below comment

    ////cout << iEntry << endl;
    //pChannels = pDDASEvent->GetData();
    //ddaschannel *curChannel = pChannels[0]; //only useful if we're DDAS built events
    
    ddaschannel *curChannel = pDDASEvent->GetData()[0]; //only looking at first event in whole DDASEvent, presumably there is only one per size because Andy "unbuilt" it
    //I'd ultimately like to be able to work on built and "unbuilt" data in the same way


    int trigChan =0; //channel for PIN1 at front of stack
    ddasDet trigger(trigChan);
    ddasDet TOF(5); //trigger for PIN2-XFP TAC
    
    int curChanNo = -1;
    curChanNo = GetGlobalChannelNum(curChannel,2);
    unsigned int curEnergy = curChannel->GetEnergy();
    double  curTime       = curChannel->GetCoarseTime();
    double  curCFDTime    = curChannel->GetTime();
    double curTOF = 0.;
    double PIN1energy = curEnergy;
    int nCh = 0;


    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    //only want to create events where PIN1 is the trigger

    if( trigger.fillEvent(curChannel, pDDASEvent) == false){ //don't know why ! function doesn't seem to be working
      lastEntry = iEntry + 1;
      continue;
    }
    
    // if(!trigger.fillEvent(curChannel,pDDASEvent)){
    //   cout << trigger.fillEvent(curChannel,pDDASEvent) << endl;
    // }

    double coinWindow     =  5000;//5000;//2000;        // Time (ns) //now that coincidence window seems to be working it would be nice to open up window and see
                                                                     //what happens
    double promptWindow   =  2000;
    double coinGammaWindow=  1000;        // Time (ns)
    double corrWindow     =  1E9;        // Time (ns)  now 1000ms (1s)
    double nextTDiff = 1E9;
    bool foundCoin = false;
    bool foundIonOfInterest = false;


    int fImplantEFMaxStrip = -100;
    int fImplantEBMaxStrip = -100;
    unsigned int fImplantEFMaxERaw  = 0;
    unsigned int fImplantEBMaxERaw  = 0;
    double decaytime = 0;

    int fDecayEFMaxStrip = -100;
    int fDecayEBMaxStrip = -100;
    unsigned int fDecayEFMaxERaw  = 0;
    unsigned int fDecayEBMaxERaw  = 0;
    int fDecayEFMaxTraceAmp = 0;

    //DSSDfront.fillEvent(curChannel,pDDASEvent);

    lastEntry = coincidenceSearch(dataChain, iEntry, coinWindow, curTime, serChannel, pDDASEvent, DSSDtest,DSSDtest2, TOF);

 
    for( auto & tof : TOF.getEvents() ){
      if(tof.signal > curTOF){
	curTOF = tof.signal;
	//cout << curTOF << endl;
      }
    }

    //Histo->h_raw_summary->Fill(serChanNo,serChannel->GetEnergy());
	
	
      
    //Histo->h_mult->Fill(nCh);
    Histo->h_PID->Fill(curTOF,PIN1energy);
      
    if( fGate->IsInside(curTOF,PIN1energy) ){ //once I have routine for checking DSSD events, should require that there are front+back events
      foundIonOfInterest = true;
    }
    
    
      
    if(!foundIonOfInterest){ 	//only look at events that are inside gate
      DSSDfront.clear();
      DSSDback.clear();
      continue;
    }



    cout << endl << "Found Ion!" << endl;

    unsigned int EmaxFront = 0;
    unsigned int EmaxBack = 0;

    cout << DSSDfront.getMasterEventList().size() << endl;
    cout << DSSDback.getMasterEventList().size() << endl;
    // for(auto &test: DSSDtest){
    //   //cout << test.getEvents().size() << endl;
    //   test.clear();
    // }

    //DSSDtest.clear();

      //going to take events with the shortest time between them
    double TdiffMin = coinWindow;  //maximum value, so long as coincidence events are the only ones passed into DSSDevent vectors
    for(auto &front : DSSDfront.getMasterEventList()){
	
      for(auto &back : DSSDback.getMasterEventList()){
	unsigned int Ediff = abs(front.signal - back.signal); //when dealing with unsigned ints, negative numbers get wrapped around to largest numbers
	//This was causing problems when trying to determine Ediff for decay events
	if(front.signal > back.signal) {Ediff = front.signal - back.signal;}
	  else {Ediff = back.signal - front.signal;}
	
	double Tdiff = abs(front.time - back.time);
	//if(Ediff < 1000){
	//if(Tdiff<TdiffMin){ //I believe should use this determination if for assigning strip, along with maxE
	cout << endl;
	cout << "Found coincident DSSDevent" << endl;
	cout << "Tdiff = " << Tdiff << endl;
	cout << "Ediff = " << Ediff << endl;
	cout << "FrontE = " << front.signal << endl;
	cout << "BackE = " << back.signal << endl;
	//cout << "Front strip = " << front.strip << endl;
	cout << "Front channel = " << front.channel << endl;
	//cout << "Back strip = " << back.strip << endl;
	cout << "Back channel = " << back.channel << endl;
	TdiffMin = Tdiff; //presumably the strips that fire closest in time should be the implantation strips-->or max energy 
	//}
	//}
	if(front.signal > EmaxFront){
	  fImplantEFMaxStrip = 40 - (front.channel-104); //should look into this mapping at some point ;
	  front.energy = EmaxFront;
	}
	if(back.signal > EmaxBack){
	  fImplantEBMaxStrip = 40 - (back.channel-184); //should look into this mapping at some point;
	  back.energy = EmaxBack;
	}	

      }
      
    }
    
    
      
      


    //now look for decay correlations

    //no need to look at past events for correlations
    
    //now look for correlations in entries after trigger

    //should include some to make sure that we aren't associating decay events with the wrong trigger
    for(long long int iSearch = iEntry+1;iSearch<fNEntries;iSearch++){
      dataChain.GetEntry(iSearch);
      //int serChanNo   = GetGlobalChannelNum(serChannel,2);
      //double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---

      ddasDet checkTime(-1);
      checkTime.fillEvent(serChannel, pDDASEvent);
      nextTDiff  = checkTime.getFillerEvent().time - curTime;

      lastEntry = iSearch;
      
      if(nextTDiff < corrWindow && nextTDiff > coinWindow/2. && nextTDiff >0){
	DSSDfront.fillEvent(serChannel, pDDASEvent);
	DSSDback.fillEvent(serChannel,pDDASEvent);
      }
      else if(abs(nextTDiff) < coinWindow/2.){
	continue;
      }
      else if(nextTDiff > corrWindow || trigger.fillEvent(serChannel, pDDASEvent)){//also want to exclude "correlations" if there is another implantation event
	//I wonder if there is a way to untangle ambiguity of two implantation events
	
	if(trigger.fillEvent(serChannel, pDDASEvent)){lastEntry = iSearch;} //move to the next trigger if found
	break; //as long as events are time ordered this shouldn't be a problem, should confirm time-ordered property
      }
    }
    
    
    DSSDevent decayEventFront;
    DSSDevent decayEventBack;
    double EdiffMin = 1000;
    double EdiffThreshold = 200; //keV
    double Ethreshold = 400;
    int stripTolerance = 2;
    double Emax = 0;
    TdiffMin = coinWindow;
    bool foundPotDecay = false;
    
    
    //I could really use a vector of "valid" DSSD events... I guess I should go ahead with abstraction      
    
    //for now going to take 'max' energy event in order to filter only proton events
    
    for(auto &front : DSSDfront.getMasterEventList() ){
      
      int frontStrip = front.channel - 64;
      double Tdiff = coinWindow; //maximum value, so long as coincidence events are the only ones passed into DSSDevent vectors
      
      for(auto &back : DSSDback.getMasterEventList() ){
	
	int backStrip = back.channel - 144;
	
	double Ediff = abs(front.energy - back.energy);
	double time = back.time - curTime;
	
	if(front.energy > back.energy) {Ediff = front.energy - back.energy;}
	else {Ediff = back.energy - front.energy;}
	
	Tdiff = abs(front.time - back.time);
	if(Ediff < EdiffThreshold && back.energy > Ethreshold && front.energy > Ethreshold && Tdiff < coinWindow && abs(fImplantEFMaxStrip - frontStrip) < stripTolerance && abs(fImplantEBMaxStrip - backStrip) < stripTolerance){
	  cout << endl;
	  cout << "Found potential decay event" << endl;
	  cout << "Tdiff = " << Tdiff << endl;
	  cout << "Ediff = " << Ediff << endl;
	  cout << "FrontE = " << front.energy << endl;
	  cout << "BackE = " << back.energy << endl;
	  //cout << "Front strip = " << front.strip << endl;
	  cout << "Front channel = " << front.channel << endl;
	  //cout << "Back strip = " << back.strip << endl;
	  cout << "Back channel = " << back.channel << endl;
	  cout << "Recorded decay time = " << time << endl; 
	  
	  
	  //if(Ediff < EdiffMin && Tdiff < TdiffMin){ // will eventually need a better criterion, as there may be multiple "correlated" events that match above condition
	  
	  //have to take multiple decay events or I will miss a lot...
	  //also this method will make a large difference into what you will see as decays... tread carefully
	  
	  //if(Tdiff < TdiffMin){ //going to take minimum time, like previously algorithm
	  //need to start consolidating these methods into classes...
	  if(front.energy > Emax){
	    decayEventFront.energy = front.energy;
	    //decayEventFront.strip = front.strip;
	    decayEventFront.time = front.time;
	    decayEventFront.entry = front.entry;
	    
	    decayEventBack.energy = back.energy;
	    //decayEventBack.strip = back.strip;
	    decayEventBack.time = back.time;
	    decayEventBack.entry = back.entry;
	    
	    Emax = front.energy;
	    EdiffMin = Ediff;
	    TdiffMin = Tdiff;
	    foundPotDecay = true;
	    //}
	    //}
	  }
	  
	}
	
      }
      
    } //end loop over correlated DSSD events
    
    if(foundPotDecay){
      Histo->hDecayXY->Fill(fImplantEBMaxStrip-20,(-1*(decayEventFront.strip-39))-20);
      //TF1 *fDSSDHighCal = (TF1*)fDSSDCalibFunc->At(fDecayEBMaxStrip);
      
      ////cout << "DSSDEvalue = " << DSSDEvalue << endl;
      //cout << endl << "Reported E = " << decayEventFront.energy << endl;
      Histo->hDecayEnergy->Fill(decayEventFront.energy);
      double time = decayEventFront.time -curTime;
      Histo->hDecayTime->Fill(time);
      if(time < 3E8){
	Histo->hDecayEnergyTGate->Fill(decayEventFront.energy);
      }
      
      long long int decayEntry = decayEventFront.entry;
      
      vector<ddasArray> activeArray;
      activeArray.push_back(SEGAarray);
      
      //now look for gamma coincidences
      
      ddasDet filler; //I know this is stupid but I want to see if these methods are working and then I will clean up more
      coincidenceSearch(dataChain, decayEntry, coinWindow, decayEventFront.time, serChannel, pDDASEvent, activeArray, filler);

      for(auto &event : activeArray[0].getMasterEventList()){
	Histo->hGammaEnergy->Fill(event.energy);
	Histo->hSeGAEnergy->Fill(event.channel-16,event.energy);
	Histo->hGammaVsDecay->Fill(decayEventFront.energy,event.energy);
      }
	
    } //end ifPotDecay
    
    DSSDfront.clear();
    DSSDback.clear();
    SEGAarray.clear();
    //technically shouldn't need these clears since vectors were initialized in loop over entries
    

    //could fill a tree with only DDASEvents that comprise of specific isotope
    //could make class that does this so long as there is a specified PID file and PID Gate file

  } //end of loop over entries

  ////cout << endl << "made it out of loop of events" << endl;

  Histo->write(); // this forces the histrograms to be read out to file

  return 1;
}
