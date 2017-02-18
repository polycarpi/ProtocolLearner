#ifndef CUDPPROTOCOLLEARNER_H
#define CUDPPROTOCOLLEARNER_H

#include "CUdpPair.hpp"

class CUdpProtocolLearner
{
public:
    CUdpProtocolLearner(CUdpPair& aHighToLowUdpPair,
                        CUdpPair& aLowToHighUdpPair,
                        const std::uint32_t aMaxPacketsToObserve,
                        const std::uint32_t aMaxTimeToObserve_s) : 
                        m_HighToLowUdpPair(aHighToLowUdpPair), // GCC bug prevents uniform initialisation here...
                        m_LowToHighUdpPair(aLowToHighUdpPair),
                        m_MaxPacketsToObserve{aMaxPacketsToObserve},
                        m_MaxTimeToObserve_s{aMaxTimeToObserve_s}
    {
		// Set up a listener on the incoming port, along with a callback
	}
	const std::uint32_t mGetPacketsSeen()
	{
		return 0;
	}
private:
    CUdpPair& m_HighToLowUdpPair;
    CUdpPair& m_LowToHighUdpPair;
    const std::uint32_t m_MaxPacketsToObserve;
    const std::uint32_t m_MaxTimeToObserve_s;
};

#endif
