#ifndef ____RBDDarray__
#define ____RBDDarray__


#include "RBDDdet.h"

class RBDDarray{

 public:
  bool fillArray(Event event);
  RBDDarray();
  ~RBDDarray();
  std::vector<Event> getEventList(){return eventList;}
  void clear(){eventList.clear();for(auto & det : detectors){det.clear();}}
  static bool compareE(const Event & event1, const Event & event2); //static makes there be only 1 copy of this function regardless of how many ddasDet's get implemented. Takes it up a level, out of the class, allowing it to be used in sorting
  void sortE();
  void addDet(RBDDdet det){detectors.push_back(det);}
  void erase(int i){auto it = eventList.begin()+i;eventList.erase(it);}

 private:
  std::vector<RBDDdet> detectors;
  std::vector<Event> eventList;


};
#endif
