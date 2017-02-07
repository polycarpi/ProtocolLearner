#include <iostream>
#include <pcap.h>
#include <vector>
#include <iomanip>
#include <assert.h>

using namespace std;



class StreamAnalyser
{
public:
	StreamAnalyser(const std::vector<uint16_t>& inVector) : vec{inVector}
	{		
	};
	void AnalyseStream()
	{
		float control = 0.0f, symbolic = 0.0f, numeric = 0.0f, character = 0.0f;
	    for(auto i : vec)
	    {
    	    if(i < 32)
    	    {
				// Control
				control += 1.0f;
			}
			else if(i < 48)
			{
				// Symbolic
				symbolic += 1.0f;
			}
			else if(i < 58)
			{
				// Numeric
				numeric += 1.0f;
			}
			else
			{
				// Character
				character += 1.0f;
			}
        }
        std::cout << vec.size() << " : (" << control / static_cast<float>(vec.size()) << ", " << 
        symbolic / static_cast<float>(vec.size()) << ", " << 
        numeric / static_cast<float>(vec.size()) << ", " << 
        character / static_cast<float>(vec.size()) << ")" << std::endl;
	};
private:
    const std::vector<uint16_t> vec;

};







void packetHandler(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	// Update the learning engine
	std::cout << pkthdr->ts.tv_sec << "." << pkthdr->ts.tv_usec << std::endl;
	uint length = pkthdr->len;
	std::vector<uint16_t> lByteVec;
	for(uint lIt = 0; lIt < length; ++lIt)
	{
		lByteVec.push_back(static_cast<uint16_t>(packet[lIt]));
	}
    StreamAnalyser analyser(lByteVec);
    analyser.AnalyseStream();
}

int main(int c, char * argv[])
{
	
  std::string deviceString;
  pcap_t * descr;
  char errorBuffer[PCAP_ERRBUF_SIZE];

  pcap_if_t * device;
  
  if(-1 == pcap_findalldevs(&device, errorBuffer))
  {
	  std::cerr << "Unable to open any devices" << std::endl;
  }
  
  while(device)
  {
	  if(0 == std::string(argv[1]).compare(std::string(device->name)))
	  {
		  deviceString = std::string(device->name);
		  std::cout << "Got " << deviceString << std::endl;
		  break;
	  }
      device = device->next;
  }
  
  descr = pcap_open_live(deviceString.c_str(), BUFSIZ, 0, -1, errorBuffer);
  if (descr == NULL)
  {
      cout << "pcap_open_live() failed: " << errorBuffer << endl;
      return 1;
  }

  if (pcap_loop(descr, 0, packetHandler, NULL) < 0)
  {
      cout << "pcap_loop() failed: " << pcap_geterr(descr);
      return 1;
  }
  


  return 0;
}
