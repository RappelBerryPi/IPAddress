#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#else

/*
#include <stdio.h>
#include <ifaddrs.h>
*/

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>

#endif

#include <iostream>
#include <string>
#include "IP.hpp"


//TODO: finish TODOs in code...

//TODO: extend to get the interface information which we're kind of already doing below, but add in generic cross/system
//information like name and link local vs whatever.

IpV4Address::IpV4Address() : IpV4Address(0L,0L) { }
IpV4Address::IpV4Address(std::string ipAddress) {
    //TODO
}

IpV4Address::IpV4Address(unsigned long ipAddress, unsigned long subnetMask) {
    this->ipV4Address.ipAddress =  htonl(ipAddress);
    this->subnetMask.ipAddress = htonl(subnetMask);
    //TODO: add validation
}

IpV4Address::IpV4Address(unsigned char quartet_1, unsigned char quartet_2, unsigned char quartet_3, unsigned char quartet_4, unsigned char subnetMaskQuartet_1, unsigned char subnetMaskQuartet_2, unsigned char subnetMaskQuartet_3, unsigned char subnetMaskQuartet_4) {
    this->ipV4Address.quartets[0] = quartet_1;
    this->ipV4Address.quartets[1] = quartet_2;
    this->ipV4Address.quartets[2] = quartet_3;
    this->ipV4Address.quartets[3] = quartet_4;

    this->subnetMask.quartets[0] = subnetMaskQuartet_1;
    this->subnetMask.quartets[1] = subnetMaskQuartet_2;
    this->subnetMask.quartets[2] = subnetMaskQuartet_3;
    this->subnetMask.quartets[3] = subnetMaskQuartet_4;
    //TODO: add validation
}

IpV4Address::IpV4Address(struct in_addr ipAddress, struct in_addr subnetMask) {
#ifdef _WIN32

    this->ipV4Address.ipAddress = ipAddress.S_un.S_addr;
    this->subnetMask.ipAddress = subnetMask.S_un.S_addr;

#else

    this->ipV4Address.ipAddress = ipAddress.s_addr;
    this->subnetMask.ipAddress =  subnetMask.s_addr;

#endif
    //TODO: add validation
}

std::string IpV4Address::getIpAddressString() {
    char buf1[16];
    snprintf(buf1, 16, "%03d.%03d.%03d.%03d", ipV4Address.quartets[0], ipV4Address.quartets[1], ipV4Address.quartets[2], ipV4Address.quartets[3]);
    std::string ipAddress(buf1);
    return ipAddress;
}

std::string IpV4Address::getSubnetMaskString() {
    char buf1[16];
    snprintf(buf1, 16, "%03d.%03d.%03d.%03d", subnetMask.quartets[0], subnetMask.quartets[1], subnetMask.quartets[2], subnetMask.quartets[3]);
    std::string ipAddress(buf1);
    return ipAddress;
}

unsigned long IpV4Address::getIpAddress() {
    return ntohl(ipV4Address.ipAddress);
}

unsigned long IpV4Address::getSubnetMask() {
    return ntohl(subnetMask.ipAddress);
}

std::string IpV4Address::getIpAddressCidr() {
    unsigned long subnetMask = this->getSubnetMask();
    subnetMask = ~subnetMask & (subnetMask - 1);
    int CIDR = 32;
    while (subnetMask != 0) {
        CIDR--;
        subnetMask = subnetMask >> 1;
    }
    std::string ipAddressString = this->getIpAddressString();
    ipAddressString.append("/");
    ipAddressString.append(std::to_string(CIDR));
    return ipAddressString;
}

bool IpV4Address::isLoopback() {
    return this->getIpAddress() == 2130706433;
}

