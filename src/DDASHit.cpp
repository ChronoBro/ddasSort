/*****************************************/
/* This channel initializes a new        */
/* DDAS channel event.                   */
/*****************************************/

#include "DDASHit.h"
#include "DDASBitMasks.h"

#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;

namespace DAQ {
  namespace DDAS {

    DDASHit::DDASHit() :
      time(0),
      coarsetime(0),
      energy(0),
      timehigh(0),
      timelow(0),
      timecfd(0),
      finishcode(0),
      channellength(0),
      channelheaderlength(0),
      overflowcode(0),
      chanid(0),
      slotid(0),
      crateid(0),
      cfdtrigsourcebit(0),
      cfdfailbit(0),
      tracelength(0),
      ModMSPS(0),
      energySums(),
      qdcSums(),
      trace(),
      externalTimestamp(0),
      m_adcResolution(0),
      m_hdwrRevision(0),
      m_adcOverflowUnderflow(false)
    {}

    void DDASHit::Reset() {
      time = 0;
      coarsetime = 0;
      energy = 0;
      timehigh = 0;
      timelow = 0;
      timecfd = 0;
      finishcode = 0;
      channellength = 0;
      channelheaderlength = 0;
      overflowcode = 0;
      chanid = 0;
      slotid = 0;
      crateid = 0;
      cfdtrigsourcebit = 0;
      cfdfailbit = 0;
      tracelength = 0;
      ModMSPS = 0;

      energySums.clear();
      qdcSums.clear();
      trace.clear();
      
      externalTimestamp = 0;
      m_adcResolution = 0;
      m_hdwrRevision = 0;
      m_adcOverflowUnderflow = false;
    }

    DDASHit::~DDASHit() {
    }

    void DDASHit::setChannel(uint32_t channel) { chanid = channel; }
    void DDASHit::setSlot(uint32_t slot) { slotid = slot; }
    void DDASHit::setCrate(uint32_t crate) {crateid = crate; }
    void DDASHit::setChannelHeaderLength(uint32_t channelHeaderLength) { channelheaderlength = channelHeaderLength; }
    void DDASHit::setChannelLength(uint32_t channelLength) { channellength = channelLength; }
    void DDASHit::setOverflowCode(uint32_t overflowBit) { overflowcode = overflowBit; }
    void DDASHit::setFinishCode(bool finishCode) { finishcode = finishCode; }
    void DDASHit::setCoarseTime(uint64_t time) { coarsetime = time; }
    void DDASHit::setRawCFDTime(uint32_t data) { timecfd = data; }
    void DDASHit::setCFDTrigSourceBit(uint32_t bit) {cfdtrigsourcebit = bit; }
    void DDASHit::setCFDFailBit(uint32_t bit) {cfdfailbit = bit; }
    void DDASHit::setTimeLow(uint32_t datum) { timelow = datum; }
    void DDASHit::setTimeHigh(uint32_t datum) { timehigh = datum & 0xffff; }

    void DDASHit::setTime(double compTime) { time = compTime; }
    void DDASHit::setEnergy(uint32_t value) { energy = value; }
    void DDASHit::setTraceLength(uint32_t length) { tracelength = length; }
    void DDASHit::setADCFrequency(uint32_t value) { ModMSPS = value; }
    void DDASHit::setADCResolution(int value) { m_adcResolution = value; }
    void DDASHit::setHardwareRevision(int value) { m_hdwrRevision = value; }
    void DDASHit::appendEnergySum(uint32_t value) { energySums.push_back(value); }
    void DDASHit::appendQDCSum(uint32_t value) { qdcSums.push_back(value); }
    void DDASHit::appendTraceSample(uint16_t value) { trace.push_back(value); }

    void DDASHit::setExternalTimestamp(uint64_t value) { externalTimestamp = value; }
    void DDASHit::setADCOverflowUnderflow(bool state) { m_adcOverflowUnderflow = state; }

  } // end DDAS namespace
} // end DAQ namespace
