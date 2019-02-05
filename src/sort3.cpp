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
  vector<int> frontChannels;
  vector<int> backChannels;
  vector< vector<double> > frontCalib;
  vector< vector<double> > backCalib;

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
    fh_ChannelDelays = new TH1D("h_Delays","",300,0,300);
  }
  
  // Load PID gate

  TFile *fGateFile = new TFile("PIDGates.root","READ");
  fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_71Kr"));
  //fGate = new TCutG(*(TCutG*)fGateFile->FindObjectAny("cut_73Sr"));
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

  for(long long int iEntry=0;iEntry<dataChain.GetEntries();iEntry=lastEntry+1){

    if(pDDASEvent->GetNEvents()>1) {cerr << "-->ERROR: Must be run on non-built event data." << endl; exit(0);} //each DDASEvent has 1 ddaschannel

    /*****************************************
   
    EVENT BUILDER

    ****************************************/

    int trigChan =0; //channel for PIN1 at front of stack
    ddasDet trigger(trigChan);
    ddasDet TOF(5); // PIN2-XFP TAC
    
    double coinWindow     =  5000;//5000;//2000;        // Time (ns)
    double promptWindow   =  2000;
    double coinGammaWindow=  1000;        // Time (ns)
    double corrWindow     =  1E9;        // Time (ns)  now 1000ms (1s)
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
    //dump buffer into detectors
    for(auto & bufferEvent : buffer.getEvents()){

      foundTOF = TOF.fillEvent(bufferEvent);

      if(foundTOF){
	if(TOF.getFillerEvent().signal > curTOF){curTOF = TOF.getFillerEvent().signal;}
	//cout << "FoundTOF" << endl;
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

    if(checkSEGA){ //readout prompt gammas
      for(auto & SEGAdet : SEGA){
	
	for(auto segaEvent : SEGAdet.getEvents()){
	  Histo->hPromptGammaEnergy->Fill(segaEvent.energy);
	}

      }
    }

    //curTOF = TOF.getFillerEvent().signal;
    Histo->h_PID->Fill(curTOF, PIN1energy);


    if( fGate->IsInside(curTOF,PIN1energy) ){//&& checkBack && checkFront && checkTOF){ 
                                              //once I have routine for checking DSSD events, should require that there are front+back events
                                              // DONE
      foundIonOfInterest = true;              //--->can also add other gates to find other ions
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

    //cout << "Front Implant Strip = " << fImplantEFMaxStrip << endl;
    //cout << "Back Implant Strip = " << fImplantEBMaxStrip << endl;

    buffer.clear();

    //now look for correlations


    do{
      bool foundDecay = false;
      bool foundDSSD = false;
      bool secondImplant = false;
      double DSSDtime = 0;

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
	  foundDSSD = true;
	}
      }

      for(auto & backs : DSSDbackHighGain){
	if(backs.fillEvent(buffer.getFillerEvent())){
	  DSSDtime = buffer.getFillerEvent().time;
	  foundDSSD = true;	  
	}
      }


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


      if(secondImplant){break;}

      //cout << "found DSSD" << endl;
      //cout << buffer.getEvents().size() << endl;

      ddasDet decayEventsFront(-1);
      ddasDet decayEventsBack(-1);
      ddasDet segaEvents(-1);

      //dump buffer into detectors
      for(auto & bufferEvent : buffer.getEvents()){

	for(auto & DSSDfront : DSSDfrontHighGain){
	  if(DSSDfront.fillEvent(bufferEvent)){
	    decayEventsFront.fillEvent(DSSDfront.getFillerEvent()); //event gets calibrated when filler, if filled with buffer event energy=-1

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

      }

      //cout << decayEventsFront.getEvents().size() << endl;
      //cout << decayEventsBack.getEvents().size() << endl;

      double EdiffThreshold = 200; //keV
      double Ethreshold = 100; //keV
      int stripTolerance = 2;
      double decayTime = corrWindow;
      double decayEnergy = 0;
      double EmaxDecay = 0;
      double reportedE = 0;
      double reportedTime = 0;

      for(auto & decayEventFront : decayEventsFront.getEvents()){

	int frontStrip = decayEventFront.channel - 64;
	decayEnergy = decayEventFront.energy;
	decayTime = decayEventFront.time - triggerTime;//trigger.getFillerEvent().time; <--- bad idea since I was checking for second implant

	//cout << "front strip = " << frontStrip << endl;
	//cout << "Front E = " << decayEnergy << endl;
	//cout << "decay time = " << decayTime << endl;

	for(auto & decayEventBack : decayEventsBack.getEvents()){
	    
	  //double Tdiff = abs(decayEventFront.time - decayEventBack.time);
	  double Ediff = abs(decayEventFront.energy - decayEventBack.energy);
	  int backStrip = decayEventBack.channel - 144;
	    
	  //cout << "back strip = " << backStrip << endl;
	  //cout << "Back E = " << decayEventBack.energy << endl;

	  if(Ediff < EdiffThreshold
	     && decayEventBack.energy > Ethreshold 
	     && decayEventFront.energy > Ethreshold 
	     && abs(fImplantEFMaxStrip - frontStrip) < stripTolerance 
	     && abs(fImplantEBMaxStrip - backStrip) < stripTolerance 
	     && decayEnergy > EmaxDecay
	     ){

	    reportedE = decayEnergy;
	    reportedTime = decayTime;
	    EmaxDecay = decayEnergy;
	    foundDecay = true;

	  }
	    

	}
      }




      if(foundDecay){
	//cout << "Found Decay Event!" << endl;
	//dump gamma events
	for(auto & segaEvent : segaEvents.getEvents()){
	  Histo->hGammaEnergy->Fill(segaEvent.energy);
	  Histo->hGammaVsDecay->Fill(decayEnergy,segaEvent.energy);
	}
	  
	Histo->hDecayEnergy->Fill(reportedE);
	Histo->hDecayTime->Fill(reportedTime);
	
	break;
      }



    } while(abs( buffer.getFillerEvent().time - triggerTime) < corrWindow );
   
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
    

    //could fill a tree with only DDASEvents that comprise of specific isotope
    //could make class that does this so long as there is a specified PID file and PID Gate file

  } //end of loop over entries

  Histo->write(); // this forces the histrograms to be read out to file

  return 1;
}
