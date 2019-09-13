
/* DDASHit class definition header file */

#ifndef DAQ_DDAS_DDASHITUNPACKER_H
#define DAQ_DDAS_DDASHITUNPACKER_H

#include "DDASHit.h"

#include <vector>
#include <cstdint>
#include <tuple>


namespace DAQ {
  namespace DDAS {

    /*! \brief Parse the DDAS Readout event data
     *
     * \brief data  pointer to first 32-bit word of an event body
     *
     * This expects data from the DDAS Readout program. It will parse the
     * entire body of the event in a manner that is consistent with the
     * data present. In other words, it uses the sizes of the event encoded
     * in the data to determine when the parsing is complete. 
     *
     * While it parses, it stores the results into the data members of the 
     * object hit. Prior to parsing, all data members are reset to 0 using the Reset()
     * method.
     */
    class DDASHitUnpacker {
      public:
        std::tuple<DDASHit, const uint32_t*> unpack(const uint32_t* beg, const uint32_t* sentinel); 
        const uint32_t* unpack(const uint32_t* beg, const uint32_t* sentinel, DDASHit& hit); 

      protected:

        const uint32_t* parseBodySize(const uint32_t* beg, const uint32_t* sentinel);
        const uint32_t* parseModuleInfo(DDASHit& hit, const uint32_t* beg);
        const uint32_t* parseHeaderWord0(DDASHit& hit, const uint32_t* beg);
        const uint32_t* parseHeaderWords1And2(DDASHit& hit, const uint32_t* beg);
        const uint32_t* parseHeaderWord3(DDASHit& hit, const uint32_t* beg);
        const uint32_t* parseTraceData(DDASHit& hit, const uint32_t* beg);

        std::tuple<double, uint32_t, uint32_t, uint32_t> 
          parseAndComputeCFD(uint32_t ModMSPS, uint32_t data);
        double parseAndComputeCFD(DDASHit& hit, uint32_t data);


        /*! \brief Compute and store CFD-corrected time in nanoseconds
         *
         * This method performs a computation that depends on the type of the 
         * digitizer that produced the data. In each case, the coarse timestamp
         * is formed using the timelow and timehigh. This is coarse timestamp 
         * is then corrected using any CFD time that exists. 
         *
         * The calculations for the various modules are as follows:
         *
         * For the 100 MSPS module:
         *
         *  time = 10*((timehigh << 32) + timelow)
         *  
         * For the 250 MSPS module...
         *
         *  time = 8*(timehigh << 32 + timelow) + 4*(timecfd/(2^14) - cfdtrigsourcebit)
         *
         * For the 500 MSPS module,
         *
         *  time = 10*(timehigh << 32 + timelow) + 2*(timecfd/(2^13) + cfdtrigsourcebit - 1)
         */
        void SetTime(DDASHit& hit);


        /*! \brief Append energy sum to stored energy sums
         *
         * \param data  energy sum to append
         *
         * This can be called many times and will cause each successive value 
         * to be appended to objec.t
         */
        void SetEnergySums(DDASHit& hit, uint32_t data);

        /*! \brief Append qdc sum to stored qdc sums
         *
         * \param data  energy sum to store
         *
         * Like the SetEnergySums() method, this can be called repeatedly to append
         * multiple values to the existing data.
         */
        void SetQDCSums(DDASHit& hit, uint32_t data);

        /*! \brief Append trace values to stored trace data
         *
         * \param data  32-bit word storing two samples to store
         *
         * The two waveform samples are extraced as the lower and upper 16-bit 
         * data words and then appended. The least significant 16-bit word is appended
         * to the stored trace first.
         */

        /*! \brief Compute time in nanoseconds from raw data (no CFD correction)
         *
         * THis method is very similar to the SetTime() method. It differs in that it doesn
         * not apply a correction for the CFD time. It simply forms the timestamp from the
         * low and high bits and then converts it to a time in nanoseconds.
         *
         * The calculations for the various modules are as follows:
         *
         * For the 100 MSPS module:
         *
         *  time = 10*((timehigh << 32) + timelow)
         *  
         * For the 250 MSPS module...
         *
         *  time = 8*(timehigh << 32 + timelow) 
         *
         * For the 500 MSPS module,
         *
         *  time = 10*(timehigh << 32 + timelow)
         *
         */
        uint64_t computeCoarseTime(uint32_t adcFrequency, uint32_t timelow, uint32_t timehigh);

        const uint32_t* extractEnergySums(const uint32_t* data, DDASHit& hit);
        const uint32_t* extractQDC(const uint32_t* data, DDASHit& hit);
        const uint32_t* extractExternalTimestamp(const uint32_t* data, DDASHit& hit);

    };

  } // end DDAS namespace
} // end DAQ namespace
#endif
