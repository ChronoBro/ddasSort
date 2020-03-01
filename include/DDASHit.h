/* DDASHit class definition header file */

#ifndef DAQ_DDAS_DDASHIT_H
#define DAQ_DDAS_DDASHIT_H

#include <vector>
#include <cstdint>


namespace DAQ {
  namespace DDAS {
    /*!  \brief Encapsulation of a generic DDAS event
     *
     * The DDASHit class is intended to encapsulate the information that is
     * emitted by the Pixie-16 dgitizer for a single event. It contains information
     * for a single channel only. It is generic because it can store data for
     * the 100 MSPS, 250 MSPS, and 500 MSPS Pixie-16 digitizers used at the lab. 
     * In general all of these contain the same set of information, however, the
     * meaning of the CFD data is different for each. The DDASHit class abstracts
     * these differences away from the user.
     *
     * This class does not provide any parsing capabilities likes its companion class
     * ddasdumper. To fill this with data, you should use the associated DDASHitUnpacker
     * class. Here is how you use it.
     *
     * \code
     * DDASHit channel;
     * DDASHitUnpacker unpacker;
     * unpacker.unpack(pData, pData+sizeOfData, channel);
     * \endcode
     *
     */
    class DDASHit { 

      private:
        /********** Variables **********/

        /* Channel events always have the following info. */
        double time;              ///< assembled time including cfd
        uint64_t coarsetime;        ///< assembled time without cfd

        uint32_t energy;              ///< energy of event
        uint32_t timehigh;            ///< bits 32-47 of timestamp
        uint32_t timelow;             ///< bits 0-31 of timestamp
        uint32_t timecfd;             ///< raw cfd time

        uint32_t finishcode;           ///< indicates whether pile-up occurred
        uint32_t channellength;        ///< number of 32-bit words of raw data
        uint32_t channelheaderlength;  ///< length of header
        uint32_t overflowcode;         ///< 1 = overflow
        uint32_t chanid;               ///< channel index
        uint32_t slotid;               ///< slot index
        uint32_t crateid;              ///< crate index

        uint32_t cfdtrigsourcebit;     ///< value of trigger source bit(s) for 250 MSPS and 500 MSPS
        uint32_t cfdfailbit;           ///< indicates whether the cfd algo failed

        uint32_t tracelength;          ///< length of stored trace

        uint32_t ModMSPS;              ///< Sampling rate of the module (MSPS)

        /* A channel may have extra information... */
        std::vector<uint32_t> energySums;  ///< Energy sum data
        std::vector<uint32_t> qdcSums;     ///< QDC sum data

        /* A waveform (trace) may be stored too. */
        std::vector<uint16_t> trace;     ///< Trace data

        uint64_t externalTimestamp;    ///< External timestamp

        int      m_hdwrRevision;       ///< hardware revision
        int      m_adcResolution;      ///< adc resolution
        bool     m_adcOverflowUnderflow;   ///< whether the ADC over- or underflowed

      public:
        /////////////////////////////////////////////////////////////
        // Canonicals

        /*! \brief Default constructor
         *
         * All member data are zero initialized.
         */
        DDASHit();

        /*! \brief Copy constructor */
        DDASHit(const DDASHit& obj) = default;

        /*! \brief Assignment operator */
        DDASHit& operator=(const DDASHit& obj) = default;

        /*! \brief Destructor  */
        ~DDASHit(); 

        /*! \brief Resets the state of all member data to that of initialization
         *
         *  For primitive types, this sets the values to 0. 
         *  For vector data (i.e. trace), the vector is cleared and resized to 0.
         */
        void Reset();

        ///////////// Data accessors

        /*! \brief Retrieve the energy
         *
         * With the advent of Pixie16 modules with 16-bit ADCs, the GetEnergy() method
         * no longer includes the ADC overflow/underflow bit. The overflow/underflow
         * bit can be accessed via the GetADCOverflowUnderflow() method instead.
         */
        uint32_t GetEnergy() const {return energy;}

        /*! \brief Retrieve most significant 16-bits of raw timestamp */
        uint32_t GetTimeHigh() const {return timehigh;}

        /*! \brief Retrieve least significant 32-bit of raw timestamp */
        uint32_t GetTimeLow() const {return timelow;}

