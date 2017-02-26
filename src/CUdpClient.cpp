#include "CUdpClient.hpp"

CUdpClient::CUdpClient(const std::string& aAddressToSendTo, 
		   const std::uint16_t aPortToSendTo,
		   boost::asio::io_service& aIoService) : 
		   m_IoService(aIoService),
		   CUdpSource(aAddressToSendTo, aPortToSendTo),
		   m_InboundSocketSet(false)
{
	boost::asio::ip::udp::resolver resolver(m_IoService);	
	boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_AddressToSendTo, std::to_string(m_PortToSendTo));
	m_RemoteEndpointToSendTo = *resolver.resolve(query); 
	
	m_OutboundSocket = new boost::asio::ip::udp::socket(m_IoService);
	m_OutboundSocket->open(boost::asio::ip::udp::v4());        
	
	mSetFrameReceivedCallback(std::bind(&CUdpClient::mDummyCallback, this));
	
	mStartReceive();
}

void CUdpClient::mSend(const std::vector<std::uint8_t>& aVecToSend)
{
	m_OutboundSocket->send_to(boost::asio::buffer(aVecToSend), m_RemoteEndpointToSendTo);
	++m_NumberPacketsSent;
	
	if(!m_InboundSocketSet)
	{
		mReadParametersFromLocalSocket(m_OutboundSocket);
		m_InboundSocketSet = true;
    }
	
}
void CUdpClient::mSetFrameReceivedCallback(std::function<void()> aFrameReceivedCallbackIn)
{
	m_FrameReceivedCallback = aFrameReceivedCallbackIn;
}
