#ifndef PACKETANALYSISALGO_H
#define PACKETANALYSISALGO_H

typedef enum eCharType_tag
{
	CHAR = 0,
	WHITESPACE = 1,
	NUMERAL = 2,
	PUNCTUATION = 3,
	CONTROL = 4,
	NUMTYPES = 5
} eCharType;

class PacketAnalysisAlgo
{
public:
    virtual void mAnalyse(const std::vector<std::uint8_t>)=0;
	virtual const std::vector<float> mGetAnalysis()=0;
    virtual ~PacketAnalysisAlgo()
    {
	}
protected:
    const eCharType mClassify(const std::uint8_t aChar)
    {
		eCharType lRet;
		if(aChar < 32)
		{
			lRet = CONTROL;
		}
		else if(aChar  < 33)
		{
			lRet = WHITESPACE;
		}
		else if(aChar < 48)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 58)
		{
			lRet = NUMERAL;
		}
		else if(aChar < 65)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 91)
		{
			lRet = CHAR;
		}
		else if(aChar < 97)
		{
			lRet = PUNCTUATION;
		}
		else if(aChar < 123)
		{
			lRet = CHAR;
		}
		else if(aChar < 127)
		{
			lRet = PUNCTUATION;
		}
		else
		{
			lRet = CONTROL;
		}
		return lRet;
	}
};




#endif
