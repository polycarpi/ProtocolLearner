#ifndef PACKETANALYSER_H
#define PACKETANALYSER_H

#include <vector>
#include <cstdint>

class PacketAnalyser
{
public:
	PacketAnalyser(const std::vector<uint16_t>& inVector) : vec{inVector}, m_PacketSize{0}, m_Symbolics{0}, m_Numeric{0}, m_Character{0}
	{		
	};
	void mAnalysePacket();
    
        const std::vector<float> getAnalysis()
        {
            return std::vector<float>({m_PacketSize, m_Symbolics, m_Numeric, m_Character});
        }
private:
    const std::vector<uint16_t> vec;
    float m_PacketSize;
    float m_Symbolics;
    float m_Numeric;
    float m_Character;
    float m_Control;
};

#endif
