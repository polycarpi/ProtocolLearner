#ifndef CUDPSENDER_H
#define CUDPSENDER_H

#include <string>
#include <cstdint>
#include <vector>

class CUdpSender
{
public:
    CUdpSender(const std::string& aAddressToSendTo, 
               const std::uint16_t aPortToSendTo) : 
               m_AddressToSendTo{aAddressToSendTo},
               m_PortToSendTo{aPortToSendTo}
               {}
    void mSend(const std::vector<std::uint8_t>& aVecToSend)
    {
	}
private:
    const std::string m_AddressToSendTo;
    const std::uint16_t m_PortToSendTo;
};


#endif
