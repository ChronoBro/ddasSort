
//
//  RBDDTriggeredEvent.cxx
//
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include "RBDDTriggeredEvent.h"

using namespace std;

void RBDDTriggeredEvent::setNull(){

  for(int i=0;i<maxChannels;i++){
    liveDets[i] = NULL;
  }

}

void RBDDTriggeredEvent::loadBar(double progress){

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



//______________________________________________________________________________
// void RBDDTriggeredEvent::Copy(TObject &obj) const
// {
//   // --
//   //
  
// //  ((RBPixie4eEvtHeader&)obj).fEvtPattern->Copy(*fEvtPattern);
// //  ((RBPixie4eEvtHeader&)obj).fEvtInfo->Copy(*fEvtInfo);
// //  for(Int_t iBit = 0; iBit<16; iBit++){
// //    ((RBPixie4eEvtHeader&)obj).fEvtPattern->SetBitNumber(iBit,fEvtPattern->TestBitNumber(iBit));
// //    ((RBPixie4eEvtHeader&)obj).fEvtInfo->SetBitNumber   (iBit,fEvtInfo->   TestBitNumber(iBit));
// //  }
// //  ((RBPixie4eEvtHeader&)obj).fNumTraceBlksPrev = fNumTraceBlksPrev;
// //  ((RBPixie4eEvtHeader&)obj).fTrigTimeHI       = fTrigTimeHI;
// //  ((RBPixie4eEvtHeader&)obj).fTrigTimeX        = fTrigTimeX;
// }



//______________________________________________________________________________
void RBDDTriggeredEvent::Print(Option_t *) const
{
  // --
  //
  
//  cout << "==================== XIA RBPixie4eHeader (BEGIN) ====================" << endl;
//  cout << "*  Block size       : " << fBlkSize << endl;         // Block size (16-bit words)
//  cout << "*  Module number    : " << fModNum << endl;          // Module number
//  cout << "*  Run type         : " << fRunFormat << endl;       // Format descriptor = Run Type
//  cout << "*  Ch header length : " << fChanHeadLen << endl;     // Channel Header Length
//  cout << "*  Coincidence Pat. : " << fCoincPat << endl;        // Coincidence pattern
//  cout << "*  Coincidence Win. : " << fCoincWin << endl;        // Coincidence window
//  cout << "*  Max. length      : " << fMaxCombEventLen << endl; // Maximum length of traces plus headers from all 4 channels (in blocks)
//  cout << "*  Event length 0   : " << fEventLength0 << endl;    // Length of traces from channel 0 plus header (in blocks)
//  cout << "*  Event length 1   : " << fEventLength1 << endl;    // Length of traces from channel 1 (in blocks)
//  cout << "*  Event length 2   : " << fEventLength2 << endl;    // Length of traces from channel 2 (in blocks)
//  cout << "*  Event length 3   : " << fEventLength3 << endl;    // Length of traces from channel 3 (in blocks)
//  cout << "==================== XIA RBPixie4eHeader (END)   ====================" << endl;
  
}


//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel &ch,
                                       Double_t windowWidth)
{
  // --
  //
  setWindowWidth(windowWidth);
  setNull();
  //fillerChannel = ch;
}

RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel* ch,
                                       Double_t windowWidth)
{
  // --
  //
  setWindowWidth(windowWidth);
  fillerChannel = ch;
  setNull();
}

RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel* ch,
                                       Double_t windowBegin, Double_t windowEnd)
{
  // --
  //
  setWindowWidth(windowEnd-windowBegin);
  fillerChannel = ch;
  setNull();
}


//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel &ch,
                                       Double_t windowBegin,Double_t windowEnd)
{
  // --
  //
  setWindowWidth(windowEnd-windowBegin);
  setNull();
  //fillerChannel = ch;
}

//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,
                                       Double_t windowWidth)
{
  // --
  //
  setWindowWidth(windowWidth);
  setNull();
}

//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,
                                       Double_t windowBegin,Double_t windowEnd)
{
  // --
  //
  setWindowWidth(windowEnd-windowBegin);
  setNull();
}

//______________________________________________________________________________
void RBDDTriggeredEvent::SetTriggerEvent(RBDDChannel *ch, Long64_t entry)
{
  // --
  //

}

