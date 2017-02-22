#ifndef CUDPSOURCE_H
#define CUDPSOURCE_H

#include <vector>
#include "boost/asio.hpp"

class CUdpSource
{
public:

    //CUdpSource(boost::asio::io_service& aService) : m_IoService(aService)
    CUdpSource()
    {
       // m_Socket = new boost::asio::ip::udp::socket(m_IoService);
       // m_Socket->open(boost::asio::ip::udp::v4());		
	}

    virtual void mSend(const std::vector<std::uint8_t>& aVecToSend)=0;
	
	~CUdpSource()
	{
		//m_Socket->close();
		//delete m_Socket;		
	}
	
protected:
    //boost::asio::ip::udp::socket * m_Socket;
    //boost::asio::io_service& m_IoService;
};

#endif

