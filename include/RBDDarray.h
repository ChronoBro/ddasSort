#ifndef ____RBDDarray__
#define ____RBDDarray__


#include "RBDDdet.h"

class RBDDarray{

 public:
  bool fillArray(Event event);
  RBDDarray();
  ~RBDDarray();
  std::vector<Event> getEventList(){return eventList;}
  void clear(){eventList.clear();for(auto & det : detectors){det->clear();}}
  static bool compareE(const Event & event1, const Event & event2); //static makes there be only 1 copy of this function regardless of how many ddasDet's get implemented. Takes it up a level, out of the class, allowing it to be used in sorting
  static bool compareE_raw(const Event & event1, const Event & event2); //static makes there be only 1 copy of this function regardless of how many ddasDet's get implemented. Takes it up a level, out of the class, allowing it to be used in sorting
  void sortE();
  void sortEraw();

  Event maxE(){sortE();return eventList.front();}
  Event maxEraw(){sortEraw(); return eventList.front();}
  Event addBack();

  //if I pass address when adding detectors then I should be able to access them with pointer list liveDets in RBDDTriggeredEvent
  //above is not true
  //trying out std::move
  void addDet(RBDDdet &det){detectors.push_back(&det);channelList.push_back(det.getAssignedChannel());}
  void erase(int i){auto it = eventList.begin()+i;eventList.erase(it);}
  std::vector<RBDDdet*> getDetectors(){return detectors;}

  void fillArray();

 private:
  std::vector<RBDDdet*> detectors;
  std::vector<Event> eventList;
  std::vector<int> channelList;

};
#endif
