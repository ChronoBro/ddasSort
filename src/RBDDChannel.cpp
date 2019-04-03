
//
//  RBDDChannel.cxx
//
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include "RBDDChannel.h"

using namespace std;

//______________________________________________________________________________
// void RBDDChannel::Copy(TObject &obj) const
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



// //______________________________________________________________________________
// void RBDDChannel::Print(Option_t *) const
// {
//   // --
//   //
  
// //  cout << "==================== XIA RBPixie4eHeader (BEGIN) ====================" << endl;
// //  cout << "*  Block size       : " << fBlkSize << endl;         // Block size (16-bit words)
// //  cout << "*  Module number    : " << fModNum << endl;          // Module number
// //  cout << "*  Run type         : " << fRunFormat << endl;       // Format descriptor = Run Type
// //  cout << "*  Ch header length : " << fChanHeadLen << endl;     // Channel Header Length
// //  cout << "*  Coincidence Pat. : " << fCoincPat << endl;        // Coincidence pattern
// //  cout << "*  Coincidence Win. : " << fCoincWin << endl;        // Coincidence window
// //  cout << "*  Max. length      : " << fMaxCombEventLen << endl; // Maximum length of traces plus headers from all 4 channels (in blocks)
// //  cout << "*  Event length 0   : " << fEventLength0 << endl;    // Length of traces from channel 0 plus header (in blocks)
// //  cout << "*  Event length 1   : " << fEventLength1 << endl;    // Length of traces from channel 1 (in blocks)
// //  cout << "*  Event length 2   : " << fEventLength2 << endl;    // Length of traces from channel 2 (in blocks)
// //  cout << "*  Event length 3   : " << fEventLength3 << endl;    // Length of traces from channel 3 (in blocks)
// //  cout << "==================== XIA RBPixie4eHeader (END)   ====================" << endl;
  
// }

void RBDDChannel::setCalibration(std::vector<double> par){

  params = par;
  isCalibrated = true;

}


double RBDDChannel::calibrate(ULong64_t energyR){

  double value = 0;
  double castER = (double)energyR;
  double power = 0;
  for(auto p: params){
    value += p*pow(castER,power);
    power++;
  }

  return value;

}
