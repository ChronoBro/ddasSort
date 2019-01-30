#include "ddasDet.h"


ddasDet::ddasDet(){
  assignedChannel = -1;
}

ddasDet::ddasDet(int givenChannel){
  assignedChannel = givenChannel;
}

ddasDet::~ddasDet(){
  events.clear();
}

void ddasDet::fillEvent(ddaschannel* foundChannel, DDASEvent* eventPointer){


  if(assignedChannel >= 0){

    Event fillerEvent;
    foundChannel = eventPointer->GetData()[0]; //taking only the first event, thus this method will only work on unbuilt events
    //will have to make another method for handling fully built events
    fillerEvent.channel = getChannelNumber(foundChannel,2);
    //std::cout << fillerEvent.channel << std::endl;
    fillerEvent.signal = foundChannel->GetEnergy();
    fillerEvent.time = foundChannel->GetCoarseTime();

    
    if(fillerEvent.channel == assignedChannel){
      //std::cout << "Filled Event" << std::endl;
      events.push_back(fillerEvent);
    }
    
  }
  else if (assignedChannel == -1){

    Event fillerEvent;
    foundChannel = eventPointer->GetData()[0]; //taking only the first event, thus this method will only work on unbuilt events
    //will have to make another method for handling fully built events
    fillerEvent.channel = getChannelNumber(foundChannel,2);
    fillerEvent.signal = foundChannel->GetEnergy();
    fillerEvent.time = foundChannel->GetCoarseTime();
    
    events.push_back(fillerEvent);

  }

}
