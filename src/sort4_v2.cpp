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

using namespace std;

//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

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

  //creating histogram class that keeps track of histograms being written to file
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

  RBDDarray SSD;

  RBDDarray DSSDloGainFront;
  RBDDarray DSSDloGainBack;
  
  RBDDarray DSSDhiGainFront;
  RBDDarray DSSDhiGainBack;

  RBDDarray SeGA;

  TFile          *fSeGACalFile; //!
  TList *fSeGACalibFunc; //! List of calibration functions.
  fSeGACalibFunc = new TList();

  ifstream alphaCalib("Calibrations/alphaCalib.dat");
  ifstream alphaCalibSSD("Calibrations/alphaCalibrationSSD.dat");

  //for fast sorting to work I have to declare detector objects at the beginning here, or at least have a well defined place that they exist,
  //then RBDDarray will carry a list of pointers to those objects
  RBDDdet SSDobjects[16];
  for(int i=32;i<48;i++){
    int chan = 0.;
    double slope;
    double offset;
    alphaCalibSSD >> chan >> offset >> slope; 

    vector<double> params;
    params.push_back(offset);
    params.push_back(slope);

    
    //RBDDASChannel* strip = new RBDDASChannel(i);
    //strip->setCalibration(params);
    //SSD.push_back(strip); 
    //RBDDdet strip(i);
    SSDobjects[i-32].setCalibration(params);
    SSDobjects[i-32].setAssignedChannel(i);
    SSD.addDet(SSDobjects[i-32]);

  }
  alphaCalibSSD.close();

  //load DSSD calibrations


  //again have to define detector objects here so they "live" somewhere well-defined
  RBDDdet frontHi[40];
  RBDDdet frontLo[40];

  RBDDdet backHi[40];
  RBDDdet backLo[40];

  for(Int_t i=0; i<80; i++){
    int chan=0.;
    double slope;
    double offset;
    alphaCalib >> chan >> offset >> slope; 

    vector<double> params;
    params.push_back(offset);
    params.push_back(slope);

    // RBDDdet strip(chan);
    // RBDDdet strip2(chan+40);
    //strip.setCalibration(params);

    // cout << chan << endl;
    // cout << chan+40 << endl;
    
    if(i<40){

      frontHi[i].setCalibration(params);
      frontHi[i].setAssignedChannel(chan);

      frontLo[i].setCalibration(params);
      frontLo[i].setAssignedChannel(chan+40);

      //I should be able to remove strips by just not adding them to the RBDDarray
      double strip = chan-64.;
      if(strip == 3 || strip == 14 || strip == 15){DSSDloGainFront.addDet(frontLo[i]);continue;}

      DSSDhiGainFront.addDet(frontHi[i]);
      DSSDloGainFront.addDet(frontLo[i]);
    }
    else{

      backHi[i-40].setCalibration(params);
      backHi[i-40].setAssignedChannel(chan);

      backLo[i-40].setCalibration(params);
      backLo[i-40].setAssignedChannel(chan+40);

      DSSDhiGainBack.addDet(backHi[i-40]);
      DSSDloGainBack.addDet(backLo[i-40]);
    }

  }
  alphaCalib.close();


  // Load SeGA Calibrartions
  char name[500];
  int SeGAchannel = 16;
  fSeGACalFile = new TFile("Calibrations/SeGACalibrations.root","READ");

  RBDDdet SEGAObjects[16];
  for(Int_t i=0; i<16; i++){
    sprintf(name,"f_cal_SeGA_%02i",i);
    TF1 *funG= (TF1*)fSeGACalFile->FindObjectAny(name);
    
    vector<double> params;
    params.push_back(funG->GetParameter(0));
    params.push_back(funG->GetParameter(1));
    
    // RBDDdet ge(SeGAchannel);
    // ge.setCalibration(params);
    SEGAObjects[i].setCalibration(params);
    SEGAObjects[i].setAssignedChannel(SeGAchannel);
    SeGA.addDet(SEGAObjects[i]);

    SeGAchannel++;

    delete funG;
  }
  fSeGACalFile->Close();
  
  // Load PID gate

  TFile *fGateFile = new TFile("PIDGates2.root","READ");
  TCutG *fGate;          //! PID Gate
  TCutG *fGate72Rb;
  TCutG *fGate70Kr;
  TCutG *fGate74Sr;
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
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

  /*****************************************
   
   LOOP OVER DDAS EVENTS IN TCHAIN

   ****************************************/

  cerr << " Processing Events..." << endl; 

  DDASEvent      *pDDASEvent = new DDASEvent;   //! pointer to DDASEvent
  ddaschannel * curChannel = new ddaschannel;

  dataChain.SetBranchAddress("ddasevent", &pDDASEvent);
  long long int lastEntry = 0;  //keep track of the last entry accessed in the branch


  RBDDdet PIN1(0);
  RBDDdet TOF(5);
  RBDDdet XFP_CFD(6);

  //clock ticks are in ns for DDAS
  double coinWindow = 5000; //5 us
  double coinWindow2 = 10000;
  double corrWindow = 5E9;  //5 s
  double waitWindow = 5E4; //0.05ms
  //corrWindow = 1E9;

  // RBDDTriggeredEvent requires virtual RRBDDChannel type
  // This was done because the event handler should not have to
  // worry about how data is unpacked.

  // Hoever, this requires user to create a class for
  // unpacking data from ROOT Tree e.g. RRBDDASChannel

  RBDDASChannel * bufferEvent = new RBDDASChannel;
  bufferEvent->setEventPointer(pDDASEvent);

  //if I create my own 'framework' for analysis this should probably be called the event handler
  RBDDTriggeredEvent* implant = new RBDDTriggeredEvent("Prompt Trigger", "title", bufferEvent, coinWindow);
  implant->setTriggerCh(0);
  //implant->activateDetector(PIN1);
  implant->activateDetector(TOF);
  //implant->activateDetector(XFP_CFD);

  implant->activateDetector(PIN1);
  implant->activateDetector(TOF);
  implant->activateArray(SeGA);
  implant->activateArray(DSSDloGainFront);
  implant->activateArray(DSSDloGainBack);
  implant->activateArray(DSSDhiGainFront);
  implant->activateArray(DSSDhiGainBack);
  implant->activateDetector(XFP_CFD);

  //implant->setTriggerCh(6); //for testing to see if all XFP signals are found
  RBDDTriggeredEvent* decay = new RBDDTriggeredEvent("Correlated Events", "title", bufferEvent, coinWindow);
  decay->setTriggerCh(0);

  decay->activateDetector(PIN1);
  decay->activateDetector(TOF);
  decay->activateArray(SeGA);
  decay->activateArray(DSSDhiGainFront);
  decay->activateArray(DSSDhiGainBack);
  decay->activateArray(DSSDloGainFront);
  decay->activateArray(DSSDloGainBack);



  RBDDTriggeredEvent* XFP = new RBDDTriggeredEvent("XFP Trigger", "title", bufferEvent, coinWindow);
  XFP->setTriggerCh(6);

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry+1){

    //clear detectors defined outside of event loop that had they're data operated on already
    implant->clear();

    //normal use
    lastEntry = implant->FillBuffer(dataChain, iEntry);  
    
    if(implant->isTriggered()){


       bool foundTOF = false;
       bool foundIonOfInterest = false;
       double curTOF = 0;
       double implantTime = implant->triggerTime;
       int fImplantEFMaxStrip = -100;
       int fImplantEBMaxStrip = -100;

       lastEntry = implant->GetCoinEvents(dataChain); //implant buffer will be filled with a list of coincidence Events
       double PIN1energy = implant->triggerSignal;

       // //dumpBuffer() now pushes event data to exact detector for the channel, this should reduce number of loops when filling events
       // //should make a very big difference when analyzing large set of implantation events
       implant->dumpBuffer();       

       foundTOF = TOF.getEvents().size() > 0; 

       if(foundTOF){ //check that TOF actually got filled
	 curTOF = TOF.getFillerEvent().signal;
	 Histo->h_PID->Fill(curTOF, PIN1energy);
	 //counterList.count("foundIon");
       }

      foundIonOfInterest = fGate->IsInside(curTOF,PIN1energy);
      if(fGate74Sr->IsInside(curTOF,PIN1energy)){counterList.count("found74Sr");}
      if(fGate72Rb->IsInside(curTOF,PIN1energy)){counterList.count("found72Rb");}
      if(fGate70Kr->IsInside(curTOF,PIN1energy)){counterList.count("found70Kr");}

      //for testing
      // cout << endl;
      // for(auto & bufferEvent: implant->GetBuffer()){
      // 	cout << bufferEvent.channel << endl;
      // 	cout << setprecision(15)  << bufferEvent.time << endl;
      // }
      // cout << endl;

      //implant->Print();

      for( auto &SeGAEvent: SeGA.getEventList() ){
    	Histo->hPromptGammaEnergy->Fill(SeGAEvent.energy);
    	//cout << "SeGA multiplicity = " << SeGA.getEventList().size() << endl;
    	//cout << "SeGA channel = " << SeGAEvent.channel << endl;

    	// for(auto & SeGAEvent2: SeGA.getEventList() ){
    	//   if(SeGAEvent.energy!=SeGAEvent2.energy){
    	//     Histo->gg_prompt->Fill(SeGAEvent.energy,SeGAEvent2.energy);
    	//   }
    	// }

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

      
      //make search for decay events here
      bool firstEvent = true;
      do{

	//clear detectors living outside of event loop that had they're data operated on already
	decay->clear();
	bool implantEvent = false;
      	if(lastEntry >= dataChain.GetEntries()){break;}
      	lastEntry = decay->FillBuffer(dataChain, lastEntry+1);

	if( abs(decay->GetBuffer().back().time - implantTime) < waitWindow ){continue;}

	//isTriggered can be passed a function of type-> bool f(int) [see top of code]
      	if(decay->isTriggered( triggerCondition ) ){
	
	  lastEntry = decay->GetCoinEvents(dataChain); //decay buffer will be filled with a list of coincidence Events	
	  decay->dumpBuffer(); //dumpbuffer will send (point) data in buffer directly to activated 'RBDDdets' and 'RBDDarrays' (i.e. place in memory)
	 
	  implantEvent = PIN1.getEvents().size() > 0;

      	  //for testing seeing the channel structure of grouped events
      	  // cout << endl;
      	  // for(auto& event : decay->GetBuffer()){
      	  //   cout << event.channel << endl;
      	  // }
	  // cout << implantEvent << endl;
      	  // cout << endl;
	  
      	  Event frontDecay;
      	  Event frontDecayAddBack;
      	  Event backDecay;

	  bool foundFront = DSSDhiGainFront.getEventList().size() > 0;
	  bool foundBack = DSSDhiGainBack.getEventList().size() > 0;

	  //need to make sure that there is a DSSDfront and DSSDback event before analyzing
      	  if(foundBack && foundFront){
	    frontDecay  = DSSDhiGainFront.maxE();
	    frontDecayAddBack  = DSSDhiGainFront.addBack();
	    backDecay = DSSDhiGainBack.maxE();
	  }
      	  else{
      	    continue;
      	  }

      	  int backStrip = backDecay.channel - 144;
      	  int frontStrip = frontDecayAddBack.channel - 64;

	  
      	  double Ethreshold = 100;
      	  double stripTolerance = 3.;
	  RBDDTrace test(frontDecay.trace);

	  //With filtering change, I reproduce the spectra for 73Sr that I got before, hooray!!! 9/22/2019

	  for(auto& frontEvent : DSSDhiGainFront.getEventList()){
	    frontStrip = frontEvent.channel - 64.;

	    if(frontEvent.energy <1500){ //apparently I needed this low energy gate for the filter to only work on things that were actually BAD 9/22/2019
	      RBDDTrace test2(frontEvent.trace);

	      if(!test2.filter()){ // filter() returns true if passes filter
		break;
	      }

	    }

	    for(auto& backEvent : DSSDhiGainBack.getEventList()){
	      backStrip = backEvent.channel -144.;

	      if( abs(frontStrip - fImplantEFMaxStrip) < stripTolerance 
		  && abs(backStrip - fImplantEBMaxStrip) < stripTolerance //I till think something is screwy with the backs...
		  && frontEvent.energy > Ethreshold
		  && backEvent.energy > Ethreshold
		  ){

		Histo->hDecayTime->Fill(frontEvent.time-implantTime);
		Histo->hDecayEnergy->Fill(frontEvent.energy);
		Histo->hDecayEnergyTot->Fill(frontDecayAddBack.energy);
		counterList.count("decays");
	    
		double decayTime = frontEvent.time-implantTime;
		double TCutoff = 2E8; //200ms

		if(decayTime < TCutoff){
		  Histo->hDecayEnergyTot_TGate->Fill(frontDecayAddBack.energy);
		  Histo->hDecayEnergyTGate->Fill(frontEvent.energy);
		  for(auto & segaEvent : SeGA.getEventList()){
		    //Histo->hGammaEnergy->Fill(segaEvent.energy);
		    Histo->hGammaVsDecayTGated->Fill(frontEvent.energy,segaEvent.energy);
		    //if(frontDecayAddBack.energy > 3000 && frontDecayAddBack.energy < 3800){
		    Histo->hGammaEnergyG->Fill(segaEvent.energy);
	  	    //}
		  }

		}

		for(auto & segaEvent : SeGA.getEventList()){
		  Histo->hGammaEnergy->Fill(segaEvent.energy);
		  Histo->hGammaVsDecay->Fill(frontEvent.energy,segaEvent.energy);
		}


	    
		if(decayTime > 1E9){
		  Histo->hDecayEnergyTotBackground->Fill(frontDecayAddBack.energy);
		  Histo->hDecayEnergyBackground->Fill(frontEvent.energy);
		}

		if(firstEvent){
		  Histo->hDecayEnergyTot_firstEvent->Fill(frontDecayAddBack.energy);
		  Histo->hDecayTimeLog->Fill(frontEvent.time-implantTime);
		  firstEvent = false;
		}
	    
	      
		break;
	      }

	    } //end loop over DSSD back events

	  } //end loop over DSSD front events	    

	}  //end second trigger

      } while(  abs(decay->GetBuffer().back().time-implantTime) < corrWindow ); //end of correlation loop

    } //end of original trigger

  } //end of loop over entries

  Histo->write(); // this forces the histrograms to be read out to file

  cout << "Total number of Ions of Interest found: " << counterList.returnValue("foundIon") << endl;
  cout << "XFP counts: " << counterList.returnValue("XFP_CFD") << endl;
  cout << "Total number of decay events found in correlation window: " << counterList.returnValue("decays") << endl;
  cout << "Total number of Ions lost in implantation (no strips): " << counterList.returnValue("lostIonNoImplantation") << endl;
  cout << "Total number of Ions with one strip in implantation: " << counterList.returnValue("lostIonOneStripImplantation") << endl;
  //cout << "Total number of Ions lost in decay: " << counterList.returnValue("foundIon") - counterList.returnValue("decays") - counterList.returnValue("lostIonNoImplantation")-counterList.returnValue("lostIonOneStripImplantation") << endl;
  cout << "Decays lost to second Implantation: " << counterList.returnValue("lostIonSecondImplant") << endl;
  //cout << "Total triggers: " << counterList.returnValue("foundIon") + counterList.returnValue("lostIonSecondImplant") << endl;
  cout << "Implants Found in Wait Window: " << counterList.returnValue("ImplantWaitWindow") << endl;
  cout << "found74Sr: " << counterList.returnValue("found74Sr") << endl;
  cout << "found72Rb: " << counterList.returnValue("found72Rb") << endl;
  cout << "found70Kr: " << counterList.returnValue("found70Kr") << endl;

  cout << endl;

  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
	    << "elapsed time: " << elapsed_seconds.count() << "s\n";  
  cout << endl;

  return 1; //cuz I'm old school
}
