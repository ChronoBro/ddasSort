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

  //RBDDarray DSSDloGainFront = setup.getArray("DSSDloGainFront");
  //RBDDarray DSSDloGainBack = setup.getArray("DSSDloGainBack");
  
  RBDDarray DSSDhiGainFront = setup.getArray("DSSDhiGainFront");
  RBDDarray DSSDhiGainBack = setup.getArray("DSSDhiGainBack");
  
  RBDDarray Clovers = setup.getArray("Clovers");
  
  
  // Load PID gate

  // TFile *fGateFile = new TFile("root-files/PIDGates2.root","READ");
  TFile *fGateFile = new TFile("PIDGates2.root","READ");
  TCutG *fGate;          //! PID Gate
  TCutG *fGate72Rb;
  TCutG *fGate70Kr;
  TCutG *fGate74Sr;
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
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
  RBDDdet PIN2 = setup.getDet("PIN2");
  RBDDdet PIN1_XFP = setup.getDet("PIN1_XFP");
  RBDDdet PIN2_XFP = setup.getDet("PIN2_XFP");
  RBDDdet PIN1_RF = setup.getDet("PIN1_RF");

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
  eventHandler->setTriggerCh(181);

  eventHandler->activateDetector(PIN1);
  eventHandler->activateArray(Clovers);
  eventHandler->activateDetector(PIN1_XFP);
  //eventHandler->activateArray(DSSDhiGainFront);
  //eventHandler->activateArray(DSSDhiGainBack);
  //eventHandler->activateArray(SSD);

   // eventHandler->activateArray(DSSDloGainFront);
  // eventHandler->activateArray(DSSDloGainBack);
  // eventHandler->activateDetector(XFP_CFD);

  std::vector<ionCorrelator> implantedIonList;
  long unsigned int oldIonNumber = 0;

  std::vector<ionCorrelator> decayIonList;


  double firstTime = 0;
  double lastTime = 0;
  bool alreadyTriggered = false;

  
  for(long long int iEntry=0;iEntry<fNEntries;iEntry=lastEntry+1){

    bool needIonOverlapCheck = oldIonNumber != implantedIonList.size();

    //Clear Detectors defined outside of event loop that had they're data operated on already
    eventHandler->clear();

    //normal use
    lastEntry = eventHandler->FillBuffer(dataChain, iEntry);  

    Histo->expSummary->Fill(eventHandler->GetBuffer().back().channel,eventHandler->GetBuffer().back().signal);
    
    double curTime = eventHandler->GetBuffer().back().time;
    lastTime = curTime;

  } //end of loop over entries
    

  
  Histo->write(); // this forces the histrograms to be read out to file

  cout << "Total number of Ions of Interest found: " << counterList.returnValue("foundIon") << endl;

  cout << endl;

  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

  std::cout << "finished computation at " << std::ctime(&end_time)
	    << "elapsed time: " << elapsed_seconds.count() << "s\n";  
  cout << endl;

  return 1; //cuz I'm old school
}
