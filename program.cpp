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
#include "NetworkInterface.hpp"

int main() {
    std::vector<IpV4Address> ipAddresses = IpV4Address::getAllIpV4Addresses();
    for (IpV4Address ipAddress: ipAddresses) {
        std::cout << ipAddress.getIpAddressCidr() << std::endl;
    }
    std::vector<NetworkInterface> networkInterfaces = NetworkInterface::getAllNetworkInterfaces();
    for (NetworkInterface networkInterface: networkInterfaces) {
        std::cout << networkInterface.toString() << std::endl;
    }
    return 0;
}