#include "wolapplication.h"

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

WolApplication::WolApplication(const int argc, const char * const argv[]) {
    parseParams(argc, argv);
}

WolApplication::~WolApplication() {
}

int WolApplication::run() noexcept {
    std::array<char, MAC_BYTES_LENGTH> mac{};
    if (!validateParams(m_params) || !converMacStrToBytes(m_params[1], mac)) {
        std::cerr << "Error: Wrong parameters\n" << std::endl;
        printHelpMessage();
        return EXIT_FAILURE;
    }

    std::cout << "Sending a magic package to the address : " << m_params[1] << std::endl;

    std::array<char, MAC_MAGIC_PACKAGE_LENGTH> magicPackage{};
    generateMagicPackage(mac, magicPackage);

    return (!sendMagicPackage(mac, magicPackage, DEFAULT_WOL_PORT));
}

void WolApplication::parseParams(const int argc, const char* const argv[]) noexcept {
    m_params.clear();

    for (int i = 0; i < argc; ++i) {
        m_params.emplace_back(argv[i]);
    }
}

bool WolApplication::validateParams(const std::vector<std::string>& params) noexcept {
    return (params.size() == 2);
}

bool WolApplication::isMACAddressString(const std::string& macStr) noexcept {
    bool isValid = (macStr.length() == MAC_STR_LENGTH);

    for (int i = 0; isValid && i < MAC_STR_LENGTH; ++i) {
        isValid = (i % 3 != 2) ? std::isxdigit(macStr[i]) : isDelimiter(macStr[i]);
    }

    return isValid;
}

bool WolApplication::converMacStrToBytes(const std::string& macStr, std::array<char, MAC_BYTES_LENGTH>& mac) noexcept {
    std::fill(mac.begin(), mac.end(), 0);

    bool result{true};
    if (isMACAddressString(macStr)) {
        constexpr int HEX{16}; // basis
        const char* ptr = macStr.c_str();
        char* endPtr = nullptr;
        for (char i = 0; i < MAC_BYTES_LENGTH; ++i) {
            mac[i] = static_cast<char>(std::strtol(ptr, &endPtr, HEX));

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

void WolApplication::generateMagicPackage(const std::array<char, MAC_BYTES_LENGTH> &mac,
                                       std::array<char, MAC_MAGIC_PACKAGE_LENGTH>& magicPackage) noexcept {
    // Set the first 6 bytes to 0xFF
    std::fill_n(magicPackage.begin(), MAC_BYTES_LENGTH, 0xFF);

    // Copy the MAC address 16 times
    for (int i = 1; i <= 16; ++i) {
        std::copy(mac.begin(), mac.end(), magicPackage.begin() + i * MAC_BYTES_LENGTH);
    }
}

bool WolApplication::sendMagicPackage(const std::array<char, MAC_BYTES_LENGTH>& mac,
                                   const std::array<char, MAC_MAGIC_PACKAGE_LENGTH>& magicPackage,
                                   const int port) noexcept {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        return false;
    }

    // Set the socket option for broadcast
    int broadcast{1};
    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        close(s);
        return false;
    }

    // Initialize server address
    sockaddr_in hostAddr{};
    hostAddr.sin_port = htons(port);
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_addr.s_addr = INADDR_BROADCAST;

    // Send the magic package
    ssize_t sentBytes = sendto(s, &magicPackage[0], magicPackage.size(), 0,
                               reinterpret_cast<struct sockaddr*>(&hostAddr), sizeof(hostAddr));

    // Close the socket
    close(s);

    // Check if the package was sent successfully
    return (sentBytes == static_cast<ssize_t>(magicPackage.size()));;
}

void WolApplication::printHelpMessage() const {
    std::cout << "Usage: wol <MAC_ADDRESS>\n\n"
              << "Wake-on-LAN (WOL) utility to send wake-up signals to a remote computer.\n\n"
              << "Examples:\n"
              << "  wol 00:1A:2B:3C:4D:5E   # Send a Wake-on-LAN signal to the default broadcast address (delimiter :).\n"
              << "  wol 00-1A-2B-3C-4D-5E   # Send a Wake-on-LAN signal to the default broadcast address (delimiter -).\n"
              << std::endl;
}

bool WolApplication::isDelimiter(const char c) const noexcept {
    const std::array<char, 2> DELIMITERS{':', '-'};
    bool result{false};

    for (const auto &ch : DELIMITERS) {
        if (c == ch) {
            result = true;
            break;
        }
    }

    return result;
}
