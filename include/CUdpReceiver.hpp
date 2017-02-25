#ifndef CUDPRECEIVER_H
#define CUDPRECEIVER_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>
#include <functional>

#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include <boost/array.hpp>

#include "CUdpSink.hpp"

class CUdpReceiver : public CUdpSink
{
public:
    CUdpReceiver(const std::string& aAddressToReceiveOn, 
                 const std::uint16_t aPortToReceiveOn,
                 boost::asio::io_service& aIoService) : 
                 CUdpSink(aAddressToReceiveOn, aPortToReceiveOn),
                 m_InboundSocket(nullptr),
                 m_PacketsSeen(0),
                 m_TotalBytesSeen(0),                
                 m_IoService(aIoService),
                 m_FrameReceivedCallback(CUdpReceiver::mDummyCallback)

    {
		m_InboundSocket = new boost::asio::ip::udp::socket(m_IoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), m_PortToReceiveOn));
        mStartReceive();
      
	}
	~CUdpReceiver()
	{
		m_InboundSocket->close();
		delete m_InboundSocket;
	}

	const std::uint32_t mGetPacketsSeen()
	{
		return m_PacketsSeen;
	}
	const std::uint32_t mGetTotalBytesSeen()
	{
		return m_TotalBytesSeen;
	}
	
	void mStartReceive() override
	{		
		m_InboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpoint,
		  boost::bind(&CUdpReceiver::handle_receive_from, this, boost::asio::placeholders::error,
		  boost::asio::placeholders::bytes_transferred)); 
	}	
	
	void mSetFrameReceivedCallback(std::function<void(const std::vector<std::uint8_t>&)> aCallbackIn)
	{
		m_FrameReceivedCallback = aCallbackIn;
	}
	
private:

    static void mDummyCallback(std::vector<std::uint8_t>& aBytesIn)
    {
	}

	void handle_receive_from(const boost::system::error_code& error,
	  size_t bytes_recvd)
	{
		++m_PacketsSeen;
		m_TotalBytesSeen += bytes_recvd;
		std::vector<std::uint8_t> lTemp(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd);
		m_FrameReceivedCallback(lTemp);
		mStartReceive();
	}	

    boost::asio::ip::udp::socket * m_InboundSocket;
    std::uint32_t m_PacketsSeen;
    std::uint32_t m_TotalBytesSeen;
    boost::asio::io_service& m_IoService;
    boost::array<char, 10> m_ReceiveBuffer;  
    boost::asio::ip::udp::endpoint m_RemoteEndpoint;
    std::function<void(std::vector<std::uint8_t>&)> m_FrameReceivedCallback;
    
};

#endif

