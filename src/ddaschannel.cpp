/*****************************************/
/* This channel initializes a new        */
/* DDAS channel event.                   */
/*****************************************/

#include "ddaschannel.h"
#include "DDASHit.h"
#include "DDASHitUnpacker.h"
#include "DDASBitMasks.h"
#include <iostream>
#include <limits>
#include <tuple>

using namespace std;

ClassImp(ddaschannel);

// Avoid the static initialization order fiasco with construct
// on first use idiom ( see https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use)
DAQ::DDAS::DDASHit& getStaticHit() {
  static DAQ::DDAS::DDASHit* pHit = new DAQ::DDAS::DDASHit;
  return *pHit;
}


ddaschannel::ddaschannel() : TObject() {
  time = 0;
  coarsetime = 0;
  cfd = 0;

  energy = 0;
  timehigh = 0;
  timelow = 0;
  timecfd = 0;

  channelnum = 0;
  finishcode = 0;
  channellength = 0;
  channelheaderlength = 0;
  overflowcode = 0;
  chanid = 0;
  slotid = 0;
  crateid = 0;
  id = 0;

  cfdtrigsourcebit = 0;
  cfdfailbit = 0;

  tracelength = 0;

  ModMSPS = 0;
  
  energySums.reserve(4);
  qdcSums.reserve(8);
  trace.reserve(200);

  externalTimestamp = 0;

  m_adcResolution = 0;
  m_hdwrRevision = 0;
  m_adcOverUnderflow = false;
}


ddaschannel& ddaschannel::operator=(DAQ::DDAS::DDASHit& hit)
{
  time                = hit.GetTime();
  coarsetime          = hit.GetCoarseTime();
  cfd                 = 0; // not used
  energy              = hit.GetEnergy();
  timehigh            = hit.GetTimeHigh();
  timelow             = hit.GetTimeLow();
  timecfd             = hit.GetTimeCFD();
  channelnum          = hit.GetChannelID();
  finishcode          = hit.GetFinishCode();
  channellength       = hit.GetChannelLength();
  channelheaderlength = hit.GetChannelLengthHeader();
  overflowcode        = hit.GetOverflowCode();
  chanid              = hit.GetChannelID();
  slotid              = hit.GetSlotID();
  crateid             = hit.GetCrateID();
  id                  = 0;
  cfdtrigsourcebit    = hit.GetCFDTrigSource();
  cfdfailbit          = hit.GetCFDFailBit();
  tracelength         = hit.GetTraceLength();
  ModMSPS             = hit.GetModMSPS();
  energySums          = hit.GetEnergySums();
  qdcSums             = hit.GetQDCSums();
  trace               = hit.GetTrace();
  externalTimestamp   = hit.GetExternalTimestamp();
  m_adcResolution     = hit.GetADCResolution();
  m_hdwrRevision      = hit.GetHardwareRevision();
  m_adcOverUnderflow  = hit.GetADCOverflowUnderflow();

  return *this;
}



void ddaschannel::Reset() {
  time = 0;
  coarsetime = 0;
  cfd = 0;

  energy = 0;
  timehigh = 0;
  timelow = 0;
  timecfd = 0;

  channelnum = 0;
  finishcode = 0;
  channellength = 0;
  channelheaderlength = 0;
  overflowcode = 0;
  chanid = 0;
  slotid = 0;
  crateid = 0;
  id = 0;

  cfdtrigsourcebit = 0;
  cfdfailbit = 0;

  tracelength = 0;

  ModMSPS = 0;

  energySums.clear();
  qdcSums.clear();
  trace.clear();
  
  externalTimestamp = 0;

  m_hdwrRevision = 0;
  m_adcResolution = 0;
  m_adcOverUnderflow = false;
}

ddaschannel::~ddaschannel() {


}

void ddaschannel::UnpackChannelData(const uint32_t *data)
{
  using namespace DAQ::DDAS;

  DDASHitUnpacker unpacker;

  DDASHit&  hit = getStaticHit();
  hit.Reset();

  size_t nShorts = *data;
  unpacker.unpack(data, 
                  data + nShorts*sizeof(uint32_t)/sizeof(uint16_t),
                  hit); 

  // copy the state
  *this = hit;
}


