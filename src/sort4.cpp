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
#include "diagnostics.h"
#include "RBDDTriggeredEvent.h"
#include "RBDDASChannel.h"
#include "RBDDarray.h"
#include "RBDDTrace.h"

using namespace std;

//shoutout to PherricOxide on StackOverflow for a quick test if a file exists
inline bool exists_test (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

bool triggerCondition(int Chan){

  if(Chan >= 64 && Chan < 184){
    return true;
  }

  return false;
}

int main(int argc,char *argv[]){

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

  RBDDASChannel * bufferEvent = new RBDDASChannel;
  bufferEvent->setEventPointer(pDDASEvent);

  //clock ticks are in ns for DDAS
  double coinWindow = 5000; //5 us
  double coinWindow2 = 10000;
  double corrWindow = 2E9;  //2 s

  RBDDTriggeredEvent* ev1 = new RBDDTriggeredEvent("Prompt Trigger", "title", bufferEvent, coinWindow);
  ev1->setTriggerCh(0);
  //ev1->activateDetector(PIN1);
  ev1->activateDetector(TOF);
  //ev1->activateDetector(XFP_CFD);

  ev1->activateArray(SeGA);
  ev1->activateArray(DSSDloGainFront);
  ev1->activateArray(DSSDloGainBack);



  //ev1->setTriggerCh(6); //for testing to see if all XFP signals are found
  RBDDTriggeredEvent* ev2 = new RBDDTriggeredEvent("Correlated Events", "title", bufferEvent, coinWindow);
  ev2->setTriggerCh(0);

  RBDDTriggeredEvent* XFP = new RBDDTriggeredEvent("XFP Trigger", "title", bufferEvent, coinWindow);
  XFP->setTriggerCh(6);

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry+1){

    /*************************
    need to figure out my buffering issues... 5/2/2019
    i.e. ->not grouping together events perfectly, so I'm throwing away events
    *************************/

    //normal use
    lastEntry = ev1->FillBuffer(dataChain, iEntry);

    //just for getting XFP counts
    // lastEntry = XFP->FillBuffer(dataChain,iEntry);
    // if(XFP->isTriggered()){counterList.count("XFP_CFD");}

    //cout << ev1->fillerChannel->GetChanNo() << endl;
    
    
    //it appears the buffer is just being filled indefinitely after being triggered... curious
    //fixed issue, there was an issue with assigning timestamps in RBDDChannel after unpacking (conversion between int and double) that was reporting the same time for each event, thus my buffer checks weren't working and it was being filled indefinitely
    
    // 5/1/2019 -> Buffer filling doesn't appear to be working properly which is then screwing up the rest of data
    // 5/2/2019 -> Buffer appears to working now, even better than sort3_v2 in that sort4 is picking up more implantation events

    //if(false){ // I'm lazy and want to turn off triggerloop for XFP counting
    if(ev1->isTriggered()){


       bool foundTOF = false;
       bool foundIonOfInterest = false;
       double curTOF = 0;
       double implantTime;
       int fImplantEFMaxStrip = -100;
       int fImplantEBMaxStrip = -100;

       lastEntry = ev1->GetCoinEvents(dataChain); //ev1 buffer will be filled with a list of coincidence Events
       double PIN1energy = ev1->triggerSignal;

       // //dumpBuffer() now pushes event data to exact detector for the channel, this should reduce number of loops when filling events
       // //should make a very big difference when analyzing large number of events
       // ev1->dumpBuffer();

       // //kind of stupid but you have to fillArrays after bufferdump. yes its bad design
       // SeGA.fillArray();
       // DSSDloGainBack.fillArray();
       // DSSDloGainFront.fillArray();
       // DSSDhiGainBack.fillArray();
       // DSSDhiGainFront.fillArray();
       

       // cout << endl;
       // for(auto& event : ev1->GetBuffer()){
       // 	cout << event.channel << endl;
       // }
       // cout << endl;

       foundTOF = ev1->dumpBuffer(TOF);

       //if(ev1->dumpBuffer(XFP_CFD)){counterList.count("XFP_CFD");}
       //cout << TOF.getEvents().size() << endl;

       if(TOF.getEvents().size()>0){
    	 curTOF = TOF.getFillerEvent().signal;
    	 Histo->h_PID->Fill(curTOF, PIN1energy);
       }

       //clear detectors after their data has been used
       TOF.clear();
       //XFP_CFD.clear();

      foundIonOfInterest = fGate->IsInside(curTOF,PIN1energy);
      if(fGate74Sr->IsInside(curTOF,PIN1energy)){counterList.count("found74Sr");}
      if(fGate72Rb->IsInside(curTOF,PIN1energy)){counterList.count("found72Rb");}
      if(fGate70Kr->IsInside(curTOF,PIN1energy)){counterList.count("found70Kr");}
      //if(!foundIonOfInterest){continue;} //only continue analysis if ion of interest is found
      //counterList.count("foundIon");

      //will now need to check if event is in PID gate before calling correlated events

      //dumpBuffer(RBDDarray or RBDDdet) isn't the most efficient since it loops through coincident events each time
      //it seems that dumpBuffer is only ever putting one event into the array event list... need to figure out whats wrong
      //above is DEFINITELY a problem, since the high gain channels fire like crazy and only one event is in the event list... WTF
      //I don't think I changed anything but now it appears to working... wow
      ev1->dumpBuffer(SeGA);
      ev1->dumpBuffer(DSSDloGainFront);
      ev1->dumpBuffer(DSSDloGainBack);

      // cout << endl;
      // cout << SeGA.getEventList().size() << endl;
      // cout << DSSDloGainFront.getEventList().size() << endl;
      // cout << DSSDloGainBack.getEventList().size() << endl;
      // cout << endl;

      // cout << endl;
      // for(auto & bufferEvent: ev1->GetBuffer()){
      // 	cout << bufferEvent.channel << endl;
      // }
      // cout << endl;

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
    	  fImplantEBMaxStrip = 40 - (backEvent.channel - 183);	  
    	  backImplant = backEvent;
    	}

      }	

      //cout << "here1" << endl;

      if(fImplantEFMaxStrip == -100 && fImplantEBMaxStrip == -100){counterList.count("lostIonNoImplantation");continue;} 
      else if(fImplantEFMaxStrip == -100 || fImplantEBMaxStrip == -100){counterList.count("lostIonOneStripImplantation");continue;}

      //cout << "here2" << endl;

      Histo->h_PID_gated->Fill(curTOF,PIN1energy);
      // cout << endl;
      // cout << "Implant Front Strip = " << fImplantEFMaxStrip << endl;
      // cout << "Implant Back Strip = " << fImplantEBMaxStrip << endl;
      // cout << endl;

      //clear detectors defined outside of event loop that had they're data operated on already
      SeGA.clear();
      DSSDloGainBack.clear();
      DSSDloGainFront.clear();
      DSSDhiGainBack.clear();
      DSSDhiGainFront.clear();
      //ev1->GetBuffer().clear();

      //need to make search for decay events here




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

  return 1; //cuz I'm old school
}
