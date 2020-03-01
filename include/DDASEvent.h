// DDASEvent.h
//
// A class to aggregate separate ddaschannel objects.
//
// Author: Jeromy Tompkins
// Date  : 5/6/2013   
//

#ifndef DDASEVENT_H
#define DDASEVENT_H

#include <vector>
#include <TObject.h>
#include "ddaschannel.h"

/*! \brief Encapsulates a Built DDAS event 
 *
 *  Any data that was written to disk downstream of the NSCLDAQ event
 *  builder will have a "built" structure. What that means is that the body
 *  of the physics event item will contain data from more than one DDAS 
 *  event. The DDASEvent class represents this type of data. It provides
 *  access to the events that make it up through the ddaschannel objects
 *  it owns and then also provides some useful methods for getting
 *  data from the event as a whole.
 */
class DDASEvent : public TObject
{
    private:
        std::vector<ddaschannel*> m_data; ///< Extensible array of primitive  ddaschannel objects

    public:
        //! Default constructor
        DDASEvent();

        
        /*! \brief Copy constructor
         *
         *   Implements a deep copy
         */
        DDASEvent(const DDASEvent& obj);

        /*! \brief  Assignment operator
         *
         * Performs a deep copy of the data belonging to obj.
         * There is no attempt to make this exception-safe.
         */
        DDASEvent& operator=(const DDASEvent& obj);

        /*! \brief Destructor
         *
         * Deletes the ddaschannel data objects! 
         */
        ~DDASEvent();

        
        /*! \brief Access internal, extensible array of channel data 
         *
         *   @return reference to the data 
         */ 
        std::vector<ddaschannel*>& GetData() { return m_data;}

        /*! \brief  Append channel data to event
         *
         * Appends the pointer to the internal, extensible data array.
         * There is no check that the object pointed to by the argument
         * exists, so that it is the user's responsibility to implement.
         *
         *   @param channel pointer to a ddaschannel object
         */
        void AddChannelData(ddaschannel* channel);

        //! Get number of channel-wise data in event
        UInt_t GetNEvents() const { return m_data.size(); }

        /*! \brief  Get timestamp of first channel datum
         *
         *   If data exists return timestamp of first element in the array. This should
         *   be the earliest unit of data stored by this object. If no data exists, 
         *   returns 0.
         */
        Double_t GetFirstTime() const;

        /*! \brief  Get timestamp of last channel datum
         *
         *   If data exists return timestamp of last element in the array. This should
         *   be the most recent unit of data stored by this object. If no data exists, 
         *   returns 0.
         */
        Double_t GetLastTime() const;

        
        /*! \brief  Get time difference between first and last channel data
         *
         *   If data exists return timestamp of first and last units of data stored
         *   by this object. If no data exists, return 0;
         */
        Double_t GetTimeWidth() const;

        /*! \brief Clear data vector and reset event
         * Deletes the ddaschannel data objects and resets the 
         * size of the extensible data array to zero  
         */ 
        void Reset();

        ClassDef(DDASEvent,1);
};

#endif
