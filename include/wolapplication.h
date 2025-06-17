#ifndef WOLAPPLICATION_H
#define WOLAPPLICATION_H

#include <array>
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
        static constexpr int    kDefaultWolPort = 9;

        using mac_t = std::array<char, kMacBytesLength>;

        bool parseParams(int argc, const char* const argv[]) noexcept;
        bool isMacStrValid(const std::string& macStr) noexcept;
        bool strToMac(const std::string& macStr, mac_t& mac) noexcept;
        void generateMagicPackage(const std::array<char, kMacBytesLength>& mac,
                                  std::array<char, kMacMagicPackageLength>& magicPackage) noexcept;
        bool sendMagicPackage(const std::array<char, kMacBytesLength>& mac,
                              const std::array<char, kMacMagicPackageLength>& magicPackage,
                              const int port = kDefaultWolPort) noexcept;

        void printHelpMessage() const noexcept;

        bool isDelimiter(char c) const noexcept;

    private:
        std::vector<std::string> m_params;

        bool m_isParamsValid{false};
};

#endif // WOLAPPLICATION_H
