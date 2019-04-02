
//
//  RBDDTrace.h
//

#ifndef ____RBDDTrace__
#define ____RBDDTrace__

//#include <cmath>
//#include <stdio.h>
#include <vector>
//
//#include <TBits.h>
//#include <TCanvas.h>
//#include <TFile.h>
//#include <TGraph.h>
#include <TH1D.h>
#include <TNamed.h>
#include <TObject.h>
//#include <TTree.h>
//#include <TList.h>


class RBDDTrace : public TNamed
{
protected:
  vector<Double_t> fTrace;        //
  Int_t            fBaseSamples;  //  Number of samples to use for baseline calculation.
  Double_t         fBaseBegin;    //
  Double_t         fBaseEnd;      //
  Double_t         fB;            //  Baseline value.
  Double_t         fA;            //  Maximum value.
  vector<Double_t> fSum;          //!
  
  TH1D            *fTraceHisto;   //!
  
public:
  RBDDTrace(){}
  ~RBDDTrace(){}
  
  void Copy(TObject &obj) const;
  void Print(Option_t* option = "") const;
  
  void              ClearSums();
  
  Double_t          FindPeak(){/*Need to implement*/}
  
  vector<Double_t> *GetTrace(){return fTrace;}
  TH1D             *GetTraceHisto(){return fTraceHisto;}
  Double_t          GetMaximum();
  Double_t          GetBaseline();
  Double_t          GetSum(Int_t sumWinID);
  Double_t          GetSum(Double_t winBegin, Double_t winEnd);
  Int_t             GetSumAndStore(Double_t winBegin, Double_t winEnd);
  Double_t          GetCFDTime(Double_t fraction = 0.5);
  
  void  SetBaseSamples(Int_t nSamples){fBaseSamples = nSamples;}

  ClassDef(RBDDTrace,1);
};

#endif /* defined(____RBDDTrace__) */
