#ifndef _NETWORKINTERFACE_HPP
#define _NETWORKINTERFACE_HPP

#ifdef _WIN32

#include <inaddr.h>
#include <ifmib.h>

#else

#include <netinet/in.h>

#endif

#include <string>
#include <vector>
#include "IP.hpp"

enum InterfaceType : unsigned long {
    ETHERNET = IF_TYPE_ETHERNET_CSMACD,
    TOKEN_RING = IF_TYPE_ISO88025_TOKENRING,
    POINT_TO_POINT = IF_TYPE_PPP,
    SOFTWARE_LOOPBACK = IF_TYPE_SOFTWARE_LOOPBACK,
    ATM = IF_TYPE_ATM,
    WIRELESS_802_11 = IF_TYPE_IEEE80211,
    TUNNEL = IF_TYPE_TUNNEL,
    FIREWIRE = IF_TYPE_IEEE1394,
    OTHER = IF_TYPE_OTHER
};

std::string formatLinkSpeed (unsigned long long linkSpeed);

class __declspec(dllexport) NetworkInterface {
    public:
        NetworkInterface();
        NetworkInterface(MIB_IFROW *Interface);
        NetworkInterface(MIB_IFROW *Interface, IpV4Address ipAddress);

        void setIpAddress(IpV4Address ipAddress);

        static std::vector<NetworkInterface> getAllNetworkInterfaces();
        std::string toString();

    protected:
        unsigned long long MTU;
        std::string physicalAddress;
        std::string name;
        std::string description;
        unsigned long long linkSpeed;
        IpV4Address ipAddress;
        InterfaceType interfaceType;
        unsigned long InterfaceIndex;
        bool isLinkLocal;

    private:
        std::string convertInterfaceTypeToString(InterfaceType InterfaceType);

};

#endif