
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


RBDDTrace::RBDDTrace(vector<unsigned short> trace, double sampleFraction){
  fBaseSampleFraction = sampleFraction;
  for(unsigned int iBin=0;iBin<trace.size();iBin++){
    fTrace.push_back((double)trace[iBin]);
  }
  analyze();
}

// RBDDTrace::RBDDTrace(vector<Double_t> trace){
//   fBaseSampleFraction = 20.;
//   fTrace = trace;
// }


RBDDTrace::RBDDTrace(vector<Double_t> trace, double sampleFraction){
  fBaseSampleFraction = sampleFraction;
  fTrace = trace;
  analyze();
}


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
  if(!isAnalyzed){
    analyze();
  }

  //
  return fA;
}

Double_t RBDDTrace::GetQDC()
{
  if(!isAnalyzed){
    analyze();
  }

  //
  return fQDC;
}


//______________________________________________________________________________
Double_t RBDDTrace::GetBaseline()
{
  //

  if(fB==0){
    double base = 0;
    int sampleSize = int((double)fTrace.size()/fBaseSampleFraction);
    for(int iBin=0; iBin<sampleSize; iBin++){ base += fTrace[iBin];}
    base = base/((double)sampleSize);
    
  // Find the baseline
  // fB = 0;
  // for(Int_t iBase=fBaseBegin; iBase<(fBaseBegin+fBaseSamples); iBase++) fB += curTrace[iBase];
  // fB = fB/(fBaseSamples);
  
    fB = base;
  }

  return fB;
}

//______________________________________________________________________________
Double_t RBDDTrace::GetSum(Int_t sumWinID)
{
  //
  
  Double_t sum = 0;
  
  // Calculate QDCs
  // fQDC0 = 0;
  // fQDC1 = 0;
  // fQDCT = 0;
  // for(Int_t iQ0=0; iQ0<fL0; iQ0++){
  //   Int_t index = fS0 + iQ0;
  //   if(index>0 && index<(Int_t)curTrace.size()) fQDC0 += curTrace[index];
  //   else cout << "index error QDC0 " << index <<  endl;
  // }
  
  
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
Double_t RBDDTrace::GetCFDTime(Double_t fraction)
{
  
}



TH1D* RBDDTrace::GetTraceHisto(){
  if(!isHistCreated){

    double timeUnit = 1./fMSPS; //timeunit will be in us if in MEGA samples/second

    if(fTrace.size()>0){
      fTraceHisto = new TH1D("trace","trace",fTrace.size(),0,(double)fTrace.size()*timeUnit);
      fTraceHisto->GetXaxis()->SetTitle("time (us)");
      for(unsigned int iBin=0;iBin<fTrace.size();iBin++){
	fTraceHisto->SetBinContent(iBin,fTrace[iBin]-fB); //I like to remove baseline for plotting
      }
      isHistCreated = true;
      return fTraceHisto;
    }
    else{
      cerr << "no trace to output stored in RBDDTrace Object" << endl;
      return 0;
    }

  }
  else{
    return fTraceHisto;
  }

 }

bool RBDDTrace::filter(){

  double triggerCheck = fTrace[100] - fB; // to get rid of "pile-up" events, which throws off the baseline
  
  //should erase the events that don't pass my filter if I'm going to use all front events
  //erasing events didn't have the desired output
  //idk what the above is about but this should work now
  if(fQDC<0 || fQDC >1E5 || fA < 0 || triggerCheck < -25){return false;}
  
  return true;

}

void RBDDTrace::analyze(){

  double derivative = 200;
  double derivativeOld = 100;
  double base = GetBaseline();
  double qdc0 = 0;
  double max = 0;

  for(unsigned int iBin=0; iBin<fTrace.size(); iBin++) {
    double charge = (double)fTrace[iBin] - base;
    qdc0 += charge;// - base;//ch->trace[iQDC] - base;
    if(iBin>0 && iBin<(fTrace.size()-1)){
      derivative = ( (double)fTrace[iBin+1]-(double)fTrace[iBin-1] )/2.; //2 point stencil for approximating first derivative
    }
    
    if(abs(derivative) < abs(derivativeOld) ){
      if(max<charge && qdc0>0) max = charge;// - base; //checking if pulse is inverted, if it is then take local minimum
      else if(max>charge && qdc0<0) max=charge;
    }
    derivativeOld = derivative;
  }

  fA = max;
  fQDC = qdc0;

  isAnalyzed = true;

}
