#include "PacketAnalyser.hpp"

void PacketAnalyser::mAnalysePacket()
{
	m_AlgoToRun->mAnalyse(m_Vec);
};
