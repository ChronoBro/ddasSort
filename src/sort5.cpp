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
    cerr << "Example: ./sort5 ddasConfigTest 100 101 myOutputFile" << endl;
    return 0;
    //abort();
  }

  TChain dataChain("dchan"); //dchan is name of tree for ddasdumped files

  //reading in config file
  //because TChain doesn't have a copy function set I havee to feed it into config contructor
  config setup(configFile.str(),runStart,runEnd, dataChain);

  //creating histogram class that keeps track of histograms being written to file
  histo * Histo = new histo(outRootfile.str());

  int maxSubRun = 9;


  
  cout << endl;

  long long int fNEntries = dataChain.GetEntries(); //64bit int, because I was using int for the entries I wasn't able to access long lists, henceso rt failing for a long list of data files

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

  TFile *fGateFile = new TFile("root-files/PIDGates3.root","READ");
  TCutG *fGate;          //! PID Gate
  TCutG *fGate72Rb;
  TCutG *fGate70Kr;
  TCutG *fGate74Sr;
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Br"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_70Br"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_70Kr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));
  fGate74Sr = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_74Sr"));
  fGate72Rb = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Rb"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Rb")); 
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_72Kr")); 
  fGate70Kr = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_70Kr"));  //calling new twice on the same FindObjectAny is no good
  //fGate = fGate70Kr; //cuz I'm lazy this is my fix...

  cout << "--> Loaded Gate: " << fGate->GetName() << endl << endl;
  //fGateFile->Close();


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
  counterList.add("activeIon");

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
  RBDDdet SCINT = setup.getDet("SCINT");

  //clock ticks are in ns for DDAS
  double coinWindow2 = 10000;
  double waitWindow = 5E4; //0.05ms

  double coinWindow = setup.getCoinWindow();
  double corrWindow = setup.getCorrWindow();
  double TGate = setup.getTGate();

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
  eventHandler->activateArray(SSD);
  eventHandler->activateDetector(XFP_CFD);
  eventHandler->activateDetector(SCINT);

  std::vector<ionCorrelator> implantedIonList;
  long unsigned int oldIonNumber = 0;

  std::vector<ionCorrelator> decayIonList;
  std::vector<Event> lastBufferHandled;

  double firstTime = 0;
  double lastTime = 0;
  double curTime = 0;
  double totalTime = 0;
  bool alreadyTriggered = false;

  double activeTime = 0;

  for(long long int iEntry=0;iEntry<fNEntries;iEntry=lastEntry+1){

    bool needIonOverlapCheck = oldIonNumber != implantedIonList.size();

    //Clear Detectors defined outside of event loop that had they're data operated on already
    eventHandler->clear();

    //normal use
    lastEntry = eventHandler->FillBuffer(dataChain, iEntry);  
    
    if(eventHandler->isTriggered()){

      lastEntry = eventHandler->GetCoinEvents(dataChain); //implant buffer will be filled with a list of coincidence Events
      //double PIN1energy = eventHandler->triggerSignal;


    IMPLANT:
      oldIonNumber = implantedIonList.size();

      bool foundTOF = false;
      bool foundIonOfInterest = false;
      double curTOF = 0;
      //double implantTime = eventHandler->triggerTime;
      double implantTime = PIN1.getFillerEvent().signal;
      int fImplantEFMaxStrip = -100;
      int fImplantEBMaxStrip = -100;

      double PIN1energy = PIN1.getFillerEvent().signal;

      //eventHandler->Print(); //if you want a human-readable list of events

      //just for testing
      // for(auto & event : eventHandler->GetBuffer()){
      // 	Histo->hTriggerTest->Fill(event.time - eventHandler->triggerTime);
      // }

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

      double Emax = 0;
      Event frontImplant;
      for(auto &frontEvent : DSSDloGainFront.getEventList()){
    	if(foundIonOfInterest){counterList.count("frontImplantMult");}


    	RBDDTrace trace(frontEvent.trace);
    	double max = trace.GetMaximum();
    	double QDC = trace.GetQDC();

    	//if(frontEvent.signal > Emax){
	if(QDC > Emax){
    	  Emax = frontEvent.signal;
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
    	  Emax = backEvent.signal;
    	  Emax = QDC;
	  if(backEvent.channel < 0){cout << "OK..." << endl;}
    	  fImplantEBMaxStrip = 40 - (backEvent.channel - 183);	  
    	  backImplant = backEvent;
    	}

      }	

      double Ethreshold=100.;
      double stripTolerance= setup.getStripTolerance();//2.;//3.;


      //IF weird triggers are coming from implant signals then if I have a short wait time after implantation then it should be OK
      // for(;iEntry<fNEntries;iEntry=lastEntry+1){
      // 	lastEntry = eventHandler->FillBufferCoin(dataChain, iEntry);  
      // 	double timeCheck =  eventHandler->GetBuffer().back().time - implantTime;
      // 	eventHandler->Clear();
      // 	eventHandler->GetBuffer().clear();
      // 	//cout << timeCheck << endl;
      // 	if( timeCheck > 2000 || eventHandler->GetBuffer().size() <2){ break;}
      // }
      

      ionCorrelator ionOfInterest(corrWindow, TGate, Ethreshold, stripTolerance, frontImplant, backImplant, Histo);       

      //check for overlaps
      int it= 0;
      bool foundOverlap = false;
      double curTime2 = eventHandler->GetBuffer().back().time;
      // for(auto & ion : implantedIonList){
      // 	double testTime = (curTime2 - ion.getFrontImplant().time);
      // 	if(ion.implantOverlap(fImplantEFMaxStrip, fImplantEBMaxStrip)){
      // 	  foundOverlap = true;
      // 	  if(testTime > 1E9){activeTime += testTime-1.0E9;} //trying to keep track of how long ionCorrelator's last
      // 	  else{counterList.sub("activeIon");}
      // 	  //implantedIonList[it].deleteFilter();//to handle ROOT's memory properly...
      // 	  implantedIonList.erase(implantedIonList.begin()+it);
      // 	}
      // 	else{
      // 	it++;
      // 	}
      // } // 4/13/2020, turning this off to see if anything changes in 71Kr spectrum... it does! and thus background is different... but that's OK because we have a better background estimation now!

      
      if(!foundIonOfInterest){continue;} //only continue analysis if ion of interest is found
      counterList.count("foundIon");

      for( auto &SeGAEvent: SeGA.getEventList() ){
      	// std::cout << "time diff... = " << SeGAEvent.time-implantTime << endl;
      	// std::cout << "energy = " << SeGAEvent.energy << endl;
      	Histo->hGammaEvsImplantT->Fill(SeGAEvent.time - implantTime,SeGAEvent.energy);
      }


      if(fImplantEFMaxStrip == -100 && fImplantEBMaxStrip == -100){counterList.count("lostIonNoImplantation");continue;} 
      if(fImplantEFMaxStrip == -100 || fImplantEBMaxStrip == -100){counterList.count("lostIonOneStripImplantation");continue;}


      

      Histo->h_PID_gated->Fill(curTOF,PIN1energy);


      if(foundOverlap){continue;}
      counterList.count("activeIon");
      implantedIonList.push_back(ionOfInterest);
	  
    } //end of original trigger
    else if(eventHandler->isTriggered( triggerCondition ) ){
	
      lastEntry = eventHandler->GetCoinEvents(dataChain); //decay buffer will be filled with a list of coincidence Events

      //just for testing purposes
      for(auto & event : eventHandler->GetBuffer()){
      	Histo->hTriggerTest->Fill(event.time - eventHandler->triggerTime);
      }


      Event frontDecay;
      Event frontDecayAddBack;
      Event backDecay;
	
      bool foundFront = DSSDhiGainFront.getEventList().size() > 0;
      bool foundBack = DSSDhiGainBack.getEventList().size() > 0;
      bool implantEvent = PIN1.getEvents().size() > 0;
      bool punchThru = SCINT.fired();//SSD.fired() || ;

      //need to make sure that there is a DSSDfront and DSSDback event before analyzing
      if(foundBack && foundFront && !implantEvent && !punchThru){


	frontDecay  = DSSDhiGainFront.maxE();
	frontDecayAddBack  = DSSDhiGainFront.addBack();
	backDecay = DSSDhiGainBack.maxE();


	for( auto & frontEvent : DSSDhiGainFront.getEventList() ){
	  RBDDTrace test(frontEvent.trace);
	  test.SetMSPS(100.); //so that time is correct on traces

	  if(frontEvent.signal > 10000 && frontEvent.signal > 15000 && test.GetQDC() > 300000. && test.GetQDC() < 400000.){
	    //nameMe << "R3_trace-Energy_" << frontEvent.energy;
	    for(auto & gamma : SeGA.getEventList()){
	      Histo->hGammaEnergy_R3events->Fill(gamma.energy);
	    }
	    break;
	  }

	}

	// RBDDTrace test(frontDecay.trace);
	// if(test.GetQDC() < -50000.){
	//   std::cout << "bad channel = " << frontDecay.channel << std::endl;
	//   eventHandler->Print();
	// }

	for(auto & ion : implantedIonList){
	  if( ion.analyze(DSSDhiGainFront.getEventList(), DSSDhiGainBack.getEventList(), SeGA.getEventList()) ){
	    counterList.count("decays");
	    //double TGate = 20E9;//1E9;//20E9;
	    if(ion.getDecayTime() < TGate){
	      Histo->hDecayEnergyTot_TGate->Fill(frontDecayAddBack.energy);
	      for(auto & gamma :SeGA.getEventList()){
		Histo->hGammaVsDecayEtot->Fill(frontDecayAddBack.energy,gamma.energy);
	      }
	      double avg = (frontDecay.energy+backDecay.energy)/2.;
	      Histo->hDecayEnergyAvg_TGate->Fill(avg);
	    }
	    else{
	      Histo->hDecayEnergyTotBackground->Fill(frontDecayAddBack.energy);
	    }
	  
	  }
	  
	}
	

      }
      else if(implantEvent){
	goto IMPLANT; //quick fix (8/10/2020)? Ideally I should restructure my code to make the above code "methods" that can be called
	cout << "Dan you need to figure out a way to handle this" << endl;
      }
	  
    } //end of second trigger


    //quick bug fix would be to kill all ions if tree switch is observed...
    //I know what that means... it will be a long-lasting bandaid
    //moved below code above correlator removal 7/13/2020

    curTime = eventHandler->GetBuffer().back().time;
    if(lastTime > curTime){
      //cout << "Tree switch?" << endl;
      for(auto & ion: implantedIonList){
	double testTime = (lastTime - ion.getFrontImplant().time);
	if(testTime > 1E9){activeTime += testTime-1.0E9;}
	if(testTime < 1E9){
	  std::cout << "Dan there are background ions you aren't measuring so you should not trust scaled background" << std::endl;
	}
      }

      implantedIonList.clear();

      totalTime += lastTime;
      eventHandler->GetBuffer().clear(); //will clearing this out cause those events to go away?
    }


    // 7/13/2020, Have identified bug that when trees switch the ions don't get killed because incorrect active time and keep filling spectra
    //... very bad!

    //should check if correlator needs to be deleted if decayTime over corrWindow
    int it= 0;
    for(auto & ion : implantedIonList){
      double testTime = (curTime - ion.getFrontImplant().time);
      if(curTime > 0 && testTime > corrWindow ){
	if(testTime > TGate){activeTime += testTime-TGate;}
	//implantedIonList[it].deleteFilter();//to handle ROOT's memory properly...
	implantedIonList.erase(implantedIonList.begin()+it); 
      }
      else{
	it++;
      }
    }

    lastTime = curTime;
    lastBufferHandled = eventHandler->GetBuffer();

  } //end of loop over entries

  totalTime += curTime;

  //This is a bit more subtle... I need to make sure that I'm only averaging ion correlators with an active time > 1s
  //If almost all are going that far then below will be OK, otherwise I need to be more careful
  //I think I'm handling this properly now
  activeTime = activeTime/double(counterList.value("activeIon"));
  Histo->hDecayEnergyBackgroundScaled->Scale(TGate/(activeTime)); //I want to scale background by TcutOff/Average Background Analysis Time
    

  
  Histo->write(); // this forces the histrograms to be read out to file

  cout << "Total number of Ions of Interest found: " << counterList.value("foundIon") << endl;
  cout << "XFP counts: " << counterList.value("XFP_CFD") << endl;
  cout << "Total number of decay events found in correlation window: " << counterList.value("decays") << endl;
  //cout << "Total number of decay events found in correlation window: " <<  << endl;
  cout << "Total number of Ions lost in implantation (no strips): " << counterList.value("lostIonNoImplantation") << endl;
  cout << "Total number of Ions with one strip in implantation: " << counterList.value("lostIonOneStripImplantation") << endl;
  //cout << "Total number of Ions lost in decay: " << counterList.value("foundIon") - counterList.value("decays") - counterList.value("lostIonNoImplantation")-counterList.value("lostIonOneStripImplantation") << endl;
  cout << "Decays lost to second Implantation: " << counterList.value("lostIonSecondImplant") << endl;
  //cout << "Total triggers: " << counterList.value("foundIon") + counterList.value("lostIonSecondImplant") << endl;
  cout << "Implants Found in Wait Window: " << counterList.value("ImplantWaitWindow") << endl;
  cout << "found74Sr: " << counterList.value("found74Sr") << endl;
  cout << "found72Rb: " << counterList.value("found72Rb") << endl;
  cout << "found70Kr: " << counterList.value("found70Kr") << endl;
  cout << "average particle rate (Hz): " << counterList.value("totalIon")/(totalTime)*1E9 << endl;

  cout << endl;

  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
	    << "elapsed time: " << elapsed_seconds.count() << "s\n";  
  cout << endl;

  return 1; //cuz I'm old school
  }