        /*! \brief Retrieve the raw cfd time */
        uint32_t GetTimeCFD() const {return timecfd;}

        /*! \brief Retrieve computed time 
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
         *
         * \retval timestamp (units of nanoseconds)
         */
        double GetTime() const {return time;}

        /*! \brief Retrieve the 48-bit timestamp in nanoseconds without any CFD correction */
        uint64_t GetCoarseTime() const {return coarsetime;}

        /*! \brief Retrieve finish code
         *
         *  The finish code will be set to 1 if pileup was detected.
         */
        uint32_t GetFinishCode() const {return finishcode;}

        /*! \brief Retrieve number of 32-bit words that were in original data packet 
         *
         * Note that this only really makes sense to be used if the object was filled
         * with data using UnpackChannelData().
         */
        uint32_t GetChannelLength() const {return channellength;}

        /*! \brief Retrieve length of header in original data packet */
        uint32_t GetChannelLengthHeader() const {return channelheaderlength;}

        /*! \brief Retrieve the overflow code */
        uint32_t GetOverflowCode() const {return overflowcode;}

        /*! \brief Retrieve the slot that the module resided in */
        uint32_t GetSlotID() const {return slotid;}

        /*! \brief Retrieve the index of the crate the module resided in */
        uint32_t GetCrateID() const {return crateid;}

        /*! \brief Retrieve the channel index */
        uint32_t GetChannelID() const {return chanid;}

        /*! \brief Retrieve the ADC frequency of the module */
        uint32_t GetModMSPS() const {return ModMSPS;}

        /*! \brief Retrieve the hardware revision */
        int GetHardwareRevision() const {return m_hdwrRevision; }

        /*! \brief Retrieve the adc resolution */
        int GetADCResolution() const {return m_adcResolution; }

        /*! Retrieve trig source from cfd data */
        uint32_t GetCFDTrigSource() const { return cfdtrigsourcebit; }

        /*! Retreive failure bit from cfd data */
        uint32_t GetCFDFailBit() const { return cfdfailbit; }

        /*! Retrieve trace length */
        uint32_t GetTraceLength() const { return tracelength; }

        /*! Access the trace data */
        std::vector<uint16_t>& GetTrace() {return trace;}
        const std::vector<uint16_t>& GetTrace() const {return trace;}

        /*! Access the energy/baseline sum data */
        std::vector<uint32_t>& GetEnergySums() { return energySums; }
        const std::vector<uint32_t>& GetEnergySums() const { return energySums; }

        /*! Access the qdc data */
        std::vector<uint32_t>& GetQDCSums() { return qdcSums; }
        const std::vector<uint32_t>& GetQDCSums() const { return qdcSums; }

        /*! * \return Retrieve the external timestamp */
        uint64_t GetExternalTimestamp() const {return externalTimestamp; }

        /*! \brief Return the adc overflow/underflow status
         *
         * In the 12 and 14 bit modules, this is value of bit 15 in the 4th header word.
         * In the 16 bit modules, this is the value of bit 31 in the 4th header word.
         */
        bool     GetADCOverflowUnderflow() const { return m_adcOverflowUnderflow; }

        void setChannel(uint32_t channel);
        void setSlot(uint32_t slot);
        void setCrate(uint32_t crate);
        void setChannelHeaderLength(uint32_t channelHeaderLength);
        void setChannelLength(uint32_t channelLength);
        void setOverflowCode(uint32_t overflow);
        void setFinishCode(bool finishCode);
        void setCoarseTime(uint64_t time);
        void setRawCFDTime(uint32_t data);
        void setCFDTrigSourceBit(uint32_t bit);
        void setCFDFailBit(uint32_t bit);

        void setTimeLow(uint32_t datum);
        void setTimeHigh(uint32_t datum);

        void setTime(double time);
        void setEnergy(uint32_t value);
        void setTraceLength(uint32_t trace);
        void setADCFrequency(uint32_t value);
        void setADCResolution(int value);
        void setHardwareRevision(int value);
        void appendEnergySum(uint32_t value);
        void appendQDCSum(uint32_t value);
        void appendTraceSample(uint16_t value);
        void setExternalTimestamp(uint64_t tstamp);
        void setADCOverflowUnderflow(bool state);
    };
  } // end DDAS namespace
} // end DAQ namespace
#endif
