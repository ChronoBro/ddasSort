#include "ddasArray.h"


class ddasDSSD{
 public:
  ddasDSSD(){;}
  // ddasDSSD(int nStripsX, int nStripsY){fronts = new ddasArray(nStripsX);backs = new ddasArray(nStripsY);}
  //ddasDSSD(std::vector<int> frontChannels, std::vector<int> backChannels){;}
  ~ddasDSSD(){fronts.clear();backs.clear();}
  void fillEvents(Event fillerEvent);
  std::vector<Event> match();

 private:
  std::vector<ddasDet> fronts;
  std::vector<ddasDet> backs;
  //ddasDet frontEvents(-1.);
  //ddasDet backEvents(-1.);


};
