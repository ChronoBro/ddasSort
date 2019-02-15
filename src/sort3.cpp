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
#include "diagnostics.h"

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
  else{
    cerr << "./sort3 firstRun# lastRun# outputFilename(no '.root')" << endl;
    abort();
  }


  histo * Histo = new histo(outRootfile.str());

  int maxSubRun = 9;

  TChain dataChain("dchan");

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

      
      dataChain.AddFile(infile.str().c_str());
      cout << "added " << infile.str() << " to TChain                           " << endl; //have to use carriage return for flush

      }
      else{
	////cout << "Couldn't find file " << infile.str() << "                        " << endl; 
	break;
      }
    } 

  }

  ////cout << "adding " << infile.str() << " to TChain..." << endl;
  
  cout << endl;

  long long int fNEntries = dataChain.GetEntries(); //64bit int, because I was using int for the entries I wasn't able to access long lists, hence sort failing for a long list of data files

  cout << "Number of entries in TChain = " << fNEntries << endl;
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
  vector<int> frontChannels;
  vector<int> backChannels;
  vector< vector<double> > frontCalib;
  vector< vector<double> > backCalib;


  vector<ddasDet> SSDhiGain;
  for(int i=32;i<48;i++){
    ddasDet fillDet(i);
    SSDhiGain.push_back(fillDet);
  }



  //DSSD is 300 micron thick?
  vector<ddasDet> DSSDfrontHighGain;
  vector<ddasDet> DSSDbackHighGain;
  
  vector<ddasDet> DSSDfrontLoGain;
  vector<ddasDet> DSSDbackLoGain;



  for(Int_t i=0; i<80; i++){
    sprintf(name,"f_cal_%02i_sp",i);
    int chan=0.;
    double slope;
    double offset;
    alphaCalib >> chan >> offset >> slope; 

    ddasDet fillDet(chan);
    ddasDet fillDet2(chan+40);
    vector<double> params;
    params.push_back(offset);
    params.push_back(slope);
    fillDet.setCalibration(params);
    fillDet2.setCalibration(params);
    //DSSDfrontHighGain.push_back(fillDet);
    vector<double> paramsFront;
    vector<double> paramsBack;
    
    if(i<40){
      frontChannels.push_back(chan);paramsFront.push_back(offset);paramsFront.push_back(slope);frontCalib.push_back(paramsFront);
      DSSDfrontHighGain.push_back(fillDet); DSSDfrontLoGain.push_back(fillDet2);
    }
    if(i>=40){backChannels.push_back(chan);paramsBack.push_back(offset);paramsBack.push_back(slope);backCalib.push_back(paramsBack);
      DSSDbackHighGain.push_back(fillDet); DSSDbackLoGain.push_back(fillDet2);
    }

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
  vector<ddasDet> SEGA;
  fSeGACalFile = new TFile("Calibrations/SeGACalibrations.root","READ");
  vector<int> SEGAchannels;
  vector< vector<double> > SEGAparamList;
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    TF1 *fCalG = new TF1(name,"pol1",0,35000);

    SEGAchannels.push_back(SeGAchannel);
    
    ddasDet fillerDet(SeGAchannel);
    

    vector<double> params;
    params.push_back(funG->GetParameter(0));
    params.push_back(funG->GetParameter(1));
    SEGAparamList.push_back(params);
    fillerDet.setCalibration(params);

    SEGA.push_back(fillerDet);

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
    fh_ChannelDelays = new TH1D("h_Delays","",300,0,300); // <--- could add timeOffset to ddasDet and load in delay with detectors
  }
  
  // Load PID gate

  TFile *fGateFile = new TFile("PIDGates2.root","READ");
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Rb")); 
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Kr")); 
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_70Kr")); 

  cout << "--> Loaded Gate: " << fGate->GetName() << endl << endl;
  fGateFile->Close();


  //creating diagnostics class for counting...
  diagnostics counterList;
  counterList.add("foundIon");
  counterList.add("decays");
  counterList.add("lostIonNoImplantation");
  counterList.add("lostIonNoDecay");
  counterList.add("lostIonSecondImplant");


  /*****************************************
   
   LOOP OVER DDAS EVENTS IN TCHAIN

   ****************************************/

  cerr << " Processing Events..." << endl; 



  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  //std::vector<ddaschannel*> pChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  ddaschannel * curChannel = new ddaschannel;
  ddaschannel * serChannel =new ddaschannel; //If you call new ddaschannels in a loop it will cause a memory leak 
                                             //--> Comes from TTree.SetBranchAddress() I suspect. 


  dataChain.SetBranchAddress("ddasevent", &pDDASEvent);
  long long int lastEntry = 0;  //keep track of the last entry accessed in the branch

  //use this to keep a list of events in coincidence with trigger
  //only fillEvent(ddaschannal*, DDASEVent *) with this object to get getEntry() count low
  ddasDet buffer(-1);


  //below would be used for filling a tree with only events involving ION of interest
  //Would also need to include "correlated" events as well
  //Should cut down on processing time tremendously if done

  // TFile * outFile = new TFile("ions.root","recreate");
  // TTree * oTree = new TTree("dchan","list of ions of interest");
  // DDASEvent * ionEvent = new DDASEvent;
  // //std::vector<ddaschannel*> ionChannels;  //! vector of channels for a given DDASEvent, if regular built this will be useful
  // oTree->Branch("ddasEvent",&ionEvent);

  // With below method working for finding events, should further abstract this so I can add different triggers and such

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry+1){

    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //each DDASEvent has 1 ddaschannel

    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    int trigChan =0; //channel for PIN1 at front of stack
    ddasDet trigger(trigChan);
    ddasDet TOF(5); // PIN2-XFP TAC
    ddasDet PIN2(1); //PIN2 Energy
    
    double coinWindow     =  2000;//5000;//2000;        // Time (ns)
    double promptWindow   =  1000;
    double coinGammaWindow=  1000;        // Time (ns)
    double corrWindow     =  1E9;        // Time (ns)  now 1000ms (1s)
    double waitWindow     =  1E5; // 0.1 ms

    bool foundIonOfInterest = false;
    bool foundImplant = false;
    bool foundTrigger = false;
    bool foundTOF = false;


    int fImplantEFMaxStrip = -100;
    int fImplantEBMaxStrip = -100;
    double decaytime = 0;


    lastEntry = iEntry;
   
    double progress0 =  (double)iEntry/(double)dataChain.GetEntries();
    if(iEntry % 10000 == 0){   
      loadBar(progress0);    
    }   
    
    dataChain.GetEntry(iEntry);
    buffer.fillEvent(curChannel, pDDASEvent);
    foundTrigger = trigger.fillEvent(buffer.getFillerEvent());

    //loop through buffer and pop events off the end until all events are in coincidence window
    if(buffer.getEvents().size() > 1){
      for(auto & bufferEvent : buffer.getEvents() ){
	if(abs(buffer.getFillerEvent().time - bufferEvent.time) < coinWindow/2.){break;}
	else{buffer.pop();};
      }
    }

    //if trigger isn't found continue;
    if(!foundTrigger){continue;}

    double PIN1energy = trigger.getFillerEvent().signal;
    double triggerTime = trigger.getFillerEvent().time;

    //below a trigger was found
    
    //fill up buffer with events in coincidence with trigger
    for(; abs(buffer.getFillerEvent().time - triggerTime) < coinWindow/2. ;){
      if(lastEntry >= dataChain.GetEntries()){break;}
      dataChain.GetEntry(lastEntry++);
      buffer.fillEvent(curChannel, pDDASEvent);
   
      progress0 =  (double)lastEntry/(double)dataChain.GetEntries();
      if(lastEntry % 10000 == 0){   
	loadBar(progress0);    
      }   
   
    }

    bool checkBack = false;
    bool checkFront = false;
    bool checkSEGA = false;

    double curTOF = 0;
    double PIN2Energy = 0;
    //dump buffer into detectors
    for(auto & bufferEvent : buffer.getEvents()){

      foundTOF = TOF.fillEvent(bufferEvent);

      if(foundTOF){
	if(TOF.getFillerEvent().signal > curTOF){curTOF = TOF.getFillerEvent().signal;}
	//cout << "FoundTOF" << endl;
      }

      if(PIN2.fillEvent(bufferEvent)){
	if(PIN2.getFillerEvent().signal > PIN2Energy){PIN2Energy = PIN2.getFillerEvent().signal;}
      }
      
      for(auto & DSSDfront : DSSDfrontLoGain){
	if(DSSDfront.fillEvent(bufferEvent)){checkFront = true;}
	//cout << "found front" << endl;
      }

      for(auto & DSSDback : DSSDbackLoGain){
	if(DSSDback.fillEvent(bufferEvent)){checkBack = true;}
	//cout << "found back" << endl;
      }

      for(auto & SEGAdet : SEGA){
	if(SEGAdet.fillEvent(bufferEvent)){checkSEGA=true;}
      }

    }


    //curTOF = TOF.getFillerEvent().signal;
    Histo->h_PID->Fill(curTOF, PIN1energy);
    Histo->h_PIN1vsPIN2->Fill(PIN2Energy, PIN1energy);

    if( fGate->IsInside(curTOF,PIN1energy) ){//&& checkBack && checkFront && checkTOF){ 
                                              //once I have routine for checking DSSD events, should require that there are front+back events
                                              // DONE
      foundIonOfInterest = true;              //--->can also add other gates to find other ions
      counterList.count("foundIon");

      //only want to read out prompt gammaes if gated on ion

      if(checkSEGA){ //readout prompt gammas
	for(auto & SEGAdet : SEGA){
	
	  for(auto segaEvent : SEGAdet.getEvents()){
	    Histo->hPromptGammaEnergy->Fill(segaEvent.energy);
	  }

	}
      }

      //andy wants me to read out traces for DSSD implantation events

    }


    //take strip with highest energy deposition as implant strip

    unsigned int Emax = 0;
    for(auto & DSSDfront : DSSDfrontLoGain){
      
      for(auto frontEvent : DSSDfront.getEvents()){
	if(frontEvent.signal > Emax){
	  Emax = frontEvent.signal;
	  fImplantEFMaxStrip =  40 - (frontEvent.channel - 104);
	}
      }

    }
    
    Emax=0;
    for(auto & backs : DSSDbackLoGain){	  
      for(auto& backEvent : backs.getEvents()){

	if(backEvent.signal > Emax){
	  Emax = backEvent.signal;
	  fImplantEBMaxStrip = 40 - (backEvent.channel - 184);
	}

      }	
    }


    //clear event lists for stuff already used
    for(auto & fronts : DSSDfrontLoGain){
      fronts.clear();
    }

    for(auto & backs : DSSDbackLoGain){
      backs.clear();
    }

    for(auto segaDet : SEGA){
      segaDet.clear();
    }

    //only take events where the implantation event was found
    if(!foundIonOfInterest){continue;}

    if(fImplantEFMaxStrip == -100 || fImplantEBMaxStrip == -100){counterList.count("lostIonNoImplantation");} 

    //cout << "Front Implant Strip = " << fImplantEFMaxStrip << endl;
    //cout << "Back Implant Strip = " << fImplantEBMaxStrip << endl;


    //setting a waiting time to cut out short-lived products
    for(; abs(buffer.getFillerEvent().time - triggerTime) < waitWindow ;){
      if(lastEntry >= dataChain.GetEntries()){break;}
      dataChain.GetEntry(lastEntry++);
      buffer.fillEvent(curChannel, pDDASEvent);
   
      progress0 =  (double)lastEntry/(double)dataChain.GetEntries();
      if(lastEntry % 10000 == 0){   
	loadBar(progress0);    
      }   
   
    }

    //clear buffer to trigger on DSSD events
    buffer.clear();



    //now look for correlations
    do{


      
      bool foundDecay = false;
      bool foundDSSD = false;
      bool foundDSSDfront = false;
      bool foundDSSDback = false;
      bool secondImplant = false;
      double DSSDtime = 0; //want to set trigger time of DSSD
      double SSDtime = 0;
      bool foundSSD = false;

      if(lastEntry >= dataChain.GetEntries()){break;}
      dataChain.GetEntry(lastEntry++);
      buffer.fillEvent(curChannel, pDDASEvent);
      if(trigger.fillEvent(buffer.getFillerEvent())){secondImplant=true;}

      progress0 =  (double)lastEntry/(double)dataChain.GetEntries();
      if(lastEntry % 10000 == 0){   
	loadBar(progress0);    
      }   

      //loop through buffer and pop events off the end until all events are in coincidence window
      for(auto & bufferEvent : buffer.getEvents() ){
	if(abs(buffer.getFillerEvent().time - bufferEvent.time) > coinWindow/2.){buffer.pop();}
	else{break;}
      }
      

      
      for(auto & fronts : DSSDfrontHighGain){
	if(fronts.fillEvent(buffer.getFillerEvent())){
	  DSSDtime = buffer.getFillerEvent().time;
	  foundDSSDfront = true;
	}
      }

      for(auto & backs : DSSDbackHighGain){
	if(backs.fillEvent(buffer.getFillerEvent())){
	  DSSDtime = buffer.getFillerEvent().time;
	  foundDSSDback = true;	  
	}
      }

      //foundDSSD = foundDSSDfront && foundDSSDback; //BAAAD idea since finding trigger...
      foundDSSD = foundDSSDfront || foundDSSDback;

      if(!foundDSSD){continue;} // keep searching until a DSSD is found

     

      //fill up buffer with events in coincidence with trigger
      for(; abs(buffer.getFillerEvent().time - DSSDtime) < coinWindow/2. ;){
	if(lastEntry >= dataChain.GetEntries()){break;}
	dataChain.GetEntry(lastEntry++);
	buffer.fillEvent(curChannel, pDDASEvent);
	if(trigger.fillEvent(buffer.getFillerEvent()) ){secondImplant=true;}
	  
	progress0 =  (double)lastEntry/(double)dataChain.GetEntries();
	if(lastEntry % 10000 == 0){   
	  loadBar(progress0);    
	}   
   
      }


      if(secondImplant){counterList.count("lostIonSecondImplant");break;}

      //cout << "found DSSD" << endl;
      //cout << buffer.getEvents().size() << endl;

      ddasDet decayEventsFront(-1);
      ddasDet decayEventsBack(-1);
      ddasDet segaEvents(-1);
      ddasDet SSDEvents(-1);

      ddasDet Scint(15); //Scintillator at back of stack?

      //dump buffer into detectors
      for(auto & bufferEvent : buffer.getEvents()){

	Scint.fillEvent(bufferEvent);

	for(auto & DSSDfront : DSSDfrontHighGain){
	  if(DSSDfront.fillEvent(bufferEvent)){ //event gets calibrated when filled
	    decayEventsFront.fillEvent(DSSDfront.getFillerEvent()); // if filled with bufferEvent, energy=-1

	  }	    
	}

	for(auto & DSSDback : DSSDbackHighGain){
	  if(DSSDback.fillEvent(bufferEvent)){
	    decayEventsBack.fillEvent(DSSDback.getFillerEvent());

	  }
	}

	for(auto & SEGAdet : SEGA){
	  if(SEGAdet.fillEvent(bufferEvent)){
	    segaEvents.fillEvent(SEGAdet.getFillerEvent());
	  }
	}

	for(auto & SSDstrip : SSDhiGain){
	  if(SSDstrip.fillEvent(bufferEvent)){
	    SSDEvents.fillEvent(SSDstrip.getFillerEvent());
	  }
	}

      }

      //cout << decayEventsFront.getEvents().size() << endl;
      //cout << decayEventsBack.getEvents().size() << endl;

      //criterion for tagging as decay
      double EdiffThreshold = 200; //keV <---perhaps using a ratio of Ediff to total E is a better criterion, essentially %err of the measurement

      double EdiffPercentThreshold = 0.15; //<--- This cut out almost ALL of the low energy 'beta' events
                                          // creating a 0.1 ms wait window did not cut out more events, so I believe this has more of an effect
                                          // on spurious events
                                          // Long wait window (0.1ms) and removal of this added a lot more low energy events

      double Ethreshold = 100; //keV <--- setting threshold has a large impact on decays seen, likely from large number of background betas
                               // 100 keV cuts out very low energy noise ~98 keV
      int stripTolerance = 2;


      double decayTime = corrWindow;
      double decayEnergy = 0;
      double EmaxDecay = 0;
      double reportedE = 0;
      double reportedTime = 0;
      int decayChannel = -1;

      //need to create better scheme for adding events back together, should only add if they have front+back coincidence, and are within a certain pixel range of implantation event



      ddasDet decayEvents(-1);

      for(auto & decayEventFront : decayEventsFront.getEvents()){

	int frontStrip = decayEventFront.channel - 64;
	decayEnergy = decayEventFront.energy;
	decayTime = decayEventFront.time - triggerTime;//trigger.getFillerEvent().time; <--- bad idea since I check for second implant 
	                                               //with trigger.fillEvent(buffer.getFillerEvent()); 

	Histo->h_mult_F->Fill(frontStrip);

	//cout << "front strip = " << frontStrip << endl;
	//cout << "Front E = " << decayEnergy << endl;
	//cout << "decay time = " << decayTime << endl;

	for(auto & decayEventBack : decayEventsBack.getEvents()){
	    
	  //double Tdiff = abs(decayEventFront.time - decayEventBack.time);
	  double Ediff = abs(decayEventFront.energy - decayEventBack.energy);
	  int backStrip = decayEventBack.channel - 144;
	    
	  double EdiffPercent = abs(Ediff/decayEnergy);

	  Histo->h_mult_B->Fill(backStrip);

	  //cout << "back strip = " << backStrip << endl;
	  //cout << "Back E = " << decayEventBack.energy << endl;

	  if(Ediff < EdiffThreshold
	     && EdiffPercent < EdiffPercentThreshold
	     && decayEventBack.energy > Ethreshold 
	     && decayEventFront.energy > Ethreshold 
	     && abs(fImplantEFMaxStrip - frontStrip) < stripTolerance 
	     && abs(fImplantEBMaxStrip - backStrip) < stripTolerance 
	     //&& decayEnergy > EmaxDecay
	     ){

	    decayEvents.fillEvent(decayEventFront); //<--- could read out all "valid" DSSD events found in decay
	    if(decayEnergy > EmaxDecay){
	      reportedE = decayEnergy;
	      reportedTime = decayTime;
	      decayChannel = decayEventFront.channel;
	      EmaxDecay = decayEnergy;
	    }
	    foundDecay = true;

	  }
	    

	}
      }



      int maxNtraces = 200;
      int nTraces = 0;
      if(foundDecay){
	counterList.count("decays");
	//cout << "Found Decay Event!" << endl;
	//dump gamma events

	//if(decayChannel == 67 || decayChannel == 78 || decayChannel == 79){break;} //exclude bad channels
	                                                                             //shouldn't need above with %E_error check

	if(decayTime < 2E8){Histo->hDecayEnergyTGate->Fill(reportedE);} //events found in first 200 ms

	double Etot = 0;
	//dump out all decay events within coinWindow
	for(auto & decayEvent : decayEvents.getEvents()){
	  Histo->hDecayEnergyAll->Fill(decayEvent.energy);
	  
	  Etot+= decayEvent.energy; //now Etot should only be comprised of events that pass my valid event filter above


	  if(decayEvent.energy < 6000 && nTraces < maxNtraces){
	    int size = decayEvent.trace.size();
	    double x[size];
	    double y[size];
	    ostringstream title;
	    title << "Trace_E-" << decayEvent.energy << "_Event-" << counterList.returnValue("decays") << "_Tree-" << dataChain.GetTreeNumber();

	    for(int i=0;i<size;i++){
	      x[i] = i;
	      y[i] = decayEvent.trace[i];
	    }

	    //TH1S * h = new TH1S(title.str().c_str(),title.str().c_str(),size,&decayEvent.trace[0]);
	    TGraph * gr = new TGraph(size,x,y);
	    gr->SetName(title.str().c_str());
	    //Histo->dirTraces->cd();
	    Histo->graphTraces.push_back(gr);
	    //delete gr; //need to keep TGraph in memory for ROOT to pickup TGraph when writing to file
	    nTraces++;
	  }

	  for(auto & segaEvent : segaEvents.getEvents()){
	    //Histo->hGammaEnergy->Fill(segaEvent.energy);
	    Histo->hGammaVsDecayAll->Fill(decayEvent.energy, segaEvent.energy);
	  }
	  
	}
	


	for(auto & segaEvent : segaEvents.getEvents()){
	  Histo->hGammaEnergy->Fill(segaEvent.energy);
	  Histo->hGammaVsDecay->Fill(reportedE,segaEvent.energy);
	  Histo->hGammaVsDecayEtot->Fill(Etot,segaEvent.energy);
	  if( abs(segaEvent.time - DSSDtime) < promptWindow/2. ){Histo->hGammaVsDecayTGated->Fill(reportedE,segaEvent.energy);}
	  // if(segaEvent.energy > 9000){
	  //   cout << "Found large energy SEGA event" << endl; //diagnostics of high energy events
	  //   cout << segaEvent.channel << endl;               //its possible these events are underflow
	  //                                                    //it appears these events only occur for channel 31, so they are likely unphysical
	  // }
	}

	// if(decayEnergy > 7500){
	//   cout << "Found large energy decay event" << endl;
	//   cout << decayChannel << endl;
	// } //diagnostics of higher energy events
	  //its possible these events come from "bad" channels
	  
	Histo->hDecayEnergy->Fill(reportedE);
	Histo->hDecayEnergyTot->Fill(Etot);
	Histo->hDecayTime->Fill(reportedTime);

	for(auto & scintEvent : Scint.getEvents()){
	  Histo->rawScint->Fill(scintEvent.signal);
	}


	for(auto & SSDEvent : SSDEvents.getEvents()){
	  Histo->rawSSDhiGain->Fill(SSDEvent.signal);
	}
	
	break;
      }



    } while(abs( buffer.getFillerEvent().time - triggerTime) < corrWindow ); //only do second search over correlation window

   
    //clear detectors that are called outside of event loop

    for(auto & fronts : DSSDfrontHighGain){
      fronts.clear();
    }

    for(auto & backs : DSSDbackHighGain){
      backs.clear();
    }

    for(auto segaDet : SEGA){
      segaDet.clear();
    }
    
    for(auto SSDstrip : SSDhiGain){
      SSDstrip.clear();
    }
    

    //could fill a tree with only DDASEvents that comprise of specific isotope
    //could make class that does this so long as there is a specified PID file and PID Gate file

  } //end of loop over entries

  Histo->write(); // this forces the histrograms to be read out to file

  cerr << "Total number of Ions of Interest found: " << counterList.returnValue("foundIon") << endl;
  cerr << "Total number of decays found: " << counterList.returnValue("decays") << endl;
  cerr << "Total number of Ions lost in implantation: " << counterList.returnValue("lostIonNoImplantation") << endl;
  cerr << "Total number of Ions lost in decay: " << counterList.returnValue("foundIon") - counterList.returnValue("decays") - counterList.returnValue("lostIonNoImplantation") << endl;
  cerr << "Decays lost to second Implantation: " << counterList.returnValue("lostIonSecondImplant") << endl;
  cerr << endl;

  return 1;
}
