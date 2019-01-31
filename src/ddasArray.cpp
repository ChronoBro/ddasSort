#include "ddasArray.h"


ddasArray::ddasArray(){
  isMapSet = false;
  isCalibrated = false;
}

ddasArray::~ddasArray(){
  masterEventList.clear();
  detectors.clear();
  isMapSet = false;
  isCalibrated = false;
}

ddasArray::ddasArray(int nChannels){

  isMapSet = true;
  isCalibrated = false;

  for(int i=0;i<nChannels;i++){
    ddasDet fillerDet;
    detectors.push_back(fillerDet);
  }

}

void ddasArray::setMap(std::vector<int> channels){

  channelList = channels;
  detectors.clear();
  for(auto &channel : channelList){
    ddasDet fillerDet(channel);
    detectors.push_back(fillerDet);
  }

  isMapSet = true;

}

bool ddasArray::fillEvent(ddaschannel * foundChannel, DDASEvent * eventPointer){

  bool foundEvent = false;
  for(auto & det: detectors){

    bool foundDetEvent = det.fillEvent(foundChannel,eventPointer);
    if(foundDetEvent){
      masterEventList.push_back(det.getFillerEvent());
    }

    foundEvent = foundDetEvent || foundEvent; //return true if at least one detector found an event

  }


  return foundEvent;
}


void ddasArray::setCalibration(std::vector< std::vector<double> > paramList){

  if(paramList.size() != detectors.size() ){
    std::cerr << std::endl << "Could not calibrate detector array" << std::endl;
    return;
  }

  for(unsigned int i=0;i< detectors.size();i++){
    detectors[i].setCalibration(paramList[i]);
  }

}
