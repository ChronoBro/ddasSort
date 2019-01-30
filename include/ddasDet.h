#include <iostream>
#include <vector>
#include "/opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h" 



struct Event{
  double energy = 0;  //calibrated energy
  unsigned int signal = -1; //Raw signal
  int channel = 0;
  double time = 0;
};


class ddasDet{
 public:
  ddasDet();
  ddasDet(int givenChannel);
  ~ddasDet();
  void fillEvent(ddaschannel* , DDASEvent* );
  std::vector<Event> getEvents(){return events;}
  int getChannelNumber(ddaschannel *dchan, int firstSlot) {return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();}

 private:
  int assignedChannel;
  std::vector<Event> events;
};
