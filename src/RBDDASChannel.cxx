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
  
  unpack(channel,event);
  
}

RBDDASChannel::unpack(ddaschannel *channel,DDASEVent* event){

  if(event->GetData().size() > 1){cout << "This class was designed to work on un-built DDAS data i.e. there is only one ddaschannel in each DDASEvent" << endl; abort;}

  ddaschannel* dchan = event->GetData()[0];

  fChanNo = dchan->GetCrateID()*64+(dchan->GetSlotID()-2)*16+dchan->GetChannelID(); //right now this is specific to experiment, we should come up with more generalized way to handle channel numbers

  fMSPS = dchan->GetModMSPS;
  fTimeStampLSB = dchan->GetCoarseTime();
  trace = dchan->GetTrace();
}
