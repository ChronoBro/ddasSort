
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

/* struct Event{ */
/*   double energy = -1;  //calibrated energy */
/*   unsigned int signal = -1; //Raw signal */
/*   int channel = -1; */
/*   double time = 0; */
/*   long long int entry = -1; */
/*   std::vector<unsigned short> trace; */
/* }; */


class RBDDChannel : public TNamed
{
protected:
  Int_t     fChanNo=0;           //! Channel number
  Int_t     fMSPS=0;             //  ADC sampling frequency (MSPS)
  Int_t     fClockFreq=0;        //
  Int_t     fTimestampLSB=0;     //
  double    fTimestamp = 0;
  ULong64_t fSignal=0;
  double    fEnergy=0;
  std::vector<UShort_t> fTrace;

public:

  ~RBDDChannel(){;}
  RBDDChannel(){;}

  virtual  Int_t                 GetChanNo()    = 0;
  virtual  ULong64_t             GetTimestamp() = 0; // Value of time counter
  virtual  ULong64_t             GetSignal()    = 0;
  virtual  std::vector<UShort_t> GetTrace()     = 0; // Assumes 16-bit or less resolution
  virtual  double                GetEnergy()    = 0;
  virtual  void                  unpack()       = 0;

  /* RBDDChannel& operator=(RBDDChannel const &rhs) { */
  /*   if (this != &rhs) { */
  /*     delete m_ptr; // free resource; */
  /*     m_ptr = 0; */
  /*     m_ptr = rhs.m_ptr; */
  /*   } */
  /*   return *this; */
  /* }; */

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

  //ClassDef(RBDDChannel,1);
};

#endif /* defined(____RBDDChannel__) */
