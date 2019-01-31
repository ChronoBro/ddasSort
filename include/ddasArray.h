//will make arrays of ddasDet here
#include "ddasDet.h"
#include <vector>
#include <iostream>
#include "/opt/build/nscldaq-ddas/main/ddasdumper/DDASEvent.h" 
#include "math.h"


class ddasArray{
 public:
  ddasArray();
  ddasArray(int nChannels);
  ~ddasArray();
  void setMap(std::vector<int> channels);
  void clear(){detectors.clear();masterEventList.clear();}
  void reset(){detectors.clear();isMapSet = false;channelList.clear();masterEventList.clear();}
  bool fillEvent(ddaschannel * , DDASEvent *);
  std::vector<int> getChannelList(){return channelList;}
  std::vector<Event> getMasterEventList(){return masterEventList;}
  void setCalibration(std::vector< std::vector<double> > paramList);

 private:
  std::vector<ddasDet> detectors;
  bool isMapSet;
  std::vector<int> channelList;
  std::vector<Event> masterEventList;
  bool isCalibrated;

};

