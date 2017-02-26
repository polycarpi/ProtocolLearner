#ifndef CUDPSOURCE_H
#define CUDPSOURCE_H

#include <vector>
#include "boost/asio.hpp"

class CUdpSource
{
public:

    CUdpSource(const std::string& aAddressToSendTo, const std::uint16_t aPortToSendTo) :
      m_AddressToSendTo(aAddressToSendTo),
      m_PortToSendTo(aPortToSendTo),
      m_NumberPacketsSent(0)
    {}

    virtual void mSend(const std::vector<std::uint8_t>& aVecToSend)=0;
	

	
protected:
	const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
    std::uint32_t m_NumberPacketsSent;
};

#endif

