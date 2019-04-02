
//
//  RBDDTrace.cxx
//
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include "RBDDTrace.h"

using namespace std;

//______________________________________________________________________________
void RBDDTrace::Copy(TObject &obj) const
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
void RBDDTrace::Print(Option_t *) const
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
void RBDDTrace::ClearSums()
{
  
}

//______________________________________________________________________________
Double_t RBDDTrace::FindPeak()
{
  /*Need to implement*/
  
}

//______________________________________________________________________________
Double_t RBDDTrace::GetMaximum()
{
  //
  
  return fA;
}

//______________________________________________________________________________
Double_t RBDDTrace::GetBaseline()
{
  //
  
  // Find the baseline
  fB = 0;
  for(Int_t iBase=fBaseBegin; iBase<(fBaseBegin+fBaseSamples); iBase++) fB += curTrace[iBase];
  fB = fB/(fBaseSamples);
  
  return fB;
}

//______________________________________________________________________________
Double_t RBDDTrace::GetSum(Int_t sumWinID)
{
  //
  
  Double_t sum = 0;
  
  // Calculate QDCs
  fQDC0 = 0;
  fQDC1 = 0;
  fQDCT = 0;
  for(Int_t iQ0=0; iQ0<fL0; iQ0++){
    Int_t index = fS0 + iQ0;
    if(index>0 && index<(Int_t)curTrace.size()) fQDC0 += curTrace[index];
    else cout << "index error QDC0 " << index <<  endl;
  }
  
  
  return sum;
}

//______________________________________________________________________________
Double_t RBDDTrace::GetSum(Double_t winBegin, Double_t winEnd)
{
  //
  
}

//______________________________________________________________________________
Int_t    RBDDTrace::GetSumAndStore(Double_t winBegin, Double_t winEnd)
{
  //
  
  
}

//______________________________________________________________________________
Double_t RBDDTrace::GetCFDTime(Double_t fraction = 0.5)
{
  
}



