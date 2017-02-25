#ifndef CUDPTESTECHOSERVER_H
#define CUDPTESTECHOSERVER_H

#include <functional>
#include <vector>
#include <iostream>
#include <boost/array.hpp>
#include <thread>
#include <chrono>

#include "CUdpSink.hpp"
#include "CUdpSource.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"



class CUdpTestEchoServer : public CUdpSink, public CUdpSource
{
public:
	CUdpTestEchoServer(const std::string& aAddressToReceiveOn, 
	                   const std::string& aAddressToSendTo, 
                       const std::uint16_t aPortToReceiveOn,
                       const std::uint16_t aPortToSendTo,
		               boost::asio::io_service& aIoService,
		               std::function<void(std::vector<std::uint8_t>&)> aProcessorCallback) :
		               CUdpSink(aAddressToReceiveOn, aPortToReceiveOn), 
		               CUdpSource(aAddressToSendTo, aPortToSendTo),
		               m_ProcessorCallback(aProcessorCallback),	
		               m_InboundSocket(nullptr),
		               m_IoService(aIoService),
		               m_OutboundSocket(m_IoService),
		               m_RemoteEndpointToReplyTo(boost::asio::ip::address::from_string("127.0.0.1"), m_PortToSendTo)
    {

        boost::asio::ip::udp::resolver resolver(m_IoService);	
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_AddressToSendTo, std::to_string(m_PortToSendTo));
        m_RemoteEndpointToReplyTo = *resolver.resolve(query); 		
		
		m_InboundSocket = new boost::asio::ip::udp::socket(m_IoService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), m_PortToReceiveOn));	
		m_OutboundSocket.open(boost::asio::ip::udp::v4());        

		mStartReceive();
	}
		               
    void mSend(const std::vector<std::uint8_t>& aVecToSend) override
    {
		std::cerr << m_RemoteEndpointToReplyTo << std::endl;
        m_OutboundSocket.send_to(boost::asio::buffer(aVecToSend), m_RemoteEndpointToReplyTo);	
	}
	~CUdpTestEchoServer()
	{
		m_InboundSocket->close();
		delete m_InboundSocket;
	}	
private:

	void mStartReceive() override
	{
		m_InboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpointReceivingFrom,
		  boost::bind(&CUdpTestEchoServer::handle_receive_from, this, boost::asio::placeholders::error,
		  boost::asio::placeholders::bytes_transferred));
	}
	
	void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd)
	{
		std::vector<std::uint8_t> lExtractedVec(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd);
        m_ProcessorCallback(lExtractedVec);
        
		mSend(lExtractedVec);
		
		mStartReceive();
	}
	std::function<void(std::vector<std::uint8_t>&)> m_ProcessorCallback;
	boost::asio::ip::udp::socket * m_InboundSocket;
    boost::asio::io_service& m_IoService;	
	boost::asio::ip::udp::socket m_OutboundSocket;
	boost::array<char, 10> m_ReceiveBuffer;
    boost::asio::ip::udp::endpoint m_RemoteEndpointReceivingFrom;
    boost::asio::ip::udp::endpoint m_RemoteEndpointToReplyTo;
    
	
};



#endif

