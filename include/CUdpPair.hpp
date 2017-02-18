#ifndef CUDPPAIR_H
#define CUDPPAIR_H

#include <string>
#include <cstdint>
#include <functional>
#include <vector>

class CUdpPair
{
public:
    CUdpPair(const std::string& aIpAddressToListenOn, 
             const std::string& aIpAddressToSendTo,
             const std::uint16_t& aPortToListenOnAndSendTo,
             const std::uint32_t& aBufferSize) : 
             m_IpAddressToListenOn{aIpAddressToListenOn},
             m_IpAddressToSendTo{aIpAddressToSendTo},
             m_Port{aPortToListenOnAndSendTo},
             m_BufferSize{aBufferSize}
    {
	}
	void mSetPacketReceivedCallback(std::function<void(const std::vector<uint8_t>)> aCallbackIn)
	{
		m_PacketReceivedCallback = aCallbackIn;
	}
private:
    const std::string m_IpAddressToListenOn;
    const std::string m_IpAddressToSendTo;
    const std::uint16_t m_Port;
    std::uint32_t m_BufferSize;
    std::function<void(const std::vector<uint8_t>)> m_PacketReceivedCallback;
    
};

#endif
