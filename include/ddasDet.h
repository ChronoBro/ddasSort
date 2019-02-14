#include <iostream>
#include <vector>
#include "/opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h" 
#include "math.h"


struct Event{
  double energy = -1;  //calibrated energy
  unsigned int signal = -1; //Raw signal
  int channel = -1;
  double time = 0;
  long long int entry = -1;
  std::vector<unsigned short> trace;
};


class ddasDet{
 public:
  ddasDet(){assignedChannel = -1;isCalibrated=false;}
  ddasDet(int givenChannel){assignedChannel = givenChannel;isCalibrated=false;}
  ~ddasDet(){events.clear();}
  bool fillEvent(ddaschannel* , DDASEvent* );//true if filled event, false if not
  std::vector<Event> getEvents(){return events;}
  int getChannelNumber(ddaschannel *dchan, int firstSlot) {return dchan->GetCrateID()*64+(dchan->GetSlotID()-firstSlot)*16+dchan->GetChannelID();}
  void reset(){events.clear();params.clear();isCalibrated=false;}
  void setCalibration(std::vector<double> par); //ordered list of polynomial parameters p0,p1,p2,...
  void clear(){events.clear();}
  Event getFillerEvent(){return fillerEvent;}
  void pop(){events.erase(events.begin());}
  void addEvent(Event event){events.push_back(event);}
  bool fillEvent(Event fillerEvent0);
  std::vector<unsigned short> getTrace(){;}

 private:
  int assignedChannel;
  std::vector<Event> events;
  bool isCalibrated;
  double calibrate(unsigned int energyR);
  std::vector<double> params;
  Event fillerEvent;

};
