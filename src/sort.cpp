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
#include "ddasDet.h"

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
      cout << "adding " << infile.str() << " to TChain...                           " << endl; //have to use carriage return for flush

      }
      else{
	//cout << "Couldn't find file " << infile.str() << "                        " << endl; 
	break;
      }
    } 

  }

  //cout << "adding " << infile.str() << " to TChain..." << endl;
  
  cout << endl;

  long long int fNEntries = chain->GetEntries(); //64bit int, because I was using int for the entries I wasn't able to access long lists, hence sort failing for a long list of data files

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



  ifstream alphaCalib("Calibrations/alphaCalib.dat");

  fDSSDCalFile = new TFile("Calibrations/Alpha/AlphaCalib_293.root","READ");
  for(Int_t i=0; i<80; i++){
    sprintf(name,"f_cal_%02i_sp",i);
    int chan=0.;
    double slope;
    double offset;
    alphaCalib >> chan >> offset >> slope; 
    
    TF1 *fun = (TF1*)fDSSDCalFile->FindObjectAny(name);
    TF1 *fCal = new TF1(name,"pol1",0,35000);
    
    
    fCal->SetParameter(0, offset); //Dan Calib
    fCal->SetParameter(1, slope);

    // fCal->SetParameter(0, fun->GetParameter(0)); //Chris Calib
    // fCal->SetParameter(1, fun->GetParameter(1));

    fDSSDCalibFunc->Add(fCal);
  }
  alphaCalib.close();

  // Load SeGA Calibrartions
  int SeGAchannel = 16;
  //ddasDet *SEGA[16];
  vector<ddasDet> SEGA;
  fSeGACalFile = new TFile("Calibrations/SeGACalibrations.root","READ");
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    TF1 *fCalG = new TF1(name,"pol1",0,35000);
    vector<double> params;
    //SEGA[i] = new ddasDet(SeGAchannel);
    ddasDet fillArray(SeGAchannel);
    params.push_back(funG->GetParameter(0));
    params.push_back(funG->GetParameter(1));
    fillArray.setCalibration(params);
    SEGA.push_back(fillArray);


    fCalG->SetParameter(0, funG->GetParameter(0));
    fCalG->SetParameter(1, funG->GetParameter(1));
    fSeGACalibFunc->Add(fCalG);
    SeGAchannel++;
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

  cerr << " Processing Events..." << endl; 



  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  //std::vector<ddaschannel*> pChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  ddaschannel * curChannel = new ddaschannel;
  ddaschannel * serChannel =new ddaschannel; //If you call new ddaschannels in a loop it will cause a memory leak. 


  
  chain->SetBranchAddress("ddasevent", &pDDASEvent); //this setups pointer to address of events in tree dchan, with the structure of a ddasevent
  
  //int lastEntry=chain->GetEntries()-1; //set default to be end of loop so it stops. Need to update entry in list so that events don't get double counted if they get grouped into an event
  long long int lastEntry = 0; 

  //below would be used for filling a tree with only events involving ION of interest
  //Would also need to include "correlated" events as well
  //Should cut down on processing time tremendously if done

  // TFile * outFile = new TFile("ions.root","recreate");
  // TTree * oTree = new TTree("dchan","list of ions of interest");
  // DDASEvent * ionEvent = new DDASEvent;
  // //std::vector<ddaschannel*> ionChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  // oTree->Branch("ddasEvent",&ionEvent);


  for(long long int iEntry=0;iEntry<chain->GetEntries();iEntry=lastEntry){

   
    double progress0 =  (double)iEntry/(double)chain->GetEntries();
    if(iEntry % 10000 == 0){   
      loadBar(progress0);    
    }   

    chain->GetEntry(iEntry);  
    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //case and point from below comment

    //cout << iEntry << endl;
    //pChannels = pDDASEvent->GetData();
    //ddaschannel *curChannel = pChannels[0]; //only useful if we're DDAS built events
    
    ddaschannel *curChannel = pDDASEvent->GetData()[0]; //only looking at first event in whole DDASEvent, presumably there is only one per size because Andy "unbuilt" it
    //I'd ultimately like to be able to work on built and "unbuilt" data in the same way


    int trigChan =0; //channel for PIN1 at front of stack
    int curChanNo = -1;
    curChanNo = GetGlobalChannelNum(curChannel,2);
    unsigned int curEnergy = curChannel->GetEnergy();
    double  curTime       = curChannel->GetCoarseTime();
    double  curCFDTime    = curChannel->GetTime();
    double curTOF = 0.;
    double PIN1energy = 0;
    int nCh = 0;

    ddasDet test(0); //call this in scope that you want to use it so that .clear() (in destructor) is called when it goes out of scope, otherwise seems to work
    test.fillEvent(curChannel, pDDASEvent);
    vector<Event> testEvents = test.getEvents();
    //cout << endl << "number of events filled in test = " << testEvent.size() << endl;


    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    //only want to create events where PIN1 is the trigger
    //may need to look into this and see if this is working properly
    if(curChanNo!=trigChan){ 
      lastEntry = iEntry + 1;
      continue;
    }


    double coinWindow     =  5000;//5000;//2000;        // Time (ns) //now that coincidence window seems to be working it would be nice to open up window and see
                                                                     //what happens
    double promptWindow   =  2000;
    double coinGammaWindow=  1000;        // Time (ns)
    double corrWindow     =  1E9;        // Time (ns)  now 1000ms (1s)
    double nextTDiff = 1E9;
    bool foundCoin = false;
    bool foundIonOfInterest = false;
    vector<int> channelsCoin; //create a list of entries (channels fired) that are in Coin with trigger
    vector<int> channelsCorr; //create a list of entries (channels fired) that are correlated with trigger

    channelsCoin.push_back(iEntry); //a channel is alwasy in coincidence with itself

    vector<DSSDevent> backEvents;
    vector<DSSDevent> frontEvents; 


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


    //look for coincidences before trigger first
    for(long long int iSearch = iEntry-1;iSearch < fNEntries;iSearch--){
      if(iEntry ==0 || iSearch < 0 || iSearch == iEntry){
	break; //don't want iSearch to go below zero
      }
      chain->GetEntry(iSearch);
      //ddaschannel * serChannel = new ddaschannel;
      serChannel = pDDASEvent->GetData()[0];
      int serChanNo   = GetGlobalChannelNum(serChannel,2);
      double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;

      
      
  
      //cout << nextTDiff << endl;
      lastEntry = iSearch;

      if(abs(nextTDiff) < coinWindow/2){
	channelsCoin.push_back(iSearch);
      }
      else{
	break; //as long as events are time ordered this shouldn't be a problem
      }

    }    

    //now look for coincidence in entries after trigger
    for(long long int iSearch = iEntry+1;iSearch<fNEntries;iSearch++){
      chain->GetEntry(iSearch);
      //ddaschannel * serChannel = new ddaschannel;
      serChannel = pDDASEvent->GetData()[0];
      int serChanNo   = GetGlobalChannelNum(serChannel,2);
      double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
      nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
      lastEntry = iSearch;

      if(abs(nextTDiff) < coinWindow/2){
	channelsCoin.push_back(iSearch);
      }
      else{
	break; //as long as events are time ordered this shouldn't be a problem
      }

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
  
      //Major Issue:
      //It appears that some events are being built where PIN1 is triggered, falls into the PID GATE, but no information from DSSDs are found. I find this to be highly unlikely so there must be some issue with the event builder
      //Once I started looking for events BEFORE the trigger then this problem seems to mostly have gone away

      //loop over events found in Coincidence
      //For DSSD silicon detectors, need to check that front-back energy fired is consistent, are there calibrations for low-gain data?-->I don't believe so

      //for (std::vector<int>::iterator it = channelsCoin.begin() ; it != channelsCoin.end(); ++it){ //in this loop iterator is pointer
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
	  //need to create list of events, with channel fired and energy
	  DSSDevent frontFired;
	  frontFired.energyR = serChannel->GetEnergy();
	  frontFired.strip = 40 - (serChanNo-104); //should look into this mapping at some point 
	  frontFired.time = serChannel->GetCoarseTime();
	  frontFired.channel = serChanNo;
	  frontFired.entry = it;
	  frontEvents.push_back(frontFired);

	  // DSSD Fronts low gain
	  //fImplantEFMaxStrip = 40 - (serChanNo-104); //it appears that sometimes PIN1 fires but there are no DSSD events... seems odd
	  if(serChannel->GetEnergy()>fImplantEFMaxERaw) { //do we always want the max strip? 
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
	  //fImplantEBMaxStrip = 40 - (serChanNo-184);
	  DSSDevent backFired;
	  backFired.energyR = serChannel->GetEnergy();
	  backFired.strip = 40 - (serChanNo-184); //should look into this mapping at some point
	  backFired.time = serChannel->GetCoarseTime();
	  backFired.channel = serChanNo;
	  backFired.entry = it;
	  backEvents.push_back(backFired);
	  


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


	//really need to check that front-back are reporting similar energy to associate the right event


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
      
      if( fGate->IsInside(curTOF,PIN1energy) ){ //once I have routine for checking DSSD events, should require that there are front+back events
	  foundIonOfInterest = true;
	}
      

      channelsCoin.clear();
      
      if(!foundIonOfInterest){ 	//only look at events that are inside gate
	continue;
      }

      cout << endl << "Found Ion!" << endl;

      //loop through DSSD events to see if there are any matching events
      //should implement this above


      //need to vet this method a little more, it seems that the back likes to fire a bunch of times when there is an implantation event, its not clear to me how to assign the events in that case... maybe it's better to just throw those events out. 
      //would be nice to implement an addback method if neighboring channel fires, then perhaps check that E_tot backs = E_tot fronts
      cout << endl << "Found " << frontEvents.size() << " coincident front DSSD events" << endl;
      cout << "Found " << backEvents.size() << " coincident back DSSD events" << endl;

      // unsigned int frontEtot = 0.;
      // unsigned int backEtot = 0.;
      //to create an addback routine, should keep one strip fixed (say one front) and add all backs and see if energy matches
      //then repeat algorithm if they don't match (outside some toleance) and try to reach some convergence
      //at the very least should assign strip with the largest reported energy, which is already being done for implantation

      //going to take events with the shortest time between them
      double TdiffMin = coinWindow;  //maximum value, so long as coincidence events are the only ones passed into DSSDevent vectors
      for(auto &front : frontEvents){
 
	for(auto &back : backEvents){
	  unsigned int Ediff = abs(front.energyR - back.energyR); //when dealing with unsigned ints, negative numbers get wrapped around to largest numbers
	                                                        //This was causing problems when trying to determine Ediff for decay events
	  if(front.energyR > back.energyR) {Ediff = front.energyR - back.energyR;}
	  else {Ediff = back.energyR - front.energyR;}

	  double Tdiff = abs(front.time - back.time);
	  //if(Ediff < 1000){
	  //if(Tdiff<TdiffMin){ //I believe should use this determination if for assigning strip, along with maxE
	    cout << endl;
	    cout << "Found coincident DSSDevent" << endl;
	    cout << "Tdiff = " << Tdiff << endl;
	    cout << "Ediff = " << Ediff << endl;
	    cout << "FrontE = " << front.energyR << endl;
	    cout << "BackE = " << back.energyR << endl;
	    cout << "Front strip = " << front.strip << endl;
	    cout << "Front channel = " << front.channel << endl;
	    cout << "Back strip = " << back.strip << endl;
	    cout << "Back channel = " << back.channel << endl;
	    TdiffMin = Tdiff; //presumably the strips that fire closest in time should be the implantation strips-->or max energy 
	    //}
	    //}


	}

      }
      

      backEvents.clear();
      frontEvents.clear();

      cout << endl;
      cout << "fImplantEBMaxERaw = " << fImplantEBMaxERaw << endl;
      cout << "fImplantEFMaxERaw = " << fImplantEFMaxERaw << endl;

     
      //same issue with unsigned ints below... UGH!

      // if( abs(fImplantEBMaxERaw - fImplantEFMaxERaw) > 1000 && fImplantEBMaxERaw > 0 && fImplantEFMaxERaw > 0){
      // 	cout << endl << "May be issue with assigning implantation events" << endl; //this seems to appear frequently... 
      // }

      


      //now look for decay correlations

      //no need to look at past events for correlations

      //now look for correlations in entries after trigger

      //should include some to make sure that we aren't associating decay events with the wrong trigger
      for(long long int iSearch = iEntry+1;iSearch<fNEntries;iSearch++){
	chain->GetEntry(iSearch);
	//ddaschannel * serChannel = new ddaschannel;
	serChannel = pDDASEvent->GetData()[0];
	int serChanNo   = GetGlobalChannelNum(serChannel,2);
	double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
	nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
	//lastEntry = iSearch;

	if(nextTDiff < corrWindow && nextTDiff > coinWindow/2. && nextTDiff >0){
	  //For some reason adding && nextTDiff > 100 broke this...  because it was breaking the loop instantly dummy
	  //should wait a little bit before actually looking for decays (to let electronics come to steady state)
	  //not sure how long the waiting should actually be, although I think waiting outside the the coincidence window is a good idea
	  channelsCorr.push_back(iSearch);
	}
	else if(nextTDiff < coinWindow/2.){
	  continue;
	}
	else if(nextTDiff > corrWindow || serChanNo == trigChan){//also want to exclude "correlations" if there is another implantation event
	                                                         //I wonder if there is a way to untangle ambiguity of two implantation events

	  if(serChanNo == trigChan){lastEntry = iSearch;} //move to the next trigger if found
	  break; //as long as events are time ordered this shouldn't be a problem, should confirm time-ordered property
	}
      }

      cout << endl << "Found " <<  channelsCorr.size() << " correlations with Ion implantation event" << endl;


      int stripCheckBack = -3;
      int stripCheckFront = -3;
      unsigned int energyCheckFront = 0;
      unsigned int energyCheckBack = 0;
      double timeCheckFront = 0;
      double timeCheckBack = 0;
      nCh = 0;
      for(auto &it : channelsCorr){
	chain->GetEntry(it);
	//ddaschannel * serChannel = new ddaschannel;
	serChannel = pDDASEvent->GetData()[0];
	int serChanNo   = GetGlobalChannelNum(serChannel,2);

	if(serChanNo >= 64 && serChanNo <= 103){
	  // DSSD Fronts high gain

	  DSSDevent frontFired;
	  frontFired.energyR = serChannel->GetEnergy();
	  frontFired.strip = serChanNo-64;
	  frontFired.time = serChannel->GetCoarseTime();
	  frontFired.channel = serChanNo;
	  frontFired.entry = it;
	  frontEvents.push_back(frontFired);


	  //cout << frontFired.energy << endl;

	  if(serChannel->GetEnergy()>fDecayEFMaxERaw) { //do we want the max energy for decay? Would it be more prudent to read out if strips are consistent?
	    fDecayEFMaxERaw = serChannel->GetEnergy();
	    // //        fImplantEFMaxStrip = serChanNo-104;
	    fDecayEFMaxStrip = serChanNo - 64;
	  }
	  nCh++;
	  decaytime = (double)serChannel->GetCoarseTime();
	}
	else if(serChanNo >= 144 && serChanNo <= 183){
	  // DSSD Backs high gain
	  DSSDevent backFired;
	  backFired.energyR = serChannel->GetEnergy();
	  backFired.strip = serChanNo-144;
	  backFired.time = serChannel->GetCoarseTime();
	  backFired.channel = serChanNo;
	  backFired.entry = it;
	  backEvents.push_back(backFired);
	  




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


      //loop over DSSD events found in correlations
      cout << endl << "Found " << frontEvents.size() << " correlated front DSSD events" << endl;
      cout << "Found " << backEvents.size() << " correlated back DSSD events" << endl;

      DSSDevent decayEventFront;
      DSSDevent decayEventBack;
      double EdiffMin = 1000;
      double EdiffThreshold = 200; //keV
      int stripTolerance = 2;
      double Emax = 0;
      TdiffMin = coinWindow;
      bool foundPotDecay = false;

      
      //I could really use a vector of "valid" DSSD events... I guess I should go ahead with abstraction      
      
      //for now going to take 'max' energy event in order to filter only proton events

      for(auto &front : frontEvents){

	double Tdiff = coinWindow; //maximum value, so long as coincidence events are the only ones passed into DSSDevent vectors

	for(auto &back : backEvents){

	  TF1 *fDSSDbackCal = (TF1*)fDSSDCalibFunc->At(back.channel-104);
	  TF1 *fDSSDfrontCal =(TF1*)fDSSDCalibFunc->At(front.channel-64);
	  
	  back.energy = fDSSDbackCal->Eval(back.energyR);
	  front.energy = fDSSDfrontCal->Eval(front.energyR);

	  double Ediff = abs(front.energy - back.energy);
	  double time = back.time - curTime;

	  if(front.energy > back.energy) {Ediff = front.energy - back.energy;}
	  else {Ediff = back.energy - front.energy;}

	  Tdiff = abs(front.time - back.time);
	  if(Ediff < EdiffThreshold && back.energy > 0 && front.energy > 0 && Tdiff < coinWindow && abs(fImplantEFMaxStrip - front.strip) < stripTolerance && abs(fImplantEBMaxStrip - back.strip) < stripTolerance){
	    cout << endl;
	    cout << "Found potential decay event" << endl;
	    cout << "Tdiff = " << Tdiff << endl;
	    cout << "Ediff = " << Ediff << endl;
	    cout << "FrontE = " << front.energy << endl;
	    cout << "BackE = " << back.energy << endl;
	    cout << "Front strip = " << front.strip << endl;
	    cout << "Front channel = " << front.channel << endl;
	    cout << "Back strip = " << back.strip << endl;
	    cout << "Back channel = " << back.channel << endl;
	    cout << "Recorded decay time = " << time << endl; 


	    //if(Ediff < EdiffMin && Tdiff < TdiffMin){ // will eventually need a better criterion, as there may be multiple "correlated" events that match above condition

	    //have to take multiple decay events or I will miss a lot...
	    //also this method will make a large difference into what you will see as decays... tread carefully

	    //if(Tdiff < TdiffMin){ //going to take minimum time, like previously algorithm
	                          //need to start consolidating these methods into classes...
	    if(back.energy > Emax){
	      decayEventFront.energy = front.energy;
	      decayEventFront.strip = front.strip;
	      decayEventFront.time = front.time;
	      decayEventFront.entry = front.entry;

	      decayEventBack.energy = back.energy;
	      decayEventBack.strip = back.strip;
	      decayEventBack.time = back.time;
	      decayEventBack.entry = back.entry;

	      Emax = back.energy;
	      EdiffMin = Ediff;
	      TdiffMin = Tdiff;
	      foundPotDecay = true;
	      //}
	      //}
	    }

	  }

	}

      } //end loop over correlated DSSD events
      

      // large amount of channels firing with small amounts of energy may be beta-particles being detected
      // should report ALL valid DSSD events, since there may be some beta+proton evets, this seems to be the case for some events
      // if they are beta+proton events then the events should have some time correlation->half-life of implant child nucleus

      if(foundPotDecay){
	Histo->hDecayXY->Fill(fImplantEBMaxStrip-20,(-1*(decayEventFront.strip-39))-20);
	//TF1 *fDSSDHighCal = (TF1*)fDSSDCalibFunc->At(fDecayEBMaxStrip);
	
	//cout << "DSSDEvalue = " << DSSDEvalue << endl;
	cout << endl << "Reported E = " << decayEventFront.energy << endl;
	Histo->hDecayEnergy->Fill(decayEventFront.energy);
	double time = decayEventFront.time -curTime;
	Histo->hDecayTime->Fill(time);
	if(time < 3E8){
	  Histo->hDecayEnergyTGate->Fill(decayEventFront.energy);
	}

	long long int decayEntry = decayEventFront.entry;

	//now look for gamma coincidences
	for(long long int iSearch = decayEntry-1;iSearch < fNEntries;iSearch--){
	  if(iEntry ==0 || iSearch < 0 || iSearch == iEntry){
	    break; //don't want iSearch to go below zero
	  }
	  chain->GetEntry(iSearch);
	  //ddaschannel * serChannel = new ddaschannel;
	  serChannel = pDDASEvent->GetData()[0];
	  int serChanNo   = GetGlobalChannelNum(serChannel,2);
	  double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
	  nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;

      
      
  
	  //cout << nextTDiff << endl;
	  //lastEntry = iSearch;

	  if(abs(nextTDiff) < coinWindow/2){
	    //channelsCoin.push_back(iSearch);
	    for(int i=0;i<16;i++){
	      SEGA[i].fillEvent(serChannel,pDDASEvent);
	    }

	  }
	  else{
	    break; //as long as events are time ordered this shouldn't be a problem
	  }

	}    


	for(long long int iSearch = decayEntry+1;iSearch<fNEntries;iSearch++){
	  chain->GetEntry(iSearch);
	  //ddaschannel * serChannel = new ddaschannel;
	  serChannel = pDDASEvent->GetData()[0];
	  int serChanNo   = GetGlobalChannelNum(serChannel,2);
	  double delay = fh_ChannelDelays->GetBinContent(serChanNo+1);  // ---=== UNCOMMENT FOR DELAY ===---
	  nextTDiff  = (serChannel->GetCoarseTime()-delay) - curTime;
	  //lastEntry = iSearch;



	  if(abs(nextTDiff) < coinWindow/2){
	    //channelsCoin.push_back(iSearch);
	    for(int i=0;i<16;i++){

	      if( SEGA[i].fillEvent(serChannel,pDDASEvent) ){
		cout << "Found SEGA event in coincidence with decay!" << endl;
	      }

	    }

	  }
	  else{
	    break; //as long as events are time ordered this shouldn't be a problem
	  }

	}

	//now fill histos with SEGA events found
	for(int i=0;i<16;i++){

	  if(SEGA[i].getEvents().size() >0){
	    for(auto &event : SEGA[i].getEvents()){
	      Histo->hGammaEnergy->Fill(event.energy);
	      Histo->hSeGAEnergy->Fill(event.channel-16,event.energy);
	    }
	  }

	}


      } //end ifPotDecay

      //these do need to be cleared since called outside of scope of entry loop
      for(int i=0;i<16;i++){
	SEGA[i].clear();
      }

      //technically shouldn't need these clears since vectors were initialized in loop over entries
      channelsCorr.clear();
      backEvents.clear();
      frontEvents.clear();


      //could fill a tree with only DDASEvents that comprise of specific isotope
      //could make class that does this so long as there is a specified PID file and PID Gate file

  } //end of loop over entries

  //cout << endl << "made it out of loop of events" << endl;

  Histo->write(); // this forces the histrograms to be read out to file

  return 1;
}
