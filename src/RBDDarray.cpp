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
    bool detFilled = det->fillEvent(event);

    triggered = triggered || detFilled;


    // if(event.channel == det.getAssignedChannel()){
    //   std::cout << std::endl;
    //   std::cout << detFilled << std::endl;
    //   std::cout << event.channel << std::endl;
    //   std::cout << det.getAssignedChannel() << std::endl;
    //   std::cout << std::endl;
    // }

    if(detFilled){
      eventList.push_back(det->getFillerEvent());
      //std::cout << "made it here" << std::endl;

    } //I calibrate when detector is filled so I need to grab event object from det

  }

  //std::cout << std::endl;

  return triggered;

}

void RBDDarray::fillArray(){

  for(auto&det : detectors){

    for(auto& event : det->getEvents()){

      eventList.push_back(event);

    }

  }

}


bool RBDDarray::compareE(const Event &event1, const Event &event2){
  return (event1.energy > event2.energy);
}

bool RBDDarray::compareE_raw(const Event &event1, const Event &event2){

  return (event1.signal > event2.signal);
}

void RBDDarray::sortE(){

  // if(!isCalibrated){
  //   std::cout << "Cannot sort uncalibrated detectors" << std::endl;
  //   return;
  // }

  std::sort (eventList.begin(), eventList.end(), compareE);
}

void RBDDarray::sortEraw(){

  // if(!isCalibrated){
  //   std::cout << "Cannot sort uncalibrated detectors" << std::endl;
  //   return;
  // }

  std::sort (eventList.begin(), eventList.end(), compareE_raw);
}


Event RBDDarray::addBack(){

  sortE();
  int stripTolerance = 3;
  Event addBackEvent = eventList.front();

  double addBackEnergy = 0.;
  for(auto & event : eventList){
    if( abs(event.channel-addBackEvent.channel)<stripTolerance   )
    addBackEnergy += event.energy;
  }

  addBackEvent.energy = addBackEnergy;

  return addBackEvent;
}

void RBDDarray::Print(){
  //std::cout << std::endl;
  std::cout << "=====================Event List=====================" << std::endl;
  int i=1;
  for(auto & det : detectors){
    det->Print();
  }


}