//______________________________________________________________________________
void RBDDTriggeredEvent::ResetTrigger()
{
  // --
  //
}

//______________________________________________________________________________
TList* RBDDTriggeredEvent::GetEvents(RBDDChannel *ch, TBranch *br)
{
  // --
  //

  //It appears andy was going to do unpacking here, but if we want to make this a general class then that shouldn't be done here
  //The unpacking is done by the derived RBDDChannel class (e.g. RBDDASChannel), so we can call unpacking here
  
  // Check that trigger event is set.
  // if(!IsTriggered()) {cerr << "---> Trigger was not set!" << endl; return 0;}
  
  // // Initialize variables to keep track of current event.
  // UShort_t  curChanNo     = *fGlobalCh;
  // ULong64_t curTimestamp  = fData->fChData.fTimestamp;
  // Long64_t  nextTDiff     = 0;
  // Double_t  CFDTimeDiff   = 0;
  // Double_t  curCFDAbsTime = fData->fChData.GetCFDAbs(0);
  // Int_t     numTraceWords = fData->fChData.fNumTraceWords;
  // fSearchEntry            = entry;
  
  // // Search for events within the window.
  // nextTDiff   = 0;
  
  // // Go to beginning of window.
  // Bool_t foundWinBegin = kFALSE;
  // if(GetTriggerEntry()==0){ fSearchEntry = 0; foundWinBegin = kTRUE;}
  // else                      fSearchEntry = GetTriggerEntry()-1;
  // while(!foundWinBegin){
  //   br->GetEntry(fSearchEntry);
  //   nextTDiff = (Long64_t)(fTriggerChannel->GetTimestamp() - ch->GetTimestamp());
    
  //   if(nextTDiff > fWindow/2) {foundWinBegin = kTRUE; fSearchEntry++;}
  //   else                      fSearchEntry--;
  //   if(fSearchEntry<1)        {foundWinBegin = kTRUE;  fSearchEntry=0;}
  // }
  
  
  // // Search through window as long there are still events.
  // Int_t     nCh   = 0;
  // Double_t  sumE  = 0;
  // fPSDValTrig     = 0;
  // fCFDValTrig     = 0;
  // while(fSearchEntry+1<fNEntries){
  //   b_fGlobalCh ->GetEntry(fSearchEntry); 
  //   b_fData     ->GetEntry(fSearchEntry);
  //   nextTDiff   = (Long64_t)(fData->fChData.fTimestamp - curTimestamp);
  //   CFDTimeDiff = fData->fChData.GetCFDAbs(0) - curCFDAbsTime;
    
  //   // Check that the event is within the coincidence window.
  //   // Update the entry for the close of the window.
  //   if(nextTDiff+fWindow/2>fWindow) {fNextEventEntry = entry+1; break;}
    
  //   // cout << "---# " << fSearchEntry << " " << *fGlobalCh << " " << nextTDiff << " " << endl;
    
  //   // Fill ch energy.
  //   hERaw[*fGlobalCh]->Fill(fData->fChData.fEnergy);
  //   hE[*fGlobalCh]->Fill(fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0]);
    
  //   Int_t numTraceWords = fData->fChData.fNumTraceWords;
  //   // Read trace.
  //   std::vector<Double_t> curTrace;
  //   for(Int_t i=0; i<numTraceWords; i++) curTrace.push_back((Double_t)fData->fChData.fTrace[i]);
    

    
  //   // Found a signal.
  //   // Check that the channel is not the trigger channel.
  //   // THIS IS WRONG IN GENERAL, only works since trig ch is 0
  //   //    if(*fGlobalCh == fTrigCh) {
  //   //      fPSDValTrig = fPSDVal;
  //   //      fQDCTCTrig = (fQDCT+fRandom.Uniform())*fCalPar[*fGlobalCh][1]+fCalPar[*fGlobalCh][0];
  //   //      fCFDValTrig = fCFDTime;
  //   //      belowCFDTrig = belowCFD;
  //   //    }
  //   if(*fGlobalCh == fTrigCh && fSearchEntry == entry) {
  //     if(fData->fChData.fEnergy>0){
  //       nCh++;
  //       sumE += fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0];
  //       fPSDValTrig = fPSDVal;
  //     }
  //     fSearchEntry++;
  //   }
  //   else{
  //     // We found a detector in coincidence.
  //     //      if(!belowCFD) hDelta   ->Fill(fCFDTime - fCFDValTrig);
  //     //hDeltaCFD->Fill(CFDTimeDiff);
  //     //      hDeltaCFD->Fill(fCFDTime - fCFDValTrig);
  //     if(fData->fChData.fEnergy>0){
  //       hDelta->Fill(nextTDiff);
  //       nCh++;
  //       sumE += fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0];
  //     }
      
  //     fSearchEntry++;
  //   }
  //   //cout << "-- >>> " << *fGlobalCh << " " << fPSDVal << " " << fPSDValTrig << endl;
  // }
  
  // return GetEvents();
}