std::vector<IpV4Address> IpV4Address::getAllIpV4Addresses() {
    std::vector<IpV4Address> returnVector(0);

#ifdef _WIN32
    PMIB_IPADDRTABLE pIPAddrTable = (MIB_IPADDRTABLE *) malloc(sizeof(MIB_IPADDRTABLE));
    unsigned long pdwSize;
    bool   bOrder = true;
    IN_ADDR ipAddr;
    unsigned long A = GetIpAddrTable(pIPAddrTable, &pdwSize, bOrder);
    if (pIPAddrTable) {
        if ((A = GetIpAddrTable(pIPAddrTable, &pdwSize, bOrder)) != 0) {
            std::cerr << "Error getting IP Address info. Error: " << A << std::endl;
            return returnVector;
        }
        for (int i = 0; i < (int) pIPAddrTable->dwNumEntries; i++) {
            IpV4Address ipV4Address(ntohl(pIPAddrTable->table[i].dwAddr), ntohl(pIPAddrTable->table[i].dwMask));
            returnVector.push_back(ipV4Address);
        }
        if (pIPAddrTable) {
            free(pIPAddrTable);
            pIPAddrTable = nullptr;
        }
    }
#else
    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1) {
        std::cerr << "Error getting interfaces" << std::endl;
        exit(EXIT_FAILURE);
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sockAddrIn = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr);
            struct sockaddr_in *NetMaskIn  = reinterpret_cast<sockaddr_in*>(ifa->ifa_netmask);
            IpV4Address ipV4Address(sockAddrIn->sin_addr, NetMaskIn->sin_addr);
            returnVector.push_back(ipV4Address);
        }
    }
    freeifaddrs(ifaddr);

#endif

    return returnVector;

}

int main() {
    /*
    //IpV4Address ipV4Address(3232235521L,4294967040L);
    IpV4Address ipV4Address(192,168,0,1,255,255,255,0);
    std::cout << ipV4Address.getIpAddressString() << std::endl;
    std::cout << ipV4Address.getSubnetMaskString() << std::endl;
    std::cout << ipV4Address.getIpAddressCidr() << std::endl;
    std::cout << ipV4Address.getIpAddress() << std::endl;
    std::cout << ipV4Address.getSubnetMask() << std::endl;
    */

    std::vector<IpV4Address> ipAddresses = IpV4Address::getAllIpV4Addresses();
    for (IpV4Address ipAddress: ipAddresses) {
        std::cout << ipAddress.getIpAddressCidr() << std::endl;
    }
    return 0;

    /*
    PMIB_IPADDRTABLE pIPAddrTable = (MIB_IPADDRTABLE *) malloc(sizeof(MIB_IPADDRTABLE));
    unsigned long pdwSize;
    bool   bOrder = true;
    IN_ADDR ipAddr;
    unsigned long A = GetIpAddrTable(pIPAddrTable, &pdwSize, bOrder);
    if (pIPAddrTable) {
        if ((A = GetIpAddrTable(pIPAddrTable, &pdwSize, bOrder)) != 0) {
            std::cerr << "Error getting IP Address info. Error: " << A << std::endl;
            return -1;
        }
        std::cout << "Number of Entries:"  << pIPAddrTable->dwNumEntries << std::endl;
        for (int i = 0; i < (int) pIPAddrTable->dwNumEntries; i++) {

            ipAddr.S_un.S_addr = (unsigned long long) pIPAddrTable->table[i].dwAddr;
            std::string ipAddress = inet_ntoa(ipAddr);

            ipAddr.S_un.S_addr = (unsigned long long) pIPAddrTable->table[i].dwMask;
            std::string subnetMask = inet_ntoa(ipAddr);

            ipAddr.S_un.S_addr = (unsigned long long) pIPAddrTable->table[i].dwBCastAddr;
            std::string broadcastAddress = inet_ntoa(ipAddr);

            unsigned short wType = pIPAddrTable->table[i].wType;

            std::cout << "Interface index[" << i << "]: " << pIPAddrTable->table[i].dwIndex << std::endl;

            std::cout << "\t" << "IP Address:      " << ipAddress << std::endl;
            std::cout << "\t" << "Subnet Mask:     " << subnetMask << std::endl;
            std::cout << "\t" << "BrodcastAddress: " << broadcastAddress << std::endl;
            std::cout << "\t" << "Type and State:  " << std::endl;
            if (wType & MIB_IPADDR_PRIMARY) {
                std::cout << "\t\t" << "Primary IP Address" << std::endl;
            }
            if (wType & MIB_IPADDR_DYNAMIC) {
                std::cout << "\t\t" << "Dynamic IP Address" << std::endl;
            }
            if (wType & MIB_IPADDR_DISCONNECTED) {
                std::cout << "\t\t" << "Address is disconnected" << std::endl;
            }
            if (wType & MIB_IPADDR_DELETED) {
                std::cout << "\t\t" << "Address is being deleted" << std::endl;
            }
            if (wType & MIB_IPADDR_TRANSIENT) {
                std::cout << "\t\t" << "Transient Address" << std::endl;
            }
            std::cout << std::endl;
        }
        if (pIPAddrTable) {
            free(pIPAddrTable);
            pIPAddrTable = nullptr;
        }
    }
    */

}