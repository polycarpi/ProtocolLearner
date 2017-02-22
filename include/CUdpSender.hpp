#ifndef CUDPSENDER_H
#define CUDPSENDER_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

#include "boost/asio.hpp"
#include "CUdpSource.hpp"



class CUdpSender : public CUdpSource
{
public:
    CUdpSender(const std::string& aAddressToSendTo, 
               const std::uint16_t aPortToSendTo,
               boost::asio::io_service& aIoService) : 
               m_AddressToSendTo{aAddressToSendTo},
               m_PortToSendTo{aPortToSendTo},
               m_IoService(aIoService)
    {
        boost::asio::ip::udp::resolver resolver(m_IoService);	
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_AddressToSendTo, std::to_string(m_PortToSendTo));
        m_RemoteEndpointToSendTo = *resolver.resolve(query); 
        m_OutboundSocket = new boost::asio::ip::udp::socket(m_IoService);
        m_OutboundSocket->open(boost::asio::ip::udp::v4());        
	}
	~CUdpSender()
	{
		delete m_OutboundSocket;
	}
    void mSend(const std::vector<std::uint8_t>& aVecToSend) override
    {
        m_OutboundSocket->send_to(boost::asio::buffer(aVecToSend), m_RemoteEndpointToSendTo);	
	}	
	
private:
    const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
    boost::asio::io_service& m_IoService;
	boost::asio::ip::udp::endpoint m_RemoteEndpointToSendTo;    
	boost::asio::ip::udp::socket * m_OutboundSocket;
};

#endif
