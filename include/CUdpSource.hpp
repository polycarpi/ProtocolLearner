#ifndef CUDPSOURCE_H
#define CUDPSOURCE_H

#include <vector>
#include "boost/asio.hpp"

class CUdpSource
{
public:

    //CUdpSource(boost::asio::io_service& aService) : m_IoService(aService)
    CUdpSource(const std::string& aAddressToSendTo, const std::uint16_t aPortToSendTo) :
      m_AddressToSendTo(aAddressToSendTo),
      m_PortToSendTo(aPortToSendTo)
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
	const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
};

#endif

