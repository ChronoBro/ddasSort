
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
#include <TTree.h>
#include <TChain.h>
//#include <TList.h>
#include "RBDDChannel.h"
#include "RBDDarray.h"

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
  Int_t triggerChanNo;

  //creating list of pointers to point directly to detector objects, this should remove unnecessary loops over detector objects
  static const int maxChannels = 256;  //hardcoding limit is not a great idea, but will be the simplest to implement for testing and such
  RBDDdet *liveDets[maxChannels];
  void setNull();
                                  //note: instantiated RBDDChannel needs to be a pointer since this is an abstract class

  long long int lastEntry=0;        //lastEntry accessed in TTree/TChain

  TList       *fChannels;           // Time-ordered list of all channel objects in window.
//  TList    *fChannels;              // List of select correlated channel objects (Maybe we don't need this)
  //std::vector<RBDDChannel*> buffer; //A buffer, to try and perform the same operations I was doing before
  //issue with above is that I have to allocate memory for each channel, however, since I'm using a derived class of RBDDChannel I'm not sure
  //how to allocate the memory properly (my previous list was just the same address in memory over and over again)
  //above issue may have been because of bugs with timestamp, should try with array of RBDDChannel* again and see if it works
  //it does seem to work but its running really slow and has a memory leak, I could try to fix that or go back to original method which works fine
  std::vector<Event> buffer;
  std::vector<RBDDChannel*> bufferTest;
  void      setWindowWidth(Double_t windowWidth){fWindowWidth = windowWidth;}

  std::vector<RBDDarray*> arrayList; //storing pointers to array objects

  Event fillEvent(RBDDChannel*); 
  void loadBar(double progress);

 public:
  RBDDTriggeredEvent(){}
  ~RBDDTriggeredEvent(){}
  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel &ch,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel &ch,Double_t windowBegin,Double_t windowEnd);

  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel* ch,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDChannel* ch,Double_t windowBegin,Double_t windowEnd);


  RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,RBDDTriggeredEvent *ev,Double_t windowBegin,Double_t windowEnd);
  RBDDTriggeredEvent(const char *name,const char *title,TTree dataTree,RBDDChannel &ch,Double_t windowWidth);
  RBDDTriggeredEvent(const char *name,const char *title,TChain dataChain,RBDDChannel &ch,Double_t windowWidth);
  //I want to modify the variables directly, thus if not a pointer I need to pass by reference
  //I should overload all of these constructors to give the option to pass pointers, which are default passed by "reference"


  /* RBDDChannel * triggerChannel;      // store information on triggered channel   */
  RBDDChannel* fillerChannel;     //channel to check 

  void SetTriggerEvent(RBDDChannel *ch, Long64_t entry);
  void ResetTrigger();
  void triggerSearch(); //possible recursive function to get rid of for-loop over entries
                        //Will implement this once original conception is working (basic loop over entries)

//  void Copy(TObject &obj) const;
  void Print(Option_t* option = "") const;
  
  ULong64_t GetWindowFirstEvent()     {return fWindowFirstEvent;}
  ULong64_t GetWindowLastEvent()      {return fWindowLastEvent;}
  ULong64_t GetWindowFirstTimestamp() {return fWindowFirstTimestamp;}
  ULong64_t GetWindowLastTimestamp()  {return fWindowLastTimestamp;}
  Double_t  GetWindowFirstTime()      {return fWindowFirstTime;}
  Double_t  GetWindowLastTime()       {return fWindowLastTime;}
  std::vector<RBDDChannel*> GetBufferTest(){return bufferTest;}
  std::vector<Event> GetBuffer(){return buffer;}


  TList*    GetEvents(RBDDChannel *ch, TBranch *br);
  TList*    GetEvents()               {return fChannels;}


  bool IsTriggerCh(Int_t ch){return triggerChanNo == ch;}
  void setTriggerCh(int ch){triggerChanNo=ch;}
  bool isTriggered(){return fillerChannel->GetChanNo() == triggerChanNo;}
  bool isTriggered( bool (*f)(int) ){ return f(fillerChannel->GetChanNo());} //passing in function for trigger condition
  long long int FillBuffer(TChain &dataChain, long long int iEntry);

  //again should overload functions to accept pointers as well
  long long int GetCoinEvents(TChain &dataChain);
  long long int GetCorrEvents(TChain &dataChain);

  double triggerSignal;
  double triggerTime;

  //should overload this with pointer support
  bool dumpBuffer(RBDDdet &det);
  bool dumpBuffer(RBDDarray &array);
  bool dumpBuffer();

  bool activateDetector(RBDDdet &det);
  bool activateArray(RBDDarray &array);

  void clear();
  

  //at some point I want to pass a bool function into the trigger condition

  //ClassDef(RBDDTriggeredEvent,1);
};

#endif /* defined(____RBDDTriggeredEvent__) */
