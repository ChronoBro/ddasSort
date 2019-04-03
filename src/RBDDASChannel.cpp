#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include "RBDDASChannel.h"

using namespace std;


RBDDASChannel::RBDDASChannel(){

}

RBDDASChannel::~RBDDASChannel(){

}

RBDDASChannel::RBDDASChannel(ddaschannel* channel, DDASEvent* event){
  
  setEventPointer(channel,event);
}

void RBDDASChannel::setEventPointer(ddaschannel* channel, DDASEvent* event){

  curChannel = channel;
  curEvent = event;
  isPointerSet=true;
 
}

void RBDDASChannel::unpack(){
  
  if(isPointerSet){

    if(curEvent->GetData().size() > 1){cerr << "RBDDASChannel was designed to work on un-built DDAS data i.e. there is only one ddaschannel in each DDASEvent... Aborting..." << endl; abort();}


    curChannel = curEvent->GetData()[0];
    
    fChanNo = curChannel->GetCrateID()*64+(curChannel->GetSlotID()-2)*16+curChannel->GetChannelID(); //right now this is specific to experiment, we should come up with more generalized way to handle channel numbers
    //should probably just choose ONE convention and use that from now on, but I'll worry about that later
    if(fChanNo == assignedChannel || assignedChannel == -1){
      fSignal = curChannel->GetEnergy();
      fMSPS = curChannel->GetModMSPS();
      fTimestampLSB = curChannel->GetCoarseTime();
      fTrace = curChannel->GetTrace();

      if(isCalibrated){fEnergy = calibrate(fSignal);}

    }

  }
  else{
    cerr << "Did not set DDASEvent pointer properly for RBDDASChannel to unpack... Aborting..." << endl; abort();
  }

}
