#ifndef CUDPPROTOCOLLEARNER_H
#define CUDPPROTOCOLLEARNER_H

#include <functional>
#include <iostream>

#include "CUdpPair.hpp"
#include "boost/asio.hpp"

class CUdpProtocolLearner
{
public:
	void mProcessPacket(const std::vector<uint8_t> aBytesIn)
	{
		if(m_ExternalStopFlag or (m_HighToLowUdpPair.mGetPacketsSeen() 
		  + m_LowToHighUdpPair.mGetPacketsSeen() >= m_MaxPacketsToObserve)) 
		{
		    m_HighToLowUdpPair.mStop();
		}
	}
    CUdpProtocolLearner(CUdpPair& aHighToLowUdpPair,
                        CUdpPair& aLowToHighUdpPair,
                        const std::uint32_t aMaxPacketsToObserve,
                        const std::uint32_t aMaxTimeToObserve_s,
                        bool& aExternalStopFlag)
                        :
                        m_HighToLowUdpPair(aHighToLowUdpPair), // GCC bug prevents uniform initialisation here...
                        m_LowToHighUdpPair(aLowToHighUdpPair),
                        m_MaxPacketsToObserve{aMaxPacketsToObserve},
                        m_MaxTimeToObserve_s{aMaxTimeToObserve_s},
                        m_ExternalStopFlag{aExternalStopFlag}
    {
		m_HighToLowUdpPair.mSetPacketReceivedCallback(std::bind(&CUdpProtocolLearner::mProcessPacket, this, std::placeholders::_1));
	}
	
	const std::uint32_t mGetPacketsSeenHighToLow()
	{
		return m_HighToLowUdpPair.mGetPacketsSeen();
	}
	const std::uint32_t mGetTotalBytesSeenHighToLow()
	{
		return m_HighToLowUdpPair.mGetTotalBytesSeen();
	}
	const std::uint32_t mGetPacketsSeenLowToHigh()
	{
		return m_LowToHighUdpPair.mGetPacketsSeen();
	}
	const std::uint32_t mGetTotalBytesSeenLowToHigh()
	{
		return m_LowToHighUdpPair.mGetTotalBytesSeen();
	}	
	void mStartListening()
	{
		m_HighToLowUdpPair.mStartReceive();
		m_LowToHighUdpPair.mStartReceive();
	}


private:
    CUdpPair& m_HighToLowUdpPair;
    CUdpPair& m_LowToHighUdpPair;
    const std::uint32_t m_MaxPacketsToObserve;
    const std::uint32_t m_MaxTimeToObserve_s;
    bool& m_ExternalStopFlag;
};
#endif
