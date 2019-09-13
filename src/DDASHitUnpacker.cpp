
#include "DDASHitUnpacker.h"
#include "DDASBitMasks.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace DAQ {
  namespace DDAS {

    const uint32_t* DDASHitUnpacker::unpack(const uint32_t* beg, const uint32_t* sentinel, DDASHit& hit)
    {

      if (beg == sentinel) {
        throw std::runtime_error("DDASHitUnpacker::unpack() Unable to parse empty data buffer.");
      }

      const uint32_t* data = beg;

      data = parseBodySize(data, sentinel);
      data = parseModuleInfo(hit, data);
      data = parseHeaderWord0(hit, data);
      data = parseHeaderWords1And2(hit, data);
      data = parseHeaderWord3(hit, data);


      // finished upacking the minimum set of data

      uint32_t channelheaderlength = hit.GetChannelLengthHeader();
      uint32_t channellength = hit.GetChannelLength();
      size_t tracelength = hit.GetTraceLength();
      //more unpacking data
      if(channellength != (channelheaderlength + tracelength/2)){
        std::stringstream errmsg;
        errmsg << "FATAL: Data corruption: Inconsistent data lengths found in header ";
        errmsg << "\nChannel length = " << std::setw(8) << channellength;
        errmsg << "\nHeader length  = " << std::setw(8) << channelheaderlength;
        errmsg << "\nTrace length   = " << std::setw(8) << tracelength;

        throw std::runtime_error(errmsg.str());
      }

      //if channel header length is 8 then the extra 4 words are energy sums and baselines
      if(channelheaderlength == 6) {
        data = extractExternalTimestamp(data, hit);

      } else if(channelheaderlength == 8) {
        data = extractEnergySums(data, hit);

      } else if (channelheaderlength == 10) {
        data = extractEnergySums(data, hit);
        data = extractExternalTimestamp(data, hit);

      } else if(channelheaderlength == 12) {
        data = extractQDC(data, hit);

      } else if (channelheaderlength == 14) {
        data = extractQDC(data, hit);
        data = extractExternalTimestamp(data, hit);

      } else if(channelheaderlength == 16) {
        //if channel header length is 16 then the extra 12 words are energy and QDC sums
        data = extractEnergySums(data, hit);
        data = extractQDC(data, hit);

      } else if(channelheaderlength == 18) {
        //if channel header length is 16 then the extra 12 words are energy and QDC sums
        data = extractEnergySums(data, hit);
        data = extractQDC(data, hit);
        data = extractExternalTimestamp(data, hit);

      }

      // if trace length is non zero, retrieve the trace
      if(tracelength !=0) {
        data = parseTraceData(hit, data);
      } //finished unpacking trace


      return data;
    }

    /////////////////////////////////////////////////////////////////////////
    //
    tuple<DDASHit, const uint32_t*> DDASHitUnpacker::unpack(const uint32_t *beg, const uint32_t* sentinel)
    {

      DDASHit hit;

      const uint32_t* data = unpack(beg, sentinel, hit);

      return make_tuple(hit, data);
    }


    const uint32_t* DDASHitUnpacker::parseTraceData(DDASHit& hit, const uint32_t* data)
    {
      vector<uint16_t>& trace = hit.GetTrace();

      size_t tracelength = hit.GetTraceLength();
      trace.reserve(tracelength);
      for(size_t z = 0; z < tracelength/2; z++){
        uint32_t datum = *data++;
        trace.push_back(datum & LOWER16BITMASK);
        trace.push_back((datum & UPPER16BITMASK)>>16);
      }   

      return data;
    }



    const uint32_t* DDASHitUnpacker::parseBodySize(const uint32_t* data, const uint32_t* sentinel) {
      uint32_t nShorts = *data; 

      // make sure there is enough data to parse
      if ((data + nShorts/sizeof(uint16_t) > sentinel) && (sentinel != nullptr)) {
        throw std::runtime_error("DDASHitUnpacker::unpack() Incomplete event data.");
      }

      return (data+1);
    }

    const uint32_t* DDASHitUnpacker::parseModuleInfo(DDASHit& hit, 
                                                     const uint32_t* data) {
      uint32_t datum = *data++;

      //next word is the module revision, adc bit, and msps
      hit.setADCFrequency(datum & LOWER16BITMASK);
      hit.setADCResolution((datum>>16) & 0xff);
      hit.setHardwareRevision((datum>>24) & 0xff);

      return data;
    }


    const uint32_t* DDASHitUnpacker::parseHeaderWord0(DDASHit& hit, 
                                                     const uint32_t* data) {
      uint32_t datum = *data++;
      //Using the first word of DDAS information extract channel identifiers
      hit.setChannel(datum & CHANNELIDMASK);
      hit.setSlot((datum & SLOTIDMASK) >> 4);
      hit.setCrate((datum & CRATEIDMASK) >> 8);
      hit.setChannelHeaderLength((datum & HEADERLENGTHMASK) >> 12 );
      hit.setChannelLength((datum & CHANNELLENGTHMASK) >> 17);
      hit.setOverflowCode((datum & OVERFLOWMASK) >> 30);
      hit.setFinishCode((datum & FINISHCODEMASK) >> 31 );
      
      return data;
    }

    const uint32_t* DDASHitUnpacker::parseHeaderWords1And2(DDASHit& hit, 
                                                           const uint32_t* data) {
      uint32_t timelow      = *data++;
      uint32_t datum1       = *data++;
      uint32_t timehigh     = datum1&0xffff;
      uint32_t adcFrequency = hit.GetModMSPS();

      double   cfdCorrection;
      uint32_t cfdtrigsource, cfdfailbit, timecfd;
      uint64_t coarseTime;

      coarseTime = computeCoarseTime(adcFrequency, timelow, timehigh) ;
//      tie(cfdCorrection, timecfd, cfdtrigsource, cfdfailbit)
//                                 = parseAndComputeCFD(adcFrequency, datum1);
      cfdCorrection = parseAndComputeCFD(hit, datum1);

      hit.setTimeLow(timelow);
      hit.setTimeHigh(timehigh);
      hit.setCoarseTime( coarseTime ); 
//      hit.setRawCFDTime(timecfd);
//      hit.setCFDTrigSourceBit( cfdtrigsource );
//      hit.setCFDFailBit( cfdfailbit );
      hit.setTime(static_cast<double>(coarseTime) + cfdCorrection);

      return data;
    }

    const uint32_t* DDASHitUnpacker::parseHeaderWord3(DDASHit& hit, 
                                                      const uint32_t* data) {

      if (hit.GetADCResolution()==16 && hit.GetModMSPS()==250) {
          hit.setTraceLength((*data >> 16) & 0x7fff);
          hit.setADCOverflowUnderflow(*data >> 31);
          hit.setEnergy(*data & LOWER16BITMASK);
      } else {
          hit.setTraceLength((*data & UPPER16BITMASK) >> 16);
          hit.setADCOverflowUnderflow((*data >> 15) & 0x1);
          hit.setEnergy((*data & 0x7fff));
      }

      return (data+1);
    }

    tuple<double, uint32_t, uint32_t, uint32_t>
    DDASHitUnpacker::parseAndComputeCFD(uint32_t ModMSPS, uint32_t data) {

      double correction;
      uint32_t cfdtrigsource, cfdfailbit, timecfd;

      //check on the module MSPS and pick the correct cfd unpacking algorithm 
      if(ModMSPS == 100){
        // 100 MSPS modules don't have trigger source bits
        cfdfailbit    = ((data & BIT31MASK) >> 31) ; 
        cfdtrigsource = 0;
        timecfd       = ((data & BIT30to16MASK) >> 16);
        correction    = (timecfd/32768.0) * 10.0; // 32768 = 2^15 ,
      }
      else if (ModMSPS == 250) {
        // cfd fail bit in bit 31
        cfdfailbit    = ((data & BIT31MASK) >> 31 );
        cfdtrigsource = ((data & BIT30MASK) >> 30 );
        timecfd       = ((data & BIT29to16MASK) >> 16);
        correction    = (timecfd/16384.0 - cfdtrigsource)*4.0; 
      }
      else if (ModMSPS == 500) {
        // no fail bit in 500 MSPS modules
        cfdfailbit    = 0;      
        cfdtrigsource = ((data & BIT31to29MASK) >> 29 );
        timecfd       = ((data & BIT28to16MASK) >> 16);
        correction    = (timecfd/8192.0 + cfdtrigsource - 1)*2.0;
      }

      return make_tuple(correction, timecfd, cfdtrigsource, cfdfailbit);

    }

    double
    DDASHitUnpacker::parseAndComputeCFD(DDASHit& hit, uint32_t data)
    {

      double correction;
      uint32_t cfdtrigsource, cfdfailbit, timecfd;
      uint32_t ModMSPS = hit.GetModMSPS();

      //check on the module MSPS and pick the correct cfd unpacking algorithm 
      if(ModMSPS == 100){
        // 100 MSPS modules don't have trigger source bits
        cfdfailbit    = ((data & BIT31MASK) >> 31) ; 
        cfdtrigsource = 0;
        timecfd       = ((data & BIT30to16MASK) >> 16);
        correction    = (timecfd/32768.0) * 10.0; // 32768 = 2^15 ,
      }
      else if (ModMSPS == 250) {
        // cfd fail bit in bit 31
        cfdfailbit    = ((data & BIT31MASK) >> 31 );
        cfdtrigsource = ((data & BIT30MASK) >> 30 );
        timecfd       = ((data & BIT29to16MASK) >> 16);
        correction    = (timecfd/16384.0 - cfdtrigsource)*4.0; 
      }
      else if (ModMSPS == 500) {
        // no fail bit in 500 MSPS modules
        cfdfailbit    = 0;      
        cfdtrigsource = ((data & BIT31to29MASK) >> 29 );
        timecfd       = ((data & BIT28to16MASK) >> 16);
        correction    = (timecfd/8192.0 + cfdtrigsource - 1)*2.0;
      }

      hit.setCFDFailBit(cfdfailbit);
      hit.setCFDTrigSourceBit(cfdtrigsource);
      hit.setRawCFDTime(timecfd);

      return correction;
    }


    uint64_t DDASHitUnpacker::computeCoarseTime(uint32_t ModMSPS, 
                                                uint32_t timelow, uint32_t timehigh)
    {
      
      uint64_t toNanoseconds = 1;

      if(ModMSPS == 100){
        toNanoseconds = 10;
      }
      else if (ModMSPS == 250) {
        toNanoseconds = 8;
      }
      else if (ModMSPS == 500) {
        toNanoseconds = 10;
      }

      uint64_t tstamp = timehigh;
      tstamp = tstamp << 32;
      tstamp |= timelow;

      return tstamp*toNanoseconds;
    }
    
    const uint32_t* DDASHitUnpacker::extractEnergySums(const uint32_t* data, DDASHit& hit) {
          vector<uint32_t>& energies = hit.GetEnergySums();
          energies.reserve(4);
          energies.insert(energies.end(), data, data+4);
          return data + 4;
    }

    const uint32_t* DDASHitUnpacker::extractQDC(const uint32_t* data, DDASHit& hit) {
          vector<uint32_t>& qdcVals = hit.GetQDCSums();
          qdcVals.reserve(8);
          qdcVals.insert(qdcVals.end(), data, data+8);
          return data + 8;
    }

    const uint32_t* 
      DDASHitUnpacker::extractExternalTimestamp(const uint32_t* data, DDASHit& hit) 
      {
        uint64_t tstamp = 0;
        uint32_t temp = *data++;
        tstamp = *data++;
        tstamp = ((tstamp << 32) | temp); 
        hit.setExternalTimestamp(tstamp);
        return data;
      }


  }  // end DDAS namespace
} // end DAQ namespace
