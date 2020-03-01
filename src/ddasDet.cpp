#include "ddasDet.h"

bool ddasDet::fillEvent(ddaschannel* foundChannel, DDASEvent* eventPointer){


  if(assignedChannel >= 0){

    //Event fillerEvent;
    foundChannel = eventPointer->GetData()[0]; //taking only the first event, thus this method will only work on unbuilt events
    //will have to make another method for handling fully built events
    
    fillerEvent.channel = getChannelNumber(foundChannel,2);
    //std::cout << fillerEvent.channel << std::endl;
    fillerEvent.signal = foundChannel->GetEnergy();
    fillerEvent.time = foundChannel->GetCoarseTime();
    //std::vector<double> fill(foundChannel->GetTrace().begin(),foundChannel->GetTrace().end(), fillerEvent.trace.get_allocator());
    fillerEvent.trace = foundChannel->GetTrace();

    if(isCalibrated){
      fillerEvent.energy = calibrate(fillerEvent.signal);
    }

    
    if(fillerEvent.channel == assignedChannel){
      //std::cout << "Filled Event" << std::endl;
      events.push_back(fillerEvent);
      return true;
    }
    
  }
  else if (assignedChannel == -1){

    //Event fillerEvent;
    foundChannel = eventPointer->GetData()[0]; //taking only the first event, thus this method will only work on unbuilt events
    //will have to make another method for handling fully built events
    fillerEvent.channel = getChannelNumber(foundChannel,2);
    fillerEvent.signal = foundChannel->GetEnergy();
    fillerEvent.time = foundChannel->GetCoarseTime();

    //std::vector<double> fill(foundChannel->GetTrace().begin(),foundChannel->GetTrace().end(),fillerEvent.trace.get_allocator());
    fillerEvent.trace = foundChannel->GetTrace();

    //fillerEvent.trace = foundChannel->GetTrace();
    
    events.push_back(fillerEvent);
    return true;
  }

  return false;

}

bool ddasDet::fillEvent(Event fillerEvent0){


  if(assignedChannel >= 0){

    
    fillerEvent.channel = fillerEvent0.channel;
    //std::cout << fillerEvent.channel << std::endl;
    fillerEvent.signal = fillerEvent0.signal;
    fillerEvent.time = fillerEvent0.time;
    fillerEvent.trace = fillerEvent0.trace;
    
    fillerEvent = fillerEvent0;
    //fillerEvent.trace = fillerEvent0.trace;

    if(isCalibrated){
      fillerEvent.energy = calibrate(fillerEvent0.signal);
    }

    if(fillerEvent0.channel == assignedChannel){
      //std::cout << "Filled Event" << std::endl;
      events.push_back(fillerEvent);
      return true;
    }
    
  }
  else if (assignedChannel == -1){

    // fillerEvent.channel = fillerEvent0.channel;
    // fillerEvent.signal = fillerEvent0.signal;
    // fillerEvent.time = fillerEvent0.time;
    // fillerEvent.energy = fillerEvent0.energy;

    fillerEvent = fillerEvent0;
    events.push_back(fillerEvent0);
    return true;
  }

  return false;

}


void ddasDet::setCalibration(std::vector<double> par){

  params = par;
  isCalibrated = true;

}

double ddasDet::calibrate(unsigned int energyR){

  double value = 0;
  double castER = (double)energyR;
  double power = 0;
  for(auto p: params){
    value += p*pow(castER,power);
    power++;
  }

  return value;

}

bool ddasDet::compareE(const Event &event1, const Event &event2){
  return (event1.energy > event2.energy);
}

void ddasDet::sortE(){

  // if(!isCalibrated){
  //   std::cout << "Cannot sort uncalibrated detectors" << std::endl;
  //   return;
  // }

  std::sort (events.begin(), events.end(), compareE);
}
