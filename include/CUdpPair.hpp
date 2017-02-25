#ifndef CUDPPAIR_H
#define CUDPPAIR_H

#include <string>
#include <cstdint>
#include <functional>
#include <vector>
#include <iostream>

#include "boost/asio.hpp"
#include "boost/bind.hpp"


class CUdpPair
{
public:
    CUdpPair(const std::string& aIpAddressToListenOn,
             const std::string& aIpAddressToSendTo,
             const std::uint16_t& aPortToListenOn,
             const std::uint16_t& aPortToSendTo,
             boost::asio::io_service& aIoService) : 
             m_IpAddressToListenOn{aIpAddressToListenOn},
             m_IpAddressToSendTo{aIpAddressToSendTo},
             m_PortToReceiveOn{aPortToListenOn},
             m_PortToSendTo{aPortToSendTo},
             m_IoService(aIoService),
             m_PacketsSeen{0},
             m_TotalBytesSeen{0}
    {
		m_InboundSocket = new boost::asio::ip::udp::socket(m_IoService);
		m_LocalEndpoint = new boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(m_IpAddressToListenOn), m_PortToReceiveOn);
        m_InboundSocket->open(m_LocalEndpoint->protocol());
        m_InboundSocket->bind(*m_LocalEndpoint);
        
        if(m_PortToSendTo != 0)
        {
			m_OutboundSocket = new boost::asio::ip::udp::socket(m_IoService);
			m_FinalEndpoint = new boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(m_IpAddressToSendTo), m_PortToSendTo);
	        m_OutboundSocket->open(m_LocalEndpoint->protocol());
	    }
	}
	
	void mSetOutboundPortAndOpenSocket(const std::uint16_t aNewPort)
	{
		m_OutboundSocket = new boost::asio::ip::udp::socket(m_IoService);
		
		m_PortToSendTo = aNewPort;
		m_FinalEndpoint = new boost::asio::ip::udp::endpoint(
		   boost::asio::ip::address::from_string(m_IpAddressToSendTo), 
		   m_PortToSendTo);
        
        m_OutboundSocket->open(m_LocalEndpoint->protocol());
	}
	
	void mSetPacketReceivedCallback(std::function<void(const std::vector<uint8_t>)>);
	void mStop();
	void mStartReceive();
	const std::uint32_t mGetPacketsSeen();
	const std::uint32_t mGetTotalBytesSeen();
	~CUdpPair()
	{
		delete m_InboundSocket;
		delete m_OutboundSocket;
		delete m_LocalEndpoint;
		delete m_FinalEndpoint;
	}
    void handle_receive_from(const boost::system::error_code&, size_t);
	
	const std::uint16_t mGetPortOfClient()
	{
		return m_RemoteEndpoint.port();
	}
	
private:
    const std::string m_IpAddressToListenOn;
    const std::string m_IpAddressToSendTo;
    const std::uint16_t m_PortToReceiveOn;
    std::uint16_t m_PortToSendTo;
    std::function<void(const std::vector<uint8_t>)> m_PacketReceivedCallback;
    boost::asio::io_service& m_IoService;
    boost::asio::ip::udp::socket * m_InboundSocket;
    boost::asio::ip::udp::socket * m_OutboundSocket;    
    std::array<std::uint8_t, 65535> m_ReceiveBuffer;
    boost::asio::ip::udp::endpoint * m_LocalEndpoint;
    boost::asio::ip::udp::endpoint * m_FinalEndpoint;
    boost::asio::ip::udp::endpoint m_RemoteEndpoint;
    std::uint32_t m_PacketsSeen;
    std::uint32_t m_TotalBytesSeen;
};

#endif
