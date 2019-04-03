
//
//  RBDDASChannel.h
//

#ifndef ____RBDDASChannel__
#define ____RBDDASChannel__

#include <TObject.h>
#include <TNamed.h>
#include <RBDDChannel.h>
#include <vector>
#include </opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h>

class RBDDASChannel : public RBDDChannel
{

 protected:
//int getChannelNumber(ddaschannel *dchan, int firstSlot) {return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();}
  ddaschannel* curChannel;
  DDASEvent* curEvent;
  bool isPointerSet = false;

 public:
  RBDDASChannel();
  RBDDASChannel(ddaschannel*, DDASEvent*);
  RBDDASChannel(int){;}
  ~RBDDASChannel();
  void unpack();
  void setEventPointer(ddaschannel*, DDASEvent*);


  Int_t                 GetChanNo(){return fChanNo;}
  ULong64_t             GetTimestamp(){return fTimestampLSB;} // Value of time counter
  ULong64_t             GetSignal(){return fSignal;}
  std::vector<UShort_t> GetTrace(){return fTrace;} // Assumes 16-bit or less resolution

  //ClassDef(RBDDASChannel,1);
};

#endif /* defined(____RBDDChannel__) */