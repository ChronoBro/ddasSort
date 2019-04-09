#include "RBDDarray.h"


RBDDarray::RBDDarray(){

}

RBDDarray::~RBDDarray(){

}


bool RBDDarray::fillArray(Event event){

  bool triggered = false;

  for(auto & det: detectors){
    bool detFilled = det.fillEvent(event);
    triggered = triggered || detFilled;

    if(detFilled){eventList.push_back(det.getFillerEvent());} //I calibrate when detector is filled so I need to grab event object from det
  }

  return triggered;

}

bool RBDDarray::compareE(const Event &event1, const Event &event2){
  return (event1.energy > event2.energy);
}

void RBDDarray::sortE(){

  // if(!isCalibrated){
  //   std::cout << "Cannot sort uncalibrated detectors" << std::endl;
  //   return;
  // }

  std::sort (eventList.begin(), eventList.end(), compareE);
}
