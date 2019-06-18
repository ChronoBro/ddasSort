#ifndef ____DSSDEvent__
#define ____DSSDEvent__

#include "event.h"

struct DSSDEvent{
  /* double energy = -1;  //calibrated energy */
  /* unsigned int signal = 0; //Raw signal */
  /* int channel = -1; */
  /* double time = 0; */
  /* long long int entry = -1; */
  /* std::vector<unsigned short> trace; */

  Event frontEvent;
  Event backEvent;

  /* Event& operator=(const Event& a) */
  /* { */
  /*   energy=a.energy; */
  /*   signal=a.signal; */
  /*   channel=a.channel; */
  /*   time = a.time; */
  /*   entry = a.entry; */
  /*   trace = a.trace; */
  /*   return *this; */
  /* } */


};

#endif

