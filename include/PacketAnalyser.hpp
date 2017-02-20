#ifndef PACKETANALYSER_H
#define PACKETANALYSER_H

#include <vector>
#include <cstdint>

/*
 * Time
 * Length
 * Direction
 * Repeated structure
 */

class PacketAnalyser
{
public:
	PacketAnalyser(const std::vector<uint8_t>& inVector) : m_Vec{inVector}, m_PacketSize{0}, m_Symbolics{0}, m_Numeric{0}, m_Character{0}
	{		
	};
	void mAnalysePacket();
    
	const std::vector<float> mGetAnalysis()
	{
		return std::vector<float>({m_PacketSize, m_Symbolics, m_Numeric, m_Character});
	}
private:
    const std::vector<uint8_t> m_Vec;
    float m_PacketSize;
    float m_Symbolics;
    float m_Numeric;
    float m_Character;
    float m_Control;
};

#endif
