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
	
	CUdpSink()
	{
	};
	
	void mSetParametersFromLocalSocket(const boost::asio::ip::udp::socket* aLocalSocket)
	{
		m_AddressToReceiveOn = aLocalSocket->local_endpoint().address().to_string();
		m_PortToReceiveOn = aLocalSocket->local_endpoint().port();
	}
	
	
protected:
    std::string m_AddressToReceiveOn;
    std::uint16_t m_PortToReceiveOn;
};

#endif
