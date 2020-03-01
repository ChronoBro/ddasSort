/* ddaschannel class definition header file */

#ifndef DDASCHANNEL_H
#define DDASCHANNEL_H

#include <vector>
#include <stdint.h>
#include "TObject.h"

namespace DAQ {
  namespace DDAS {
    class DDASHit;
  }
}

/*!  \brief Encapsulation of a generic DDAS event
*
* The ddaschannel class is intended to encapsulate the information that is
* emitted by the Pixie-16 dgitizer for a single event. It contains information
* for a single channel only. It is generic because it can store data for
* the 100 MSPS, 250 MSPS, and 500 MSPS Pixie-16 digitizers used at the lab. 
* In general all of these contain the same set of information, however, the
* meaning of the CFD data is different for each. The ddaschannel class abstracts
* these differences away from the user.
*
* This class provides a raw data parser (\see UnpackChannelData) that should be
* used to fill the item with data. It is encouraged to construct complete
* events from raw data using it. For example,
*
* \code
* ddaschannel channel;
* channel.UnpackChannelData( pDataBuffer );
* \endcode
*
* It should also be noted that the ddaschannel can be persistently stored in a 
* ROOT file. It inherits from TObject and has an appropriate dictionary generated
* for it that will stream it to and from a file. In fact, the ddasdumper program
* creates a TTree filled with ddaschannel object. 
*
*/
class ddaschannel : public TObject {
public:

  /********** Variables **********/

  // ordering is important with regards to access and file size.  Should
  // always try to maintain order from largest to smallest data type
  // Double_t, Int_t, Short_t, Bool_t, pointers

  /* Channel events always have the following info. */
  Double_t time;              ///< assembled time including cfd
  Double_t coarsetime;        ///< assembled time without cfd
  Double_t cfd;               ///< cfd time only \deprecated

  UInt_t energy;              ///< energy of event
  UInt_t timehigh;            ///< bits 32-47 of timestamp
  UInt_t timelow;             ///< bits 0-31 of timestamp
  UInt_t timecfd;             ///< raw cfd time

  Int_t channelnum;           ///< \deprecated
  Int_t finishcode;           ///< indicates whether pile-up occurred
  Int_t channellength;        ///< number of 32-bit words of raw data
  Int_t channelheaderlength;  ///< length of header
  Int_t overflowcode;         ///< 1 = overflow
  Int_t chanid;               ///< channel index
  Int_t slotid;               ///< slot index
  Int_t crateid;              ///< crate index
  Int_t id;                   ///< \deprecated

  Int_t cfdtrigsourcebit;     ///< value of trigger source bit(s) for 250 MSPS and 500 MSPS
  Int_t cfdfailbit;           ///< indicates whether the cfd algo failed

  Int_t tracelength;          ///< length of stored trace

  Int_t ModMSPS;              ///< Sampling rate of the module (MSPS)
  Int_t m_adcResolution;      ///< adc resolution (i.e. bit depth)
  Int_t m_hdwrRevision;       ///< hardware revision
  Bool_t m_adcOverUnderflow;  ///< whether adc overflowed or underflowed

  /* A channel may have extra information... */
  std::vector<UInt_t> energySums;  ///< Energy sum data
  std::vector<UInt_t> qdcSums;   ///< QDC sum data
  
  /* A waveform (trace) may be stored too. */
  std::vector<UShort_t> trace;     ///< Trace data

  Double_t externalTimestamp;  ///< External clock
  /////////////////////////////////////////////////////////////
  // Canonicals

  /*! \brief Default constructor
   *
   * All member data are zero initialized.
   */
  ddaschannel();
  

  /*! \brief Copy constructor */
  ddaschannel(const ddaschannel& obj) = default;

  /*! \brief Assignment operator */
  ddaschannel& operator=(const ddaschannel& obj) = default;

