#ifndef CUDPSENDER_H
#define CUDPSENDER_H

#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

#include "boost/asio.hpp"
#include "CUdpSource.hpp"



class CUdpSender : public CUdpSource
{
public:
    CUdpSender(const std::string& aAddressToSendTo, 
               const std::uint16_t aPortToSendTo,
               boost::asio::io_service& aIoService) : 
               CUdpSource(aIoService),
               m_AddressToSendTo{aAddressToSendTo},
               m_PortToSendTo{aPortToSendTo}
    {
        boost::asio::ip::udp::resolver resolver(m_Service);	
        boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), m_AddressToSendTo, std::to_string(m_PortToSendTo));
        m_RemoteEndpointToSendTo = *resolver.resolve(query); 
	}
private:
    const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
};

#endif
