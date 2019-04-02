
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

//______________________________________________________________________________
void RBDDTriggeredEvent::Copy(TObject &obj) const
{
  // --
  //
  
//  ((RBPixie4eEvtHeader&)obj).fEvtPattern->Copy(*fEvtPattern);
//  ((RBPixie4eEvtHeader&)obj).fEvtInfo->Copy(*fEvtInfo);
//  for(Int_t iBit = 0; iBit<16; iBit++){
//    ((RBPixie4eEvtHeader&)obj).fEvtPattern->SetBitNumber(iBit,fEvtPattern->TestBitNumber(iBit));
//    ((RBPixie4eEvtHeader&)obj).fEvtInfo->SetBitNumber   (iBit,fEvtInfo->   TestBitNumber(iBit));
//  }
//  ((RBPixie4eEvtHeader&)obj).fNumTraceBlksPrev = fNumTraceBlksPrev;
//  ((RBPixie4eEvtHeader&)obj).fTrigTimeHI       = fTrigTimeHI;
//  ((RBPixie4eEvtHeader&)obj).fTrigTimeX        = fTrigTimeX;
}



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
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel *ch,
                                       Double_t windowWidth)
{
  // --
  //
}

//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel *ch,
                                       Double_t windowBegin,Double_t windowEnd)
{
  // --
  //
}

//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,
                                       Double_t windowWidth)
{
  // --
  //
}

//______________________________________________________________________________
RBDDTriggeredEvent::RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,
                                       Double_t windowBegin,Double_t windowEnd)
{
  // --
  //
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
  
  // Check that tigger event is set.
  if(!IsTriggered()) {cerr << "---> Trigger was not set!" << endl; return 0;}
  
  // Initialize variables to keep track of current event.
  UShort_t  curChanNo     = *fGlobalCh;
  ULong64_t curTimestamp  = fData->fChData.fTimestamp;
  Long64_t  nextTDiff     = 0;
  Double_t  CFDTimeDiff   = 0;
  Double_t  curCFDAbsTime = fData->fChData.GetCFDAbs(0);
  Int_t     numTraceWords = fData->fChData.fNumTraceWords;
  fSearchEntry            = entry;
  
  // Search for events within the window.
  nextTDiff   = 0;
  
  // Go to beginning of window.
  Bool_t foundWinBegin = kFALSE;
  if(GetTriggerEntry()==0){ fSearchEntry = 0; foundWinBegin = kTRUE;}
  else                      fSearchEntry = GetTriggerEntry()-1;
  while(!foundWinBegin){
    br->GetEntry(fSearchEntry);
    nextTDiff = (Long64_t)(fTriggerChannel->GetTimestamp() - ch->GetTimestamp());
    
    if(nextTDiff > fWindow/2) {foundWinBegin = kTRUE; fSearchEntry++;}
    else                      fSearchEntry--;
    if(fSearchEntry<1)        {foundWinBegin = kTRUE;  fSearchEntry=0;}
  }
  
  
  // Search through window as long there are still events.
  Int_t     nCh   = 0;
  Double_t  sumE  = 0;
  fPSDValTrig     = 0;
  fCFDValTrig     = 0;
  while(fSearchEntry+1<fNEntries){
    b_fGlobalCh ->GetEntry(fSearchEntry);
    b_fData     ->GetEntry(fSearchEntry);
    nextTDiff   = (Long64_t)(fData->fChData.fTimestamp - curTimestamp);
    CFDTimeDiff = fData->fChData.GetCFDAbs(0) - curCFDAbsTime;
    
    // Check that the event is within the coincidence window.
    // Update the entry for the close of the window.
    if(nextTDiff+fWindow/2>fWindow) {fNextEventEntry = entry+1; break;}
    
    // cout << "---# " << fSearchEntry << " " << *fGlobalCh << " " << nextTDiff << " " << endl;
    
    // Fill ch energy.
    hERaw[*fGlobalCh]->Fill(fData->fChData.fEnergy);
    hE[*fGlobalCh]->Fill(fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0]);
    
    Int_t numTraceWords = fData->fChData.fNumTraceWords;
    // Read trace.
    std::vector<Double_t> curTrace;
    for(Int_t i=0; i<numTraceWords; i++) curTrace.push_back((Double_t)fData->fChData.fTrace[i]);
    

    
    // Found a signal.
    // Check that the channel is not the trigger channel.
    // THIS IS WRONG IN GENERAL, only works since trig ch is 0
    //    if(*fGlobalCh == fTrigCh) {
    //      fPSDValTrig = fPSDVal;
    //      fQDCTCTrig = (fQDCT+fRandom.Uniform())*fCalPar[*fGlobalCh][1]+fCalPar[*fGlobalCh][0];
    //      fCFDValTrig = fCFDTime;
    //      belowCFDTrig = belowCFD;
    //    }
    if(*fGlobalCh == fTrigCh && fSearchEntry == entry) {
      if(fData->fChData.fEnergy>0){
        nCh++;
        sumE += fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0];
        fPSDValTrig = fPSDVal;
      }
      fSearchEntry++;
    }
    else{
      // We found a detector in coincidence.
      //      if(!belowCFD) hDelta   ->Fill(fCFDTime - fCFDValTrig);
      //hDeltaCFD->Fill(CFDTimeDiff);
      //      hDeltaCFD->Fill(fCFDTime - fCFDValTrig);
      if(fData->fChData.fEnergy>0){
        hDelta->Fill(nextTDiff);
        nCh++;
        sumE += fData->fChData.fEnergy * fCalPar[*fGlobalCh][1] + fCalPar[*fGlobalCh][0];
      }
      
      fSearchEntry++;
    }
    //cout << "-- >>> " << *fGlobalCh << " " << fPSDVal << " " << fPSDValTrig << endl;
  }
  
  return GetEvents();
}


