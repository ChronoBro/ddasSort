#ifndef ____RBDDdet__
#define ____RBDDdet__


#include <iostream>
#include <vector>
#include "/opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h" 
#include "math.h"
#include <algorithm>
#include "event.h"

class RBDDdet{
 public:
  RBDDdet(){assignedChannel = -1;isCalibrated=false;}
  ~RBDDdet(){events.clear();}
  RBDDdet(int givenChannel){assignedChannel=givenChannel;isCalibrated=false;}
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
  void erase(int i){auto it = events.begin()+i;events.erase(it);}
  int getAssignedChannel(){return assignedChannel;}

 private:
  int assignedChannel;
  std::vector<Event> events;
  bool isCalibrated;
  double calibrate(unsigned int energyR);
  std::vector<double> params;
  Event fillerEvent;
  //would be nice to have some kind of geometry attached to these


};
#endif
