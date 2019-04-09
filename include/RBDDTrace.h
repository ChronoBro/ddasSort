
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
  std::vector<Double_t> fTrace;        //
  Int_t            fBaseSamples;  //  Number of samples to use for baseline calculation.
  Double_t         fBaseSampleFraction;//
  Double_t         fBaseBegin;    //
  Double_t         fBaseEnd;      //
  Double_t         fB = 0;            //  Baseline value.
  Double_t         fA = 0;            //  Maximum value.
  Double_t         fQDC = 0;
  double           fMSPS = 1;
  std::vector<Double_t> fSum;          //!
  
  TH1D            *fTraceHisto;   //!
  void             analyze();
  bool isAnalyzed = false;
  bool isHistCreated = false;
  


public:
  RBDDTrace(){}
  ~RBDDTrace(){}
  //RBDDTrace(std::vector<Double_t> trace);
  RBDDTrace(std::vector<Double_t> trace, double sampleFraction = 20.);
  RBDDTrace(std::vector<unsigned short> trace, double sampleFraction = 20.);
  
  void Copy(TObject &obj) const;
  void Print(Option_t* option = "") const;
  
  void              ClearSums();
  
  Double_t          FindPeak();//{/*need to implement*/}
  
  std::vector<Double_t> GetTrace(){return fTrace;}
  TH1D*             GetTraceHisto();//{return fTraceHisto;}
  Double_t          GetMaximum(); //max and QDC are calculated at the same time 
  Double_t          GetQDC();//this and above should just return values or do calculation if not done
  Double_t          GetBaseline();
  Double_t          GetSum(Int_t sumWinID);
  Double_t          GetSum(Double_t winBegin, Double_t winEnd);
  Int_t             GetSumAndStore(Double_t winBegin, Double_t winEnd);
  Double_t          GetCFDTime(Double_t fraction = 0.5);
  bool              filter(); //returns true if trace should be filtered out

  void  SetBaseSampleFraction(double fraction){fBaseSampleFraction = fraction;}
  void  SetBaseSamples(Int_t nSamples){fBaseSamples = nSamples;}
  void  SetMSPS(double MSPS){fMSPS=MSPS;}
  

  //ClassDef(RBDDTrace,1); //comment this if I want to compile normally, otherwise I have to do some root trickery in the compilation
};

#endif /* defined(____RBDDTrace__) */
