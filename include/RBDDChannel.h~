
//
//  RBDDChannel.h
//

#ifndef ____RBDDChannel__
#define ____RBDDChannel__

//#include <cmath>
//#include <stdio.h>
//#include <vector>
//
//#include <TBits.h>
//#include <TCanvas.h>
//#include <TFile.h>
//#include <TGraph.h>
//#include <TH1I.h>
#include <TObject.h>
#include <TNamed.h>
//#include <TTree.h>
//#include <TList.h>


class RBDDChannel : public TNamed
{
protected:
  Int_t     fChanNo;           //! Channel number
  Int_t     fMSPS;             //  ADC sampling frequency (MSPS)
  Int_t     fClockFreq;        //
  Int_t     fTimestampLSB;     //
  ULong64_t fSignal;
  std:vector<UShort_t> fTrace;

public:
  RBDDChannel();
  ~RBDDChannel();
  
  virtual  Int_t                 GetChanNo()    = 0;
  virtual  ULong64_t             GetTimestamp() = 0; // Value of time counter
  virtual  ULong64_t             GetSignal()    = 0;
  virtual  std::vector<UShort_t> GetTrace()     = 0; // Assumes 16-bit or less resolution
  
  /*

  // What's the purpose of having the above virtual? I think it depends on where the unpacking is done, but is we're
  // using a derived class for unpacking then it makes sense to have the below defined here than overwriting it in
  // the derived class

  Int_t                 GetChanNo(){return fChanNo;}
  ULong64_t             GetTimestamp(){return fTimeStampLSB;} // Value of time counter
  ULong64_t             GetSignal(){return fSignal;}
  std::vector<UShort_t> GetTrace(){return fTrace;} // Assumes 16-bit or less resolution

  */

  Int_t GetClockFreq()   {return fClockFreq;}
  Int_t GetTimestampLSB(){return fTimestampLSB;}
  
  ClassDef(RBDDChannel,1);
};

#endif /* defined(____RBDDChannel__) */
