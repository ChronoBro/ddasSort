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
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
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


  RBDDASChannel * bufferEvent = new RBDDASChannel;
  bufferEvent->setEventPointer(pDDASEvent);

  //clock ticks are in ns for DDAS
  double coinWindow = 5000; //5 us
  double coinWindow2 = 10000;
  double corrWindow = 5E9;  //5 s
  double waitWindow = 5E4; //0.05ms
  //corrWindow = 1E9;

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

    /*************************
    need to figure out my buffering issues... 5/2/2019
    i.e. ->not grouping together events perfectly, so I'm throwing away events
    *************************/

    //clear detectors defined outside of event loop that had they're data operated on already
    implant->clear();


    //normal use
    lastEntry = implant->FillBuffer(dataChain, iEntry);

  
    
    //just for getting XFP counts
    // lastEntry = XFP->FillBuffer(dataChain,iEntry);
    // if(XFP->isTriggered()){counterList.count("XFP_CFD");}

    //cout << implant->fillerChannel->GetChanNo() << endl;
    
    
    //it appears the buffer is just being filled indefinitely after being triggered... curious
    //fixed issue, there was an issue with assigning timestamps in RBDDChannel after unpacking (conversion between int and double) that was reporting the same time for each event, thus my buffer checks weren't working and it was being filled indefinitely
    
    // 5/1/2019 -> Buffer filling doesn't appear to be working properly which is then screwing up the rest of data
    // 5/2/2019 -> Buffer appears to working now, even better than sort3_v2 in that sort4 is picking up more implantation events

    //if(false){ // I'm lazy and want to turn off triggerloop for XFP counting
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

       // //kind of stupid but you have to fillArrays after bufferdump. yes its bad design
       // SeGA.fillArray();
       // DSSDloGainBack.fillArray();
       // DSSDloGainFront.fillArray();
       // DSSDhiGainBack.fillArray();
       // DSSDhiGainFront.fillArray();
       //dumpBuffer should now be doing the above fillArray()'s
       

       // cout << endl;
       // for(auto& event : implant->GetBuffer()){
       // 	cout << event.channel << endl;
       // }
       // cout << endl;

       //foundTOF = implant->dumpBuffer(TOF);

       //if(implant->dumpBuffer(XFP_CFD)){counterList.count("XFP_CFD");}
       //cout << TOF.getEvents().size() << endl;

       foundTOF = TOF.getEvents().size() > 0; 

       if(foundTOF){ //check that TOF actually got filled
	 curTOF = TOF.getFillerEvent().signal;
	 Histo->h_PID->Fill(curTOF, PIN1energy);
	 //counterList.count("foundIon");
       }

       //clear detectors after their data has been used
       //TOF.clear();
       //XFP_CFD.clear();
       //above shouldn't be necessary, i.e. clear() only needs to be called once by the event handler

      foundIonOfInterest = fGate->IsInside(curTOF,PIN1energy);
      if(fGate74Sr->IsInside(curTOF,PIN1energy)){counterList.count("found74Sr");}
      if(fGate72Rb->IsInside(curTOF,PIN1energy)){counterList.count("found72Rb");}
      if(fGate70Kr->IsInside(curTOF,PIN1energy)){counterList.count("found70Kr");}

      //dumpBuffer(RBDDarray or RBDDdet) isn't the most efficient since it loops through coincident events each time
      //it seems that dumpBuffer is only ever putting one event into the array event list... need to figure out whats wrong
      //above is DEFINITELY a problem, since the high gain channels fire like crazy and only one event is in the event list... WTF
      //I don't think I changed anything but now it appears to working... wow
      // implant->dumpBuffer(SeGA);
      // implant->dumpBuffer(DSSDloGainFront);
      // implant->dumpBuffer(DSSDloGainBack);

      // cout << endl;
      // cout << SeGA.getEventList().size() << endl;
      // cout << DSSDloGainFront.getEventList().size() << endl;
      // cout << DSSDloGainBack.getEventList().size() << endl;
      // cout << endl;

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


      // if(frontImplant.signal!=0){ //need to make sure that there is a frontImplant!
      // 	RBDDTrace trace(frontImplant.trace);
      // 	trace.GetBaseline(); //calling GetBaseline() will set base value so that histograms are corrected for baseline
      // 	trace.SetMSPS(250);
      // 	ostringstream histoName;
      // 	histoName << "traceFrontImplant_" << counterList.returnValue("foundIon") << "_strip=" << fImplantEFMaxStrip <<"_Tree=" << dataChain.GetTreeNumber() << endl;
      // 	trace.GetTraceHisto()->SetName(histoName.str().c_str());
      // 	Histo->traceHistos.push_back(trace.GetTraceHisto());
      // }


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

      Event frontImplantTest;// = DSSDloGainFront.maxEraw();
      Event backImplantTest;// = DSSDloGainBack.maxEraw();

      int implantFrontTest =  -100;
      int implantBackTest = -100;

      if(DSSDloGainFront.getEventList().size() > 0){implantFrontTest = 40 - (DSSDloGainFront.maxEraw().channel - 103);}
      if(DSSDloGainBack.getEventList().size() > 0){implantBackTest = 40 - (DSSDloGainBack.maxEraw().channel - 183);}

      
      //these unfortunately don't agree, will have to look into it closer
      // if(implantBackTest != fImplantEBMaxStrip){cout << "back E raw sorting not consistent" << endl; cout << implantBackTest << " " << fImplantEBMaxStrip << endl;}
      // if(implantFrontTest != fImplantEFMaxStrip){cout << "front E raw sorting not consistent" << endl; cout << implantFrontTest << " " <<fImplantEFMaxStrip<< endl;}

      
      // fImplantEBMaxStrip = implantBackTest;
      // fImplantEFMaxStrip = implantFrontTest;


      // SeGA.clear();
      // DSSDloGainBack.clear();
      // DSSDloGainFront.clear();
      // DSSDhiGainBack.clear();
      // DSSDhiGainFront.clear();

      //cout << "here1" << endl;
      //cout << "here2" << endl;

      //Histo->h_PID_gated->Fill(curTOF,PIN1energy);
      // cout << endl;
      // cout << "Implant Front Strip = " << fImplantEFMaxStrip << endl;
      // cout << "Implant Back Strip = " << fImplantEBMaxStrip << endl;
      // cout << endl;

      //implant->GetBuffer().clear();

      //will now need to check if event is in PID gate before calling correlated events

      // commented line below for just counting PID
      if(!foundIonOfInterest){continue;} //only continue analysis if ion of interest is found
      counterList.count("foundIon");

      if(fImplantEFMaxStrip == -100 && fImplantEBMaxStrip == -100){counterList.count("lostIonNoImplantation");continue;} 
      if(fImplantEFMaxStrip == -100 || fImplantEBMaxStrip == -100){counterList.count("lostIonOneStripImplantation");continue;}

      

      Histo->h_PID_gated->Fill(curTOF,PIN1energy);

      // cout << "found implant event : " << counterList.returnValue("foundIon") << endl;
      // cout << "front strip : " << fImplantEFMaxStrip << endl;
      // cout << "back strip  : " <<fImplantEBMaxStrip << endl;

      // DSSDloGainFront.Print();
      // DSSDloGainBack.Print();

      //continue; //I'm lazy and don't want to go through correlation loop

      //would be very nice, if everytime that an implant is found, so long as it doesn't overlap in space with previous implant (within stripTolerance),
      //that I would do another correlation search for an event in that region of the detector
      //I think this could be parallelized
      //not sure if its better to parallelize the main for loop or to parallelize searches
      
      //need to make search for decay events here
      bool firstEvent = true;
      do{
	//cout << "in correlation loop" << endl;

	decay->clear();
	bool implantEvent = false;
      	if(lastEntry >= dataChain.GetEntries()){break;}
      	lastEntry = decay->FillBuffer(dataChain, lastEntry+1);

	if( (decay->GetBuffer().back().time - implantTime) < waitWindow ){continue;}

	
	//I may have better luck if I remove this second trigger condition, as I will be considering whether there is a front and back below
      	if(decay->isTriggered( triggerCondition ) ){
	
	  lastEntry = decay->GetCoinEvents(dataChain); //decay buffer will be filled with a list of coincidence Events	
	  decay->dumpBuffer(); //dumpbuffer will send directly to activated 'RBDDdets' but not activated 'RBDDarrays'
	//it fills data in RBDDarrays now
	  implantEvent = PIN1.getEvents().size() > 0;
	  //PIN1.clear(); //always clear right after you're finished with the data
	  //TOF.clear();
	  // if(implantEvent && (PIN1.getEvents()[0].time - implantTime) < 3E8){

	  //   // Event frontImplant = DSSDloGainFront.maxEraw();
	  //   // Event backImplant = DSSDloGainBack.maxEraw();
	    

	  //   //need to clear events before dumping buffer again, no matter what
	  //   // SeGA.clear();
	  //   // DSSDhiGainBack.clear();
	  //   // DSSDhiGainFront.clear();
	  //   // DSSDloGainFront.clear();
	  //   // DSSDloGainBack.clear();
	  //   //decay->clear(); //all clearing can be done with TriggeredEvent class
	  //   break;
	  //   //continue;
	  //   //just for fun below
	  //   //break;
	  // }

	  //just for fun
	  //if(implantEvent){break;}
	  
      	  //need to fillArray for RBDDarrays to get list of buffer events, I know not the smartest
      	  // SeGA.fillArray();
      	  // DSSDhiGainBack.fillArray();
      	  // DSSDhiGainFront.fillArray();

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

	  //cout << "really?" << endl;
	  bool foundFront = DSSDhiGainFront.getEventList().size() > 0;
	  bool foundBack = DSSDhiGainBack.getEventList().size() > 0;

      	  if(foundBack && foundFront){
	    frontDecay  = DSSDhiGainFront.maxE();
	    frontDecayAddBack  = DSSDhiGainFront.addBack();
	    backDecay = DSSDhiGainBack.maxE();
	  }
      	  else{
      	    //cout << "no back event found" << endl;
      	    //only want events where both a front and back strip fired
	    // SeGA.clear();
	    // DSSDhiGainBack.clear();
	    // DSSDhiGainFront.clear();
	    // DSSDloGainFront.clear();
	    // DSSDloGainBack.clear();
	    //decay->clear();
      	    continue;
      	  }

      	  int backStrip = backDecay.channel - 144;
      	  int frontStrip = frontDecayAddBack.channel - 64;

      	  //backDecay = DSSDhiGainBack.addBack();

      	  //just to check that sorting is working properly, and it is (highest energy is first in listt)!
      	  //also want to check addback now
      	  //addback is working
      	  // for(auto & event : DSSDhiGainBack.getEventList()){
      	  //   cout << "channel: " << event.channel<< endl;;
      	  //   cout << "E: " << event.energy << endl;
      	  //   cout << endl;
      	  // }

      	  // cout << "front strip max E: " << frontDecay.channel << endl;
      	  // cout << "back strip max E: " << backDecay.channel << endl;
      	  // cout << "back strip energy: " << backDecay.energy << endl;

	  // cout << "Decay Trigger" << endl;
	  // cout << "front max E strip : " << frontStrip << endl;
	  // cout << "back max E strip  : " << backStrip << endl;
	  // cout << endl;
	  
	  // cout << "Fronts-> " << endl;
	  // DSSDhiGainFront.Print();
	  // cout << "Backs-> " << endl;
	  // DSSDhiGainBack.Print();
	  //continue;
	  
      	  double Ethreshold = 100;
      	  double stripTolerance = 3.;
	  RBDDTrace test(frontDecay.trace);

	  // if(!test.filter()){
	  //   cout << endl;
	  //   cout << "signal = " << frontDecay.signal;
	  //   cout << endl;
	  //   cout << "energy = " << frontDecay.energy;
	  //   cout << endl;
	  //   cout << "strip = " << frontStrip;
	  //   test.Print();
	  // }



	  //main difference I can find between logic below and that from sort3_v2
	  //is that sort3_v2 searches through all front+back events for something within stripTolerance
	  //I will try implementing something similar for testing
	  //Implementing above recovers similar results from previous sorting, as it should!

	  for(auto& frontEvent : DSSDhiGainFront.getEventList()){
	    frontStrip = frontEvent.channel - 64.;

	    if(frontEvent.energy <1500){
	      RBDDTrace test2(frontEvent.trace);
	      if(!test2.filter()){ // returns true if passes filter
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
		    //if(frontDecayAddBack.energy > 3000 && frontDecayAddBack.energy < 3800){
		    Histo->hGammaEnergyG->Fill(segaEvent.energy);
	  	    //}
		  }

		}

		for(auto & segaEvent : SeGA.getEventList()){
		  Histo->hGammaEnergy->Fill(segaEvent.energy);
		  //if(frontDecayAddBack.energy > 3000 && frontDecayAddBack.energy < 3800){
		  //Histo->hGammaEnergyG->Fill(segaEvent.energy);
		  //}
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

	//decay->clear();
	// SeGA.clear();
	// DSSDhiGainBack.clear();
	// DSSDhiGainFront.clear();
	// DSSDloGainFront.clear();
	// DSSDloGainBack.clear();


      } while(  abs(decay->GetBuffer().back().time-implantTime) < corrWindow );


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


  // Some computation here
  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
	    << "elapsed time: " << elapsed_seconds.count() << "s\n";  
  cout << endl;

  return 1; //cuz I'm old school
}
