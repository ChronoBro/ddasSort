#include "RBDDdet.h"

bool RBDDdet::fillEvent(ddaschannel* foundChannel, DDASEvent* eventPointer){
  //deprecated

  if(assignedChannel >= 0){

    //Event fillerEvent;
    foundChannel = eventPointer->GetData()[0]; //taking only the first event, thus this method will only work on unbuilt events
    //will have to make another method for handling fully built events
    double channelFound = getChannelNumber(foundChannel,2);
    if(channelFound == assignedChannel){
      fillerEvent.channel = channelFound;;
      //std::cout << fillerEvent.channel << std::endl;
      fillerEvent.signal = foundChannel->GetEnergy();
      fillerEvent.time = foundChannel->GetCoarseTime();
      //std::vector<double> fill(foundChannel->GetTrace().begin(),foundChannel->GetTrace().end(), fillerEvent.trace.get_allocator());
      fillerEvent.trace = foundChannel->GetTrace();
      
      if(isCalibrated){
	fillerEvent.energy = calibrate(fillerEvent.signal);
      }
      
      

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

bool RBDDdet::fillEvent(Event fillerEvent0){

  if(events.size() > 2){
    std::cout << "size of events in channel #" << assignedChannel << ": something is likely wrong e.g. detector not cleared" << std::endl;
  }

  if(assignedChannel >= 0){

    

    if(fillerEvent0.channel == assignedChannel){

      fillerEvent.channel = fillerEvent0.channel;
      //std::cout << fillerEvent.channel << std::endl;
      fillerEvent.signal = fillerEvent0.signal;
      fillerEvent.time = fillerEvent0.time;
      fillerEvent.trace = fillerEvent0.trace;
      
      //fillerEvent = fillerEvent0;
      //fillerEvent.trace = fillerEvent0.trace;
      
      if(isCalibrated){
	fillerEvent.energy = calibrate(fillerEvent0.signal);
      }
      

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


void RBDDdet::setCalibration(std::vector<double> par){

  params = par;
  isCalibrated = true;

}

double RBDDdet::calibrate(unsigned int energyR){

  double value = 0;
  double castER = (double)energyR;
  double power = 0;
  for(auto p: params){
    value += p*pow(castER,power);
    power++;
  }

  return value;

}


void RBDDdet::Print(){

  //std::cout << std::endl;
    //cout << "=====================Event List=====================" << std::endl;
  int i=1;
  for(auto & event : events){    
    std::cout << "Event     : " << i << std::endl;
    std::cout << "channel   : " << event.channel << std::endl;
    std::cout << "signal    : " << event.signal << std::endl;
    std::cout << "energy    : " << event.energy << std::endl;
    std::cout << "time      : " << event.time << std::endl;
    std::cout << std::endl;
    i++;
  }


}