long long int RBDDTriggeredEvent::FillBuffer(TChain &dataChain, long long int iEntry){

  //put load bar in here

  // if(lastEntry > dataChain.GetEntries()){
  //   std::cout << "WTF" <<std::endl;
  //   std::cout << "WTF" <<std::endl;
  // }

  dataChain.GetEntry(iEntry);
  fillerChannel->unpack();
  //bufferTest.push_back(fillerChannel); //is this the memory leak? commented this one line and it largely went away so this contributed.
  // cout << "fillerChannel time = " <<fillerChannel->GetTimestamp() << endl;
  // cout << "fillerChannel chan = " <<fillerChannel->GetChanNo() << endl;
  buffer.push_back(fillEvent(fillerChannel));

  lastEntry = iEntry;

  //memory leak was occuring in the program where buffer was being filled indefinitely
  //I believe this is an issue where events aren't being erased from the buffer
  //throwing in abs() in time check made this problem go away as far as I can tell

  //I think something is wrong with how my buffer is being filled... 5/2/2019

  //should also pop events off the front if they aren't in coincidence with latest event

  // for(auto & bufferEvent : buffer ){
  //   double timeDiff = abs(fillerChannel->GetTimestamp()-bufferEvent.time);
  //   if(timeDiff > fWindowWidth){buffer.erase( buffer.begin() );} 
  //   else{break;}
  // }

  //below seems to work better
  //it does but now I'm losing the triggered events in the buffer... which means I'm losing ALL good events presumably,
  //would explain why I'm not seeing any decays, but I do think the below code is more in line with my original goals.
  //below code throws out almost half the events and I don't know why 5/2/2019

  for(std::vector<Event>::iterator it = buffer.begin(); it != buffer.end();) {
    double timeDiff = abs(fillerChannel->GetTimestamp()-(*it).time); //need parentheses around iterator for this to work
    if(timeDiff > fWindowWidth){it = buffer.erase(it);}
    else{it++;}
  }

   double progress0 =  (double)lastEntry/(double)dataChain.GetEntries();
   if(lastEntry % 10000 == 0){   
     loadBar(progress0);    
   }


   return lastEntry;

}

long long int RBDDTriggeredEvent::GetCoinEvents(TChain &dataChain){

  //have to figure out a scheme to keep the original trigger event in the buffer...


  //triggerChannel = buffer.back(); //setting stuff equal to each other may be dangerous
  
  triggerSignal = buffer.back().signal;
  triggerTime = buffer.back().time;
  int triggerChannel = buffer.back().channel;
  int triggerEnergy = buffer.back().energy;
  std::vector<unsigned short> triggerTrace = buffer.back().trace;

  // triggerSignal = bufferTest.back()->GetSignal();
  // triggerTime = bufferTest.back()->GetTimestamp();

  //fill up buffer with events in coincidence
  for(; abs(buffer.back().time - triggerTime) < fWindowWidth/2. ;){ //buffer was getting filled indefinitely because of 
                                                                    //issues with getting timestap, should be careful about that
  //for(; abs(bufferTest.back()->GetTimestamp()-triggerTime) < fWindowWidth/2.;){
    if(lastEntry >= dataChain.GetEntries()){break;}

    lastEntry = lastEntry +1; //hmmmm.... Idk why lastEntry++ didn't work here but it sure screwed things up!
                              //lets see if buffer works with RBDDChannels now
    

    lastEntry = FillBuffer(dataChain,lastEntry);
    
    // cout << lastEntry+1 << endl;
    // cout << "WTF is going on?!" << endl;
    // cout << buffer.back().time << endl;
    // cout << triggerTime << endl;

  }

  //this should be a quick fix for the trigger not being put with the buffer
  //this would mean that the last event is improperly grouped and that should be fixed ultimately
  if(buffer.back().time - triggerTime > fWindowWidth){ //I originally had fWindowWidth/2. ... this was dumb...
    Event trigger;
    trigger.signal = triggerSignal;
    trigger.time = triggerTime;
    trigger.channel = triggerChannel;
    trigger.energy = triggerEnergy;
    trigger.trace = triggerTrace;
    //buffer.push_front(trigger);
    buffer.insert(buffer.begin(),trigger);

    //this should reset everything correctly 
    buffer.erase(buffer.end());
    lastEntry-1;

  }

  //if one calls FillBuffer first then the buffer should only have events from the first half of window

  return lastEntry;

}


