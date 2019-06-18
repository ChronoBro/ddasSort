#include "RBDDarray.h"


RBDDarray::RBDDarray(){

}

RBDDarray::~RBDDarray(){

}


bool RBDDarray::fillArray(Event event){

  bool triggered = false;

  //std::cout << detectors.size() << std::endl;

  //std::cout << std::endl;

  for(auto & det: detectors){
    bool detFilled = det.fillEvent(event);

    triggered = triggered || detFilled;


    // if(event.channel == det.getAssignedChannel()){
    //   std::cout << std::endl;
    //   std::cout << detFilled << std::endl;
    //   std::cout << event.channel << std::endl;
    //   std::cout << det.getAssignedChannel() << std::endl;
    //   std::cout << std::endl;
    // }

    if(detFilled){
      eventList.push_back(det.getFillerEvent());
      //std::cout << "made it here" << std::endl;

    } //I calibrate when detector is filled so I need to grab event object from det

  }

  //std::cout << std::endl;

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