  /*! \brief Copy DDASHit into ddaschannel
   *
   * This copies the contents of an existing DDASHit into
   * the ddaschannel object.  For data members that are not identical
   * between the DDASHit and ddaschannel class, this does a best effort
   * at handling them appropriately. These are those values and how they
   * are handled:
   *
   * | ddaschannel data member |  handling      |
   * |-------------------------|----------------|
   * | cfd                     | set to zero    |
   * | channelnum              | set to value of DDASHit::GetChannelID() |
   * | id                      | set to zero    |
   *
   * \param hit   the hit to copy
   */
  ddaschannel& operator=(DAQ::DDAS::DDASHit& hit);

  /*! \brief Destructor  */
  ~ddaschannel();

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
   * object. Prior to parsing, all data members are reset to 0 using the Reset()
   * method.
   */
  void UnpackChannelData(const uint32_t *data);

  /*! \brief Resets the state of all member data to that of initialization
   *
   *  For primitive types, this sets the values to 0. 
   *  For vector data (i.e. traces), the vector is cleared and resized to 0.
   */
  void Reset();

  ///////////// Data accessors
  
  /*! \brief Retrieve the energy  */
  UInt_t GetEnergy() const {return energy;}

  /*! \brief Retrieve most significant 16-bits of raw timestamp */
  UInt_t GetTimeHigh() const {return timehigh;}

  /*! \brief Retrieve least significant 32-bit of raw timestamp */
  UInt_t GetTimeLow() const {return timelow;}

  /*! \brief Retrieve the raw cfd time */
  UInt_t GetCFDTime() const {return timecfd;}

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
  Double_t GetTime() const {return time;}

  /*! \brief Retrieve the 48-bit timestamp in nanoseconds without any CFD correction */
  Double_t GetCoarseTime() const {return coarsetime;}

  /*! \deprecated */
  Double_t GetCFD() const {return cfd;}

  /*! \brief Retrieve specific energy sum
   *
   *  \param i  index of energy sum to access
   *
   *  There is no bound checking here. The caller is responsible for ensuring that
   *  the data exists to be retrieved.
   */
  UInt_t GetEnergySums(Int_t i) const {return energySums[i];}

  /*! \brief Retrieve the channel number */
  Int_t GetChannelNum() const {return channelnum;}

  /*! \brief Retrieve finish code
   *
   *  The finish code will be set to 1 if pileup was detected.
   */
  Int_t GetFinishCode() const {return finishcode;}

  /*! \brief Retrieve number of 32-bit words that were in original data packet 
   *
   * Note that this only really makes sense to be used if the object was filled
   * with data using UnpackChannelData().
   */
  Int_t GetChannelLength() const {return channellength;}

  /*! \brief Retrieve length of header in original data packet */
  Int_t GetChannelLengthHeader() const {return channelheaderlength;}

  /*! \brief Retrieve the overflow code */
  Int_t GetOverflowCode() const {return overflowcode;}

  /*! \brief Retrieve the slot that the module resided in */
  Int_t GetSlotID() const {return slotid;}

  /*! \brief Retrieve the index of the crate the module resided in */
  Int_t GetCrateID() const {return crateid;}

  /*! \brief Retrieve the channel index */
  Int_t GetChannelID() const {return chanid;}

  /*! \deprecated */
  Int_t GetID() const {return id;}

  /*! \brief Retrieve the ADC frequency of the module */
  Int_t GetModMSPS() const {return ModMSPS;}

  /*! \brief Retrieve a copy of the trace stored */
  std::vector<UShort_t> GetTrace() const {return trace;}

  Int_t GetADCResolution() const { return m_adcResolution; }
  Int_t GetHardwareRevision() const { return m_hdwrRevision; }
  Bool_t GetADCOverflowUnderflow() const { return m_adcOverUnderflow; }

  uint32_t GetCfdTrigSource() const { return cfdtrigsourcebit; }; // Per S.L. request.
  
  ClassDef(ddaschannel, 5)
};

#endif
