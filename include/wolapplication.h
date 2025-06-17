#ifndef WOLAPPLICATION_H
#define WOLAPPLICATION_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class WolApplication
{
    public:
        WolApplication(int argc, const char* const argv[]);
        ~WolApplication() = default;

        int run() noexcept;

        WolApplication(const WolApplication&) = delete;
        WolApplication(WolApplication&&) = delete;

        WolApplication& operator = (const WolApplication&) = delete;
        WolApplication& operator = (WolApplication&&) = delete;

    private:
        static constexpr size_t kMacStrLength = 17;
        static constexpr size_t kMacBytesLength = 6;
        static constexpr size_t kMacMagicPackageLength = 102;
        static constexpr uint16_t kDefaultWolPort = 9;

        using mac_t = std::array<char, kMacBytesLength>;
        using magic_t = std::array<char, kMacMagicPackageLength>;

        bool parseParams(int argc, const char* const argv[]) noexcept;
        bool strToMac(const std::string& macStr, mac_t& mac) noexcept;
        magic_t genMagicPackage(const mac_t& mac) noexcept;
        bool sendMagicPackage(const mac_t& mac,
                              const magic_t& magicPackage,
                              const uint16_t port = kDefaultWolPort) noexcept;

        bool isMacStrValid(const std::string& macStr) noexcept;
        bool isDelimiter(char c) const noexcept;

        void printHelpMessage() const noexcept;

    private:
        std::vector<std::string> m_params;

        bool m_isParamsValid{false};
};

#endif // WOLAPPLICATION_H
