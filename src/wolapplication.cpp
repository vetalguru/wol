#include "wolapplication.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "wolsocket.h"

WolApplication::WolApplication(int argc, const char * const argv[]) {
    m_isParamsValid = parseParams(argc, argv);
}

int WolApplication::run() noexcept {
    mac_t mac{};
    if (!m_isParamsValid || !strToMac(m_params[1], mac)) {
        std::cerr << "Error: Wrong parameters\n" << std::endl;
        printHelpMessage();
        return EXIT_FAILURE;
    }

    std::cout << "Sending a magic package to the address : " << m_params[1] << std::endl;

    return (!sendMagicPackage(mac, genMagicPackage(mac)));
}

bool WolApplication::parseParams(int argc, const char* const argv[]) noexcept {
    m_params.clear();

    for (int i = 0; i < argc; ++i) {
        m_params.emplace_back(argv[i]);
    }

    return (m_params.size() == 2);
}

bool WolApplication::isMacStrValid(const std::string& macStr) noexcept {
    bool isValid = (macStr.length() == kMacStrLength);
    for (int i = 0; isValid && i < kMacStrLength; ++i) {
        isValid = (i % 3 != 2) ? std::isxdigit(macStr[i]) : isDelimiter(macStr[i]);
    }

    return isValid;
}

bool WolApplication::strToMac(const std::string& macStr, mac_t& mac) noexcept {
    std::fill(mac.begin(), mac.end(), 0);
    bool result{true};

    if (isMacStrValid(macStr)) {
        constexpr int aHex{16}; // basis
        const char* ptr = macStr.c_str();
        char* endPtr = nullptr;
        for (char i = 0; i < kMacBytesLength; ++i) {
            mac[i] = static_cast<char>(std::strtol(ptr, &endPtr, aHex));

            // Check end of line
            if (*endPtr == '\0') {
                break;
            }

            ptr = ++endPtr;
        }
    } else {
        result = false;
    }

    return result;
}

WolApplication::magic_t WolApplication::genMagicPackage(const mac_t& mac) noexcept {
    magic_t magicPackage;

    // Set the first 6 bytes to 0xFF
    std::fill_n(magicPackage.begin(), kMacBytesLength, 0xFF);

    // Copy the MAC address 16 times
    for (int i = 1; i <= 16; ++i) {
        std::copy(mac.begin(), mac.end(), magicPackage.begin() + i * kMacBytesLength);
    }

    return magicPackage;
}

bool WolApplication::sendMagicPackage(const mac_t& mac,
                                      const magic_t& magicPackage,
                                      const uint16_t port) noexcept {

    Socket s(AF_INET, SOCK_DGRAM, 0);
    if (s.get() == Socket::SOCKET_ERROR) {
        return false;
    }

    // Set the socket option for broadcast
    int broadcast{1};
    if (::setsockopt(s.get(), SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        return false;
    }

    // Initialize server address
    sockaddr_in hostAddr{};
    hostAddr.sin_port = htons(port);
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_addr.s_addr = INADDR_BROADCAST;

    // Send the magic package
    ssize_t sentBytes = ::sendto(s.get(), &magicPackage[0], magicPackage.size(), 0,
                               reinterpret_cast<struct sockaddr*>(&hostAddr), sizeof(hostAddr));

    // Check if the package was sent successfully
    return (sentBytes == static_cast<ssize_t>(magicPackage.size()));
}

void WolApplication::printHelpMessage() const noexcept {
    std::cout << "Usage: wol <MAC_ADDRESS>\n\n"
              << "Wake-on-LAN (WOL) utility to send wake-up signals to a remote computer.\n\n"
              << "Examples:\n"
              << "  wol 00:1A:2B:3C:4D:5E   # Send a Wake-on-LAN signal to the default broadcast address (delimiter :).\n"
              << "  wol 00-1A-2B-3C-4D-5E   # Send a Wake-on-LAN signal to the default broadcast address (delimiter -).\n"
              << std::endl;
}

bool WolApplication::isDelimiter(char c) const noexcept {
    const std::array<char, 2> kDelimiters{':', '-'};
    return std::any_of(kDelimiters.begin(), kDelimiters.end(), [c](char b) { return c == b; });
}
