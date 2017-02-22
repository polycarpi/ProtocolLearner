#ifndef CUDPSINK_H
#define CUDPSINK_H

#include <string>

#include "boost/asio.hpp"


class CUdpSink
{
public:
    virtual void mStartReceive() = 0;
    CUdpSink(const std::string& aAddressToReceiveOn,
             const std::uint16_t aPortToReceiveOn) : 
             m_AddressToReceiveOn(aAddressToReceiveOn),
             m_PortToReceiveOn(aPortToReceiveOn)
	{
		
	}
protected:
    const std::string m_AddressToReceiveOn;
    std::uint16_t m_PortToReceiveOn;
        
};

#endif
