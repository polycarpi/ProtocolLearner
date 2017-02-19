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
		m_HighToLowUdpPair.mStop();
	}
    CUdpProtocolLearner(CUdpPair& aHighToLowUdpPair,
                        CUdpPair& aLowToHighUdpPair,
                        const std::uint32_t aMaxPacketsToObserve,
                        const std::uint32_t aMaxTimeToObserve_s)
                        :
                        m_HighToLowUdpPair(aHighToLowUdpPair), // GCC bug prevents uniform initialisation here...
                        m_LowToHighUdpPair(aLowToHighUdpPair),
                        m_MaxPacketsToObserve{aMaxPacketsToObserve},
                        m_MaxTimeToObserve_s{aMaxTimeToObserve_s},
                        m_PacketsSeen{0}
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
    std::uint32_t m_PacketsSeen;
};
#endif
