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
    
    events.push_back(fillerEvent);
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
