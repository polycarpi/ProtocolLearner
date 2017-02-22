#ifndef CUDPSOURCE_H
#define CUDPSOURCE_H

#include <vector>
#include "boost/asio.hpp"

class CUdpSource
{
public:

    CUdpSource(boost::asio::io_service& aService) : m_Service(aService)
    {
        m_Socket = new boost::asio::ip::udp::socket(m_Service);
        m_Socket->open(boost::asio::ip::udp::v4());		
	}

    void mSend(const std::vector<std::uint8_t>& aVecToSend)
    {
        m_Socket->send_to(boost::asio::buffer(aVecToSend), m_RemoteEndpointToSendTo);	
	}
	
	~CUdpSource()
	{
		m_Socket->close();
		delete m_Socket;		
	}
	
protected:
    boost::asio::ip::udp::endpoint m_RemoteEndpointToSendTo;
    boost::asio::ip::udp::socket * m_Socket;
    boost::asio::io_service& m_Service;

};

#endif