void RBDDTriggeredEvent::triggerSearch(){
  
  

}


Event RBDDTriggeredEvent::fillEvent(RBDDChannel* fillChannel){

  Event fillEvent;
  fillEvent.signal = fillChannel->GetSignal();
  // if(fillChannel->calibrationStatus()){
  //   fillEvent.energy = fillChannel->GetEnergy();
  // }
  fillEvent.channel = fillChannel->GetChanNo();
  fillEvent.time = fillChannel->GetTimestamp();
  fillEvent.trace = fillChannel->GetTrace();

  return fillEvent;

}


bool RBDDTriggeredEvent::dumpBuffer(RBDDdet &det){

  bool foundDet = false;

  for(auto & bufferEvent : buffer){
    bool test = det.fillEvent(bufferEvent);
    foundDet = foundDet || test;
    //foundDet = foundDet ||  det.fillEvent(bufferEvent);
  }

  return foundDet;
}


bool RBDDTriggeredEvent::dumpBuffer(RBDDarray &array){

  bool foundDet = false;

  //cout << endl;
  for(auto & bufferEvent : buffer){
    bool test = array.fillArray(bufferEvent);
    foundDet  = foundDet || test;
    // std::cout << std::endl;
    // std::cout << bufferEvent.channel << std::endl;
    // foundDet = foundDet || array.fillArray(bufferEvent);
    // std::cout << test << std::endl << std::endl;
    //cout << bufferEvent.channel << endl;

  }
  
  // std::cout << "###############################" << std::endl;
  // std::cout << array.getEventList().size() << std::endl;
  // std::cout << "###############################" << std::endl;

  //cout << endl;
  return foundDet;
  
}


bool RBDDTriggeredEvent::dumpBuffer(){

  bool test = false;

  for(auto & bufferEvent : buffer){
    
    if(liveDets[bufferEvent.channel]!=NULL){ //should always check if pointer is NULL otherwise will get segfault

      //tell detector with assigned channel to handle event
      test = liveDets[bufferEvent.channel]->fillEvent(bufferEvent);

      

    }
     
  }

  //force array to populate event list
  for(auto & array : arrayList){
    array->fillArray();
  }
  
  return test;
}


bool RBDDTriggeredEvent::activateDetector(RBDDdet &det){

  if(det.getAssignedChannel() > maxChannels){
    cout << "need to increase maximum channels" << endl;
    return false;
  }

  //set the address of pointer to detector object
  liveDets[det.getAssignedChannel()] = &det;
  return true;

}

bool RBDDTriggeredEvent::activateArray(RBDDarray &array){

  for(auto & det : array.getDetectors()){ //vector is a list of pointers now
 
    //cout <<  det->getAssignedChannel() << endl;
    if(det->getAssignedChannel() > maxChannels){
      cout << "need to increase maximum channels" << endl;
      return false;
    }

    //set the address of pointer to detector object
    liveDets[det->getAssignedChannel()] = det;   
  }

  //store pointer to array objects to call filling
  arrayList.push_back(&array);

  return true;
}
  
void RBDDTriggeredEvent::clear(){
  
  for(int i=0;i<maxChannels;i++){
 
    if(liveDets[i]!=NULL){ //should always check if pointer is NULL otherwise will get segfault
   
      liveDets[i]->clear();
    }
  }

  for(auto & array : arrayList){
    array->clear();
  }


}

void RBDDTriggeredEvent::Print(){

  for(int i=0;i<maxChannels;i++){

    if(liveDets[i]!=NULL){

      liveDets[i]->Print();
    }

  }


}
