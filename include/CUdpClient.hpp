#ifndef CUDPSENDER_H
#define CUDPSENDER_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

#include "boost/asio.hpp"
#include "CUdpSource.hpp"
#include "CUdpSink.hpp"
#include <boost/array.hpp>
#include "boost/bind.hpp"



class CUdpClient : public CUdpSource, public CUdpSink
{
public:
    CUdpClient(const std::string&, 
               const std::uint16_t,
               boost::asio::io_service&);
	
	~CUdpClient()
	{
		delete m_OutboundSocket;
	}
    void mSend(const std::vector<std::uint8_t>&) override;
	void mSetFrameReceivedCallback(std::function<void()>);


	
private:


	void mStartReceive() override
	{
		m_OutboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpointToSendTo,
		  boost::bind(&CUdpClient::handle_receive_from, this, boost::asio::placeholders::error,
		  boost::asio::placeholders::bytes_transferred));
	}
	void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
	{
        m_FrameReceivedCallback();
        		
		mStartReceive();
	}	
	

    void mDummyCallback(void){};
    std::function<void(void)> m_FrameReceivedCallback;
    boost::asio::io_service& m_IoService;
	boost::asio::ip::udp::endpoint m_RemoteEndpointToSendTo;    
	boost::asio::ip::udp::socket * m_OutboundSocket;
	bool m_InboundSocketSet;
	boost::array<char, 65535> m_ReceiveBuffer;
	
};

#endif
