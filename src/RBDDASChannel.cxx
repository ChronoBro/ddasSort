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
  
  setEventPoint(channel,event);
}

RBDDASChannel::setEventPointer(ddaschannel* channel, DDASEvent* event){

  curChannel = channel;
  curEvent = event;
  isPointerSet=true;
 
}

RBDDASChannel::unpack(){
  

  if(event->GetData().size() > 1){cerr << "RBDDASChannel was designed to work on un-built DDAS data i.e. there is only one ddaschannel in each DDASEvent... Aborting..." << endl; abort;}

  if(isPointerSet){
    curChannel = curEvent->GetData()[0];
    
    fChanNo = dchan->GetCrateID()*64+(dchan->GetSlotID()-2)*16+dchan->GetChannelID(); //right now this is specific to experiment, we should come up with more generalized way to handle channel numbers
    //should probably just choose ONE convention and use that from now on, but I'll worry about that later

    fMSPS = curChannel->GetModMSPS;
    fTimeStampLSB = curChannel->GetCoarseTime();
    trace = curChannel->GetTrace();
  }
  else{
    cerr << "Did not set DDASEvent pointer properly for RBDDASChannel to unpack... Aborting..." << endl; abort;
  }

}
