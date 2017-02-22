#ifndef CUDPTESTECHOSERVER_H
#define CUDPTESTECHOSERVER_H

#include <functional>
#include <vector>
#include <boost/array.hpp>

#include "CUdpSink.hpp"
#include "CUdpSource.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"



class CUdpTestEchoServer : public CUdpSink, public CUdpSource
{
public:
	CUdpTestEchoServer(const std::string& aAddressToReceiveOn, 
                       const std::uint16_t aPortToReceiveOn,
		               boost::asio::io_service& aIoService,
		               std::function<void(std::vector<std::uint8_t>)> aProcessorCallback) :
		               CUdpSink(aAddressToReceiveOn, aPortToReceiveOn),               
		               m_ProcessorCallback(aProcessorCallback),	
		               m_InboundSocket(nullptr),
		               m_IoService(aIoService)
    {
		m_InboundSocket = new boost::asio::ip::udp::socket(m_IoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), m_PortToReceiveOn));		
		mStartReceive();
	}
		               
    void mSend(const std::vector<std::uint8_t>& aVecToSend) override
    {
        //m_OutboundSocket.send_to(boost::asio::buffer(aVecToSend), m_RemoteEndpointToSendTo);	
	}
	~CUdpTestEchoServer()
	{
		m_InboundSocket->close();
		delete m_InboundSocket;
	}	
private:

	void mStartReceive() override
	{
		m_InboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpointToSendTo,
		  boost::bind(&CUdpTestEchoServer::handle_receive_from, this, boost::asio::placeholders::error,
		  boost::asio::placeholders::bytes_transferred)); 		
	}
	
	void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
	{
        m_ProcessorCallback(std::vector<std::uint8_t>(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd));
		
		mStartReceive();
	}
	std::function<void(std::vector<std::uint8_t>)> m_ProcessorCallback;
	boost::asio::ip::udp::socket * m_InboundSocket;
    boost::asio::io_service& m_IoService;	
	//boost::asio::ip::udp::socket m_OutboundSocket;
	boost::array<char, 10> m_ReceiveBuffer;
    boost::asio::ip::udp::endpoint m_RemoteEndpointToSendTo;
    
	
};



#endif

