#ifndef CUDPSENDER_H
#define CUDPSENDER_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

#include "boost/asio.hpp"



class CUdpSender
{
public:
    CUdpSender(const std::string& aAddressToSendTo, 
               const std::uint16_t aPortToSendTo,
               boost::asio::io_service& aIoService) : 
               m_AddressToSendTo{aAddressToSendTo},
               m_PortToSendTo{aPortToSendTo},
               m_Service(aIoService)
    {
        boost::asio::ip::udp::resolver resolver(m_Service);	
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_AddressToSendTo, std::to_string(m_PortToSendTo));
        m_ReceiverEndpoint = *resolver.resolve(query);
        m_Socket = new boost::asio::ip::udp::socket(m_Service);
        m_Socket->open(boost::asio::ip::udp::v4()); 
	}
	~CUdpSender()
	{
		m_Socket->close();
		delete m_Socket;
	}
    void mSend(const std::vector<std::uint8_t>& aVecToSend)
    {
		//std::cerr << "UDP sender sending " << aVecToSend.size() << " bytes to " << m_ReceiverEndpoint << std::endl;
        m_Socket->send_to(boost::asio::buffer(aVecToSend), m_ReceiverEndpoint);	
        //std::cerr << "Synchronous send complete" << std::endl;
	}
private:
    boost::asio::io_service& m_Service;
    const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
    boost::asio::ip::udp::endpoint m_ReceiverEndpoint;
    boost::asio::ip::udp::socket * m_Socket;
};

#endif
