#ifndef PACKETANALYSISALGO_H
#define PACKETANALYSISALGO_H

class PacketAnalysisAlgo
{
public:
    virtual void mAnalyse(const std::vector<std::uint8_t>)=0;
    virtual ~PacketAnalysisAlgo()
    {
	}
};




#endif
