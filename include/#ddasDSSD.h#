#include "ddasArray.h"


class ddasDSSD{
 public:
  ddasDSSD(){;}
  ddasDSSD(int nStripsX, int nStripsY){fronts = new ddasArray(nStripsX);backs = new ddasArray(nStripsY);}
  ddasDSSD(std::vector<int> frontChannels, std::vector<int> backChannels){;}
  ~ddasDSSD(){delete fronts;delete backs;}
  void fillEvents(Event fillerEvent);
  std::vector<Event> match();

 private:
  ddasArray * fronts;
  ddasArray * backs;


};
