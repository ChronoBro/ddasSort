#include "DDASEvent.h"

ClassImp(DDASEvent)

DDASEvent::DDASEvent() : TObject(), m_data() {}

DDASEvent::DDASEvent(const DDASEvent& obj)
   : TObject(obj), m_data()
{
    // Create new copies of the ddaschannel events
    for (UInt_t i=0; i<m_data.size(); ++i) {
        m_data[i] = new ddaschannel(*obj.m_data[i]);
    }
    
}

DDASEvent& DDASEvent::operator=(const DDASEvent& obj)
{
    // This assignment operator is simple at the expense of 
    // some safety. The entirety of the data vector is deleted
    // prior to assignment. If the initialization of the 
    // ddaschannel objects threw an exception or caused something 
    // else to happen that is bad, then it would be a big problem.
    // The possibility does exist for this to happen. Coding up 
    // a safer version is just more complex, harder to understand,
    // and will be slower.

    if (this!=&obj) {
        // Create new copies of the ddaschannel events

        for (UInt_t i=0; i<m_data.size(); ++i) {
            delete m_data[i];
        }

        m_data.resize(obj.m_data.size());

        for (UInt_t i=0; i<m_data.size(); ++i) {
            m_data[i] = new ddaschannel(*obj.m_data[i]);
        }

    }

    return *this;
}

DDASEvent::~DDASEvent()
{
    Reset();
}


void DDASEvent::AddChannelData(ddaschannel* channel)
{
    m_data.push_back(channel);
}

Double_t DDASEvent::GetFirstTime() const
{
    Double_t time=0;

    if (m_data.size()>0) 
        time = m_data.front()->GetTime();
    
    return time;
}

Double_t DDASEvent::GetLastTime() const
{
    Double_t time=0;

    if (m_data.size()>0) 
        time = m_data.back()->GetTime();
    
    return time;
}

Double_t DDASEvent::GetTimeWidth() const
{
    Double_t end_time   = GetLastTime();
    Double_t start_time = GetFirstTime();
    
    return (end_time-start_time);
}

void DDASEvent::Reset()
{
    // Delete all of the object stored in m_data
    for (UInt_t i=0; i<m_data.size(); ++i) {
        delete m_data[i];
    }
    
    // Clear the array and resize it to zero
    m_data.clear();
}
