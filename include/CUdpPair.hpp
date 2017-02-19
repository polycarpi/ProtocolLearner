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
             const std::uint16_t& aPortToListenOnAndSendTo,
             const std::uint32_t& aBufferSize,
             boost::asio::io_service& aIoService) : 
             m_IpAddressToListenOn{aIpAddressToListenOn},
             m_IpAddressToSendTo{aIpAddressToSendTo},
             m_Port{aPortToListenOnAndSendTo},
             m_BufferSize{aBufferSize},
             m_IoService(aIoService),
             m_PacketsSeen{0},
             m_TotalBytesSeen{0}
    {
		m_InboundSocket = new boost::asio::ip::udp::socket(m_IoService);
		m_LocalEndpoint = new boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(m_IpAddressToListenOn), m_Port);
        m_InboundSocket->open(m_LocalEndpoint->protocol());
        m_InboundSocket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
        m_InboundSocket->bind(*m_LocalEndpoint);		
	}
	void mSetPacketReceivedCallback(std::function<void(const std::vector<uint8_t>)> aCallbackIn)
	{
		m_PacketReceivedCallback = aCallbackIn;
	}
	void mStop()
	{
		m_IoService.stop();
	}
	
	void mStartReceive()
	{
        m_InboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpoint,
          boost::bind(&CUdpPair::handle_receive_from, this, boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)); 
	}
	
	const std::uint32_t mGetPacketsSeen()
	{
		return m_PacketsSeen;
	}
	
	const std::uint32_t mGetTotalBytesSeen()
	{
		return m_TotalBytesSeen;
	}
	~CUdpPair()
	{
		delete m_InboundSocket;
	}
		
    void handle_receive_from(const boost::system::error_code& error,
      size_t bytes_recvd)
    {
        ++m_PacketsSeen;
        m_TotalBytesSeen += bytes_recvd;
        m_PacketReceivedCallback(std::vector<uint8_t>(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd));
		mStartReceive();
	}		
		
private:



    const std::string m_IpAddressToListenOn;
    const std::string m_IpAddressToSendTo;
    const std::uint16_t m_Port;
    std::uint32_t m_BufferSize;
    std::function<void(const std::vector<uint8_t>)> m_PacketReceivedCallback;
    boost::asio::io_service& m_IoService;
    boost::asio::ip::udp::socket * m_InboundSocket;
    std::array<std::uint8_t, 10000> m_ReceiveBuffer;
    boost::asio::ip::udp::endpoint * m_LocalEndpoint;
    boost::asio::ip::udp::endpoint m_RemoteEndpoint;
    std::uint32_t m_PacketsSeen;
    std::uint32_t m_TotalBytesSeen;
};

#endif
