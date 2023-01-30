#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <inaddr.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>
#include <sstream>
#include "IP.hpp"
#include "NetworkInterface.hpp"

NetworkInterface::NetworkInterface() {
    this->MTU = 0;
    this->physicalAddress = "";
    this->name = "";
    this->description = "";
    this->linkSpeed = 0;
    this->ipAddress = IpV4Address();
    this->interfaceType = OTHER;
}

NetworkInterface::NetworkInterface(MIB_IFROW *Interface) {
    this->MTU = Interface->dwMtu;

    std::stringstream physicalAddressStream;
    for (int i = 0; i < Interface->dwPhysAddrLen; i++) {
        char buf[3];
        snprintf(buf, 3, "%.2X", Interface->bPhysAddr[i]);
        physicalAddressStream << buf;
        if (i < Interface->dwPhysAddrLen - 1) {
            physicalAddressStream << "-";
        }
    }
    this->physicalAddress = physicalAddressStream.str();
    std::wstring WideString(Interface->wszName);
    this->description = std::string(WideString.begin(), WideString.end());
    this->name = std::string(Interface->bDescr, Interface->bDescr + 256);
    this->linkSpeed = Interface->dwSpeed;
    this->ipAddress = IpV4Address();
    this->interfaceType = static_cast<InterfaceType>(Interface->dwType);
    this->InterfaceIndex = Interface->dwIndex;
}

NetworkInterface::NetworkInterface(MIB_IFROW *Interface, IpV4Address ipAddress) : NetworkInterface(Interface) {
    this->ipAddress = ipAddress;
}

void NetworkInterface::setIpAddress(IpV4Address ipAddress) {
    this->ipAddress = ipAddress;
}

std::vector<NetworkInterface> NetworkInterface::getAllNetworkInterfaces() {
    std::vector<NetworkInterface> NetworkInterfaces(0);

    unsigned long dwSize = 0;
    long returnValue = 0;

    unsigned int i, j;

    /* variables used for GetIfTable and GetIfEntry */
    MIB_IFTABLE *InterfaceTable;
    MIB_IFROW *Interface;

    // Allocate memory for our pointers.
    std::vector<IpV4Address> ipV4Addresses = IpV4Address::getAllIpV4Addresses();


    InterfaceTable = (MIB_IFTABLE *) malloc(sizeof (MIB_IFTABLE));
    if (InterfaceTable == NULL) {
        std::cerr << "Error allocating memory needed to call GetIfTable" << std::endl;
        return NetworkInterfaces;
    }
    // Make an initial call to GetIfTable to get the
    // necessary size into dwSize
    dwSize = sizeof (MIB_IFTABLE);
    if (GetIfTable(InterfaceTable, &dwSize, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
        free(InterfaceTable);
        InterfaceTable = (MIB_IFTABLE *) malloc(dwSize);
        if (InterfaceTable == NULL) {
            std::cerr << "Error allocating memory needed to call GetIfTable" << std::endl;
            return NetworkInterfaces;
        }
    }
    // Make a second call to GetIfTable to get the actual
    // data we want.
    if ((returnValue = GetIfTable(InterfaceTable, &dwSize, FALSE)) == NO_ERROR) {
        for (i = 0; i < InterfaceTable->dwNumEntries; i++) {
            MIB_IFROW *InterfaceRow = (MIB_IFROW *) &InterfaceTable->table[i];
            NetworkInterface networkInterface(InterfaceRow);
            for (int j = 0; j < ipV4Addresses.size(); j++) {
                if (InterfaceRow->dwIndex == ipV4Addresses[j].deviceIndex) {
                    networkInterface.setIpAddress(ipV4Addresses[j]);
                }
            }
            NetworkInterfaces.push_back(networkInterface);
        }
    } else {
        std::cerr << "GetIfTable failed with error: " << returnValue << std::endl;
        if (InterfaceTable != NULL) {
            free(InterfaceTable);
            InterfaceTable = NULL;
        }  
        return NetworkInterfaces;
    }
    return NetworkInterfaces;
}

std::string NetworkInterface::convertInterfaceTypeToString(InterfaceType interfaceType) {
    switch (interfaceType) {
        case ETHERNET:
            return std::string("Ethernet");
            break;
        case TOKEN_RING:
            return std::string("Token Ring");
            break;
        case POINT_TO_POINT:
            return std::string("Point to Point");
            break;
        case SOFTWARE_LOOPBACK:
            return std::string("Loopback");
            break;
        case ATM:
            return std::string("ATM");
            break;
        case WIRELESS_802_11:
            return std::string("Wi-Fi");
            break;
        case TUNNEL:
            return std::string("Tunnel");
            break;
        case FIREWIRE:
            return std::string("Firewire");
            break;
        case OTHER:
            return std::string("Other");
            break;
        default:
            return std::string("Other");
            break;
    }

}

std::string NetworkInterface::toString() {
    std::string returnString = this->name;
    returnString.append("\n\tPhysical Address: ");
    returnString.append(this->physicalAddress);
    returnString.append("\n\tDescription:      ");
    returnString.append(this->description);
    returnString.append("\n\tMTU:              ");
    returnString.append(std::to_string(this->MTU));
    returnString.append("\n\tLink Speed:       ");
    returnString.append(formatLinkSpeed(this->linkSpeed));
    returnString.append("\n\tInterface Type:   ");
    returnString.append(this->convertInterfaceTypeToString(this->interfaceType));
    returnString.append("\n\tIPAddress:        ");
    returnString.append(this->ipAddress.getIpAddressCidr());
    returnString.append("\n");
    return returnString;
}


std::string formatLinkSpeed (unsigned long long linkSpeed) {
    std::string suffix = "b";
    if (linkSpeed >= 1000) {
        linkSpeed /= 1000;
        suffix = "Kb";
    }
    if (linkSpeed >= 1000) {
        linkSpeed /= 1000;
        suffix = "Mb";
    }
    if (linkSpeed >= 1000) {
        linkSpeed /= 1000;
        suffix = "Gb";
    }
    if (linkSpeed >= 1000) {
        linkSpeed /= 1000;
        suffix = "Tb";
    }
    std::string returnString;
    returnString.append(std::to_string(linkSpeed));
    returnString.append(suffix);
    return returnString;
}