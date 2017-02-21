#ifndef CUDPPROTOCOLLEARNER_H
#define CUDPPROTOCOLLEARNER_H

#include <functional>
#include <iostream>
#include <atomic>

#include "CUdpPair.hpp"
#include "boost/asio.hpp"
#include "KMeans.hpp"
#include "PacketAnalyser.hpp"

class CUdpProtocolLearner
{
public:
	void mProcessPacket(const std::vector<uint8_t> aBytesIn)
	{
		
		if(m_HighToLowUdpPair.mGetPacketsSeen() 
		  + m_LowToHighUdpPair.mGetPacketsSeen() <= m_MaxPacketsToObserve)
		{
			PacketAnalyser lPacketAnalyser(aBytesIn);
			lPacketAnalyser.mAnalysePacket();
			
			const auto lAnalysis = lPacketAnalyser.mGetAnalysis();
			m_MessageClassificationEngine.mSubmit(lAnalysis);			
		}
	}
	
	const std::uint32_t mGetNumberPacketsInLearningEngine()
	{
		return m_MessageClassificationEngine.mGetNumPoints();
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
                        m_MessageClassificationEngine(2)
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
    KMeans<float> m_MessageClassificationEngine;
};
#endif
