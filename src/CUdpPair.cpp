#include "CUdpPair.hpp"

void CUdpPair::mSetPacketReceivedCallback(std::function<void(const std::vector<uint8_t>)> aCallbackIn)
{
	m_PacketReceivedCallback = aCallbackIn;
}
	
void CUdpPair::mStop()
{
	m_IoService.stop();
}

void CUdpPair::mStartReceive()
{
	m_InboundSocket->async_receive_from(boost::asio::buffer(m_ReceiveBuffer), m_RemoteEndpoint,
	  boost::bind(&CUdpPair::handle_receive_from, this, boost::asio::placeholders::error,
	  boost::asio::placeholders::bytes_transferred)); 
}

const std::uint32_t CUdpPair::mGetPacketsSeen()
{
	return m_PacketsSeen;
}

const std::uint32_t CUdpPair::mGetTotalBytesSeen()
{
	return m_TotalBytesSeen;
}

void CUdpPair::handle_receive_from(const boost::system::error_code& error,
  size_t bytes_recvd)
{
    //std::cerr << "UDP pair handle_receive_from received " << bytes_recvd << " bytes from " << m_RemoteEndpoint << std::endl;
	++m_PacketsSeen;
	m_TotalBytesSeen += bytes_recvd;
	
	m_PacketReceivedCallback(std::vector<uint8_t>(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd));
	
	//std::cerr << "UDP pair forwarding " << bytes_recvd << " bytes to " << *m_FinalEndpoint << std::endl;
	const auto lTempVec = std::vector<uint8_t>(m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + bytes_recvd);
	m_OutboundSocket->send_to(boost::asio::buffer(lTempVec), *m_FinalEndpoint);
	//std::cerr << "Forwarded " << std::endl;

	mStartReceive();
}	
