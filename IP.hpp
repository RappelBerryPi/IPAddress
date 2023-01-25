#ifdef _WIN32

#include <inaddr.h>

#else

#include <netinet/in.h>

#endif

#include <string>
#include <vector>

union ipAddress {
    unsigned char quartets[4];
    unsigned long ipAddress;
};

class IpV4Address {
    public:
        IpV4Address();
        IpV4Address(std::string ipAddress);
        IpV4Address(unsigned long ipAddress, unsigned long subnetMask);
        IpV4Address(unsigned char quartet_1, unsigned char quartet_2, unsigned char quartet_3, unsigned char quartet_4, unsigned char subnetMaskQuartet_1, unsigned char subnetMaskQuartet_2, unsigned char subnetMaskQuartet_3, unsigned char subnetMaskQuartet_4);
        IpV4Address(struct in_addr ipAddress, struct in_addr subnetMask);

        std::string getIpAddressString();
        std::string getSubnetMaskString();
        std::string getIpAddressCidr();
        unsigned long getIpAddress();
        unsigned long getSubnetMask();
        bool isLoopback();

        static std::vector<IpV4Address> getAllIpV4Addresses();

    protected:
        union ipAddress ipV4Address;
        union ipAddress subnetMask;
        /*
        bool isMulticast;
        bool isLoopback;
        bool isLinkLocal;
        */
};