#include "DDASEvent.h" 
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
#include "diagnostics.h"
#include "RBDDTriggeredEvent.h"
#include "RBDDASChannel.h"
#include "RBDDarray.h"
#include "RBDDTrace.h"
#include <iomanip>
#include <chrono>
#include <ctime>
#include "ionCorrelator.h"
#include "config.h"

using namespace std;


//not sure why... but this seems to be missing tons of stuff when I have this active 9/16/2019
//may have been an issue with buffering which should be fixed now 9/21/2019
//this condition was used to reproduce 73Sr data so it works fine 9/22/2019
bool triggerCondition(int Chan){

  //return true;

  if(Chan >= 64 && Chan < 104) {
    return true;
  }
  else if(Chan >= 144 && Chan < 184){
    return true;
  }
  else{
    return false;
  }
}

int main(int argc,char *argv[]){

  auto start = std::chrono::system_clock::now();

  cerr << endl << "------------------------------------";
  cerr << endl << "NSCL DDAS sorting for unbuilt Events" << endl;
  cerr <<         "------------------------------------" << endl << endl;



  int runStart=99;
  int runEnd=99;
  ostringstream outRootfile;
  ostringstream configFile;

  if(argc==3){
    string s_configFile(argv[1]);
    configFile << "config/" << s_configFile << ".toml";
    runStart = atoi(argv[2]);
    runEnd=runStart;
    outRootfile << "run_" << runStart << "_processed.root";
  }
  else if(argc==4){
    string s_configFile(argv[1]);
    configFile << "config/" << s_configFile << ".toml";
    runStart = atoi(argv[2]);
    runEnd = atoi(argv[3]);
    outRootfile << "run_" << runStart << "-" << runEnd << "_processed.root";
  }
  else if(argc==5){
    string s_configFile(argv[1]);
    configFile << "config/" << s_configFile << ".toml";
    runStart = atoi(argv[2]);
    runEnd = atoi(argv[3]);
    string s_outRootfile(argv[4]);
    outRootfile << s_outRootfile << ".root";
  }
  else{
    cerr << "./sort5 configFile(no '.toml') firstRun# lastRun# outputFilename(no '.root')" << endl;
    abort();
  }

  TChain dataChain("dchan"); //dchan is name of tree is ddasdumped files

  //reading in config file
  //because TChain doesn't have a copy function set I havee to feed it into config contructor
  config setup(configFile.str(),runStart,runEnd, dataChain);

  //creating histogram class that keeps track of histograms being written to file
  histo * Histo = new histo(outRootfile.str());

  int maxSubRun = 9;


  
  cout << endl;

  long long int fNEntries = dataChain.GetEntries(); //64bit int, because I was using int for the entries I wasn't able to access long lists, hence sort failing for a long list of data files

  cout << "Number of entries in TChain = " << fNEntries << endl;
  cout << endl;



  /*****************************************
   
   DECLARING DETECTOR OBJECTS FOR USE BELOW

   ****************************************/

  RBDDarray SSD  = setup.getArray("SSD");

  RBDDarray DSSDloGainFront = setup.getArray("DSSDloGainFront");
  RBDDarray DSSDloGainBack = setup.getArray("DSSDloGainBack");
  
  RBDDarray DSSDhiGainFront = setup.getArray("DSSDhiGainFront");
  RBDDarray DSSDhiGainBack = setup.getArray("DSSDhiGainBack");

  RBDDarray SeGA = setup.getArray("SeGA");

  
  // Load PID gate

  TFile *fGateFile = new TFile("root-files/PIDGates2.root","READ");
  TCutG *fGate;          //! PID Gate
  TCutG *fGate72Rb;
  TCutG *fGate70Kr;
  TCutG *fGate74Sr;
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  // fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
    //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));
  fGate74Sr = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  fGate72Rb = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Rb")); 
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Kr")); 
  fGate70Kr = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_70Kr")); 

  cout << "--> Loaded Gate: " << fGate->GetName() << endl << endl;
  fGateFile->Close();


  //creating diagnostics class for counting...
  diagnostics counterList;
  counterList.add("foundIon");
  counterList.add("decays");
  counterList.add("lostIonNoImplantation");
  counterList.add("lostIonOneStripImplantation");
  counterList.add("lostIonNoDecay");
  counterList.add("lostIonSecondImplant");
  counterList.add("ImplantWaitWindow");
  counterList.add("frontImplantMult");
  counterList.add("backImplantMult");
  counterList.add("found74Sr");
  counterList.add("found72Rb");
  counterList.add("found70Kr");
  counterList.add("XFP_CFD");
  counterList.add("totalIon");

  /*****************************************
   
   LOOP OVER DDAS EVENTS IN TCHAIN

   ****************************************/

  cerr << " Processing Events..." << endl; 

  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  ddaschannel * curChannel = new ddaschannel;

  dataChain.SetBranchAddress("ddasevent", &pDDASEvent);
  long long int lastEntry = 0;  //keep track of the last entry accessed in the branch

  RBDDdet PIN1 = setup.getDet("PIN1");
  RBDDdet TOF = setup.getDet("TOF");
  RBDDdet XFP_CFD = setup.getDet("XFP");

  //clock ticks are in ns for DDAS
  double coinWindow2 = 10000;
  double waitWindow = 5E4; //0.05ms

  double coinWindow = setup.getCoinWindow();
  double corrWindow = setup.getCorrWindow();

  // RBDDTriggeredEvent requires virtual RRBDDChannel type
  // This was done because the event handler should not have to
  // worry about how data is unpacked.

  // Hoever, this requires user to create a class for
  // unpacking data from ROOT Tree e.g. RRBDDASChannel

  RBDDASChannel * bufferEvent = new RBDDASChannel;
  bufferEvent->setEventPointer(pDDASEvent);

  RBDDTriggeredEvent* eventHandler = new RBDDTriggeredEvent("Correlated Events", "title", bufferEvent, coinWindow);
  eventHandler->setTriggerCh(0);

  eventHandler->activateDetector(PIN1);
  eventHandler->activateDetector(TOF);
  eventHandler->activateArray(SeGA);
  eventHandler->activateArray(DSSDhiGainFront);
  eventHandler->activateArray(DSSDhiGainBack);
  eventHandler->activateArray(DSSDloGainFront);
  eventHandler->activateArray(DSSDloGainBack);
  eventHandler->activateDetector(XFP_CFD);

  std::vector<ionCorrelator> implantedIonList;
  long unsigned int oldIonNumber = 0;

  std::vector<ionCorrelator> decayIonList;


  double firstTime = 0;
  double lastTime = 0;
  bool alreadyTriggered = false;

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry+1){

    bool needIonOverlapCheck = oldIonNumber != implantedIonList.size();

    //Clear Detectors defined outside of event loop that had they're data operated on already
    eventHandler->clear();

    //normal use
    lastEntry = eventHandler->FillBuffer(dataChain, iEntry);  
    
    if(eventHandler->isTriggered()){

      oldIonNumber = implantedIonList.size();

      bool foundTOF = false;
      bool foundIonOfInterest = false;
      double curTOF = 0;
      double implantTime = eventHandler->triggerTime;
      int fImplantEFMaxStrip = -100;
      int fImplantEBMaxStrip = -100;

      lastEntry = eventHandler->GetCoinEvents(dataChain); //implant buffer will be filled with a list of coincidence Events
      double PIN1energy = eventHandler->triggerSignal;

      //eventHandler->Print(); //if you want a human-readable list of events

      foundTOF = TOF.getEvents().size() > 0; 

      if(foundTOF){ //check that TOF actually got filled
	curTOF = TOF.getFillerEvent().signal;
	Histo->h_PID->Fill(curTOF, PIN1energy);
	counterList.count("totalIon");
	if(!alreadyTriggered){
	  firstTime = implantTime;
	}
      }

      foundIonOfInterest = fGate->IsInside(curTOF,PIN1energy);
      if(fGate74Sr->IsInside(curTOF,PIN1energy)){counterList.count("found74Sr");}
      if(fGate72Rb->IsInside(curTOF,PIN1energy)){counterList.count("found72Rb");}
      if(fGate70Kr->IsInside(curTOF,PIN1energy)){counterList.count("found70Kr");}

      for( auto &SeGAEvent: SeGA.getEventList() ){
    	Histo->hPromptGammaEnergy->Fill(SeGAEvent.energy);
      }

      double Emax = 0;
      Event frontImplant;
      for(auto &frontEvent : DSSDloGainFront.getEventList()){
    	if(foundIonOfInterest){counterList.count("frontImplantMult");}


    	RBDDTrace trace(frontEvent.trace);
    	double max = trace.GetMaximum();
    	double QDC = trace.GetQDC();

    	//if(frontEvent.signal > Emax){
    	if(QDC > Emax){
    	  //Emax = frontEvent.signal;
    	  Emax = QDC;
    	  fImplantEFMaxStrip =  40 - (frontEvent.channel - 103);
    	  implantTime = frontEvent.time;
	  
    	  frontImplant = frontEvent;
	  
    	}

      }



      Emax=0;
      Event backImplant;
      for(auto& backEvent : DSSDloGainBack.getEventList()){
    	if(foundIonOfInterest){counterList.count("backImplantMult");}

    	RBDDTrace trace(backEvent.trace);
    	double max = trace.GetMaximum();
    	double QDC = trace.GetQDC();

    	//if(backEvent.signal > Emax){
    	if(QDC > Emax){
    	  //Emax = backEvent.signal;
    	  Emax = QDC;
	  if(backEvent.channel < 0){cout << "OK..." << endl;}
    	  fImplantEBMaxStrip = 40 - (backEvent.channel - 183);	  
    	  backImplant = backEvent;
    	}

      }	


      
      if(!foundIonOfInterest){continue;} //only continue analysis if ion of interest is found
      counterList.count("foundIon");

      if(fImplantEFMaxStrip == -100 && fImplantEBMaxStrip == -100){counterList.count("lostIonNoImplantation");continue;} 
      if(fImplantEFMaxStrip == -100 || fImplantEBMaxStrip == -100){counterList.count("lostIonOneStripImplantation");continue;}

      

      Histo->h_PID_gated->Fill(curTOF,PIN1energy);

      double Ethreshold=100.;
      double stripTolerance= setup.getStripTolerance();//2.;//3.;

      ionCorrelator ionOfInterest(corrWindow, Ethreshold, stripTolerance, frontImplant, backImplant, Histo); 
      
      //check for overlaps before adding to list
      int it= 0;
      bool foundOverlap = false;
      for(auto & ion : implantedIonList){
	if(ion.implantOverlap(fImplantEFMaxStrip, fImplantEBMaxStrip)){
	  foundOverlap = true;
	  implantedIonList.erase(implantedIonList.begin()+it);
	}
	else{
	it++;
	}
      }

      if(foundOverlap){continue;}

      implantedIonList.push_back(ionOfInterest);
	  
    } //end of original trigger
    else if(eventHandler->isTriggered( triggerCondition ) ){
	
	lastEntry = eventHandler->GetCoinEvents(dataChain); //decay buffer will be filled with a list of coincidence Events

	Event frontDecay;
	Event frontDecayAddBack;
	Event backDecay;
	
	bool foundFront = DSSDhiGainFront.getEventList().size() > 0;
	bool foundBack = DSSDhiGainBack.getEventList().size() > 0;
	bool implantEvent = PIN1.getEvents().size() > 0;

	//need to make sure that there is a DSSDfront and DSSDback event before analyzing
	if(foundBack && foundFront && !implantEvent){
	  frontDecay  = DSSDhiGainFront.maxE();
	  frontDecayAddBack  = DSSDhiGainFront.addBack();
	  backDecay = DSSDhiGainBack.maxE();


	  for(auto & ion : implantedIonList){
	    if( ion.analyze(DSSDhiGainFront.getEventList(), DSSDhiGainBack.getEventList(), SeGA.getEventList()) ){
	      counterList.count("decays");
	      double TGate = 2E8;
	      if(ion.getDecayTime() < TGate){
		Histo->hDecayEnergyTot_TGate->Fill(frontDecayAddBack.energy);
	      }
	      else{
		Histo->hDecayEnergyTotBackground->Fill(frontDecayAddBack.energy);
	      }
	  
	    }
	  
	  }
	

	}
	else if(implantEvent){
	  cout << "Dan you need to figure out a way to handle this" << endl;
	}
	  
    } //end of second trigger

    //should check if correlator needs to be deleted if decayTime over corrWindow
    int it= 0;
    double curTime = eventHandler->GetBuffer().back().time;
    for(auto & ion : implantedIonList){
      if(curTime > 0 && (curTime - ion.getFrontImplant().time) > corrWindow ){
	implantedIonList.erase(implantedIonList.begin()+it);
      }
      else{
	it++;
      }
    }

    lastTime = curTime;

  } //end of loop over entries
    

  
  Histo->write(); // this forces the histrograms to be read out to file

  cout << "Total number of Ions of Interest found: " << counterList.returnValue("foundIon") << endl;
  cout << "XFP counts: " << counterList.returnValue("XFP_CFD") << endl;
  cout << "Total number of decay events found in correlation window: " << counterList.returnValue("decays") << endl;
  //cout << "Total number of decay events found in correlation window: " <<  << endl;
  cout << "Total number of Ions lost in implantation (no strips): " << counterList.returnValue("lostIonNoImplantation") << endl;
  cout << "Total number of Ions with one strip in implantation: " << counterList.returnValue("lostIonOneStripImplantation") << endl;
  //cout << "Total number of Ions lost in decay: " << counterList.returnValue("foundIon") - counterList.returnValue("decays") - counterList.returnValue("lostIonNoImplantation")-counterList.returnValue("lostIonOneStripImplantation") << endl;
  cout << "Decays lost to second Implantation: " << counterList.returnValue("lostIonSecondImplant") << endl;
  //cout << "Total triggers: " << counterList.returnValue("foundIon") + counterList.returnValue("lostIonSecondImplant") << endl;
  cout << "Implants Found in Wait Window: " << counterList.returnValue("ImplantWaitWindow") << endl;
  cout << "found74Sr: " << counterList.returnValue("found74Sr") << endl;
  cout << "found72Rb: " << counterList.returnValue("found72Rb") << endl;
  cout << "found70Kr: " << counterList.returnValue("found70Kr") << endl;
  cout << "average particle rate (Hz): " << counterList.returnValue("totalIon")/(lastTime-firstTime)*1E9 << endl;

  cout << endl;

  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
	    << "elapsed time: " << elapsed_seconds.count() << "s\n";  
  cout << endl;

  return 1; //cuz I'm old school
}
