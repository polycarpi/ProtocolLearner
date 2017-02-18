#include "PacketAnalyser.hpp"

void PacketAnalyser::mAnalysePacket()
{
    for(auto i : vec)
    {
			if(i < 32)
			{
	    // Control
	    m_Control += 1.0f;
	}
	else if(i < 48)
	{
	    // Symbolic
	    m_Symbolics += 1.0f;
	}
	else if(i < 58)
	{
	    // Numeric
	    m_Numeric += 1.0f;
	}
	else
	{
	    // Character
	    m_Character += 1.0f;
	}
		}
		m_PacketSize = static_cast<float>(vec.size());
		m_Symbolics /= static_cast<float>(vec.size());
		m_Numeric /= static_cast<float>(vec.size());
		m_Character /= static_cast<float>(vec.size());
};
