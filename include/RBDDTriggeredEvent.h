
//
//  RBDDTriggeredEvent.h
//

#ifndef ____RBDDTriggeredEvent__
#define ____RBDDTriggeredEvent__

//#include <cmath>
//#include <stdio.h>
//#include <vector>
//
//#include <TBits.h>
//#include <TCanvas.h>
//#include <TFile.h>
//#include <TGraph.h>
//#include <TH1I.h>
#include <TNamed.h>
#include <TObject.h>
#include <vector>
//#include <TTree.h>
//#include <TList.h>

#include "RBDDChannel.h"

class RBDDTriggeredEvent : public TNamed
{
 private:
  Double_t  fWindowWidth;           // Coincidence/correlation window width (ns)
  Double_t  fWindowBegin;           // Relative start time of window (ns).
  Double_t  fWindowEnd;             // Relative end time of window (ns).
  ULong64_t fWindowFirstEvent;      // Event number of window start
  ULong64_t fWindowLastEvent;       // Event number of window end
  ULong64_t fWindowFirstTimestamp;  // Timestamp of window start
  ULong64_t fWindowLastTimestamp;   // Timestamp of window end
  Double_t  fWindowFirstTime;       // Event time of window start (ns)
  Double_t  fWindowLastTime;        // Event time of window end (ns)
  Int_t triggerChannel;

  RBDDChannel *fTriggerChannel;      // The 
  TList       *fChannels;           // Time-ordered list of all channel objects in window.
//  TList    *fChannels;              // List of select correlated channel objects (Maybe we don't need this)
  std::vector<RBDDChannel> triggeredChannels; //A vector of channels that are in coincidence with the trigger
  
  
 public:
  RBDDTriggeredEvent(){}
  ~RBDDTriggeredEvent(){}
  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel *ch,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel *ch,Double_t windowBegin,Double_t windowEnd);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,Double_t windowBegin,Double_t windowEnd);
  
  void SetTriggerEvent(RBDDChannel *ch, Long64_t entry);
  void ResetTrigger();
  void triggerSearch(); //possible recursive function to get rid of for-loop over entries


//  void Copy(TObject &obj) const;
  void Print(Option_t* option = "") const;
  
  ULong64_t GetWindowFirstEvent()     {return fWindowFirstEvent;}
  ULong64_t GetWindowLastEvent()      {return fWindowLastEvent;}
  ULong64_t GetWindowFirstTimestamp() {return fWindowFirstTimestamp;}
  ULong64_t GetWindowLastTimestamp()  {return fWindowLastTimestamp;}
  Double_t  GetWindowFirstTime()      {return fWindowFirstTime;}
  Double_t  GetWindowLastTime()       {return fWindowLastTime;}
  
  TList*    GetEvents(RBDDChannel *ch, TBranch *br);
  TList*    GetEvents()               {return fChannels;}


  Bool_t IsTriggerCh(Int_t ch){if(fTriggerCh==ch) return kTRUE; else return kFALSE;}
  
  ClassDef(RBDDTriggeredEvent,1);
};

#endif /* defined(____RBDDTriggeredEvent__) */
