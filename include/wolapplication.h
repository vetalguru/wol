#ifndef WOLAPPLICATION_H
#define WOLAPPLICATION_H

#include <array>
#include <string>
#include <vector>

class WolApplication
{
    public:
        WolApplication(const int argc, const char* const argv[]);
        WolApplication(const WolApplication&) = delete;
        WolApplication(WolApplication&&) = delete;
        ~WolApplication();

        int run() noexcept;

        WolApplication& operator = (const WolApplication&) = delete;
        WolApplication& operator = (WolApplication&&) = delete;

    private:
        static constexpr size_t MAC_STR_LENGTH{17};
        static constexpr size_t MAC_BYTES_LENGTH{6};
        static constexpr size_t MAC_MAGIC_PACKAGE_LENGTH{102};
        static constexpr int    DEFAULT_WOL_PORT{9};

    private:
        bool parseParams(const int argc, const char* const argv[]) noexcept;
        bool isMACAddressString(const std::string& macStr) noexcept;
        bool converMacStrToBytes(const std::string& macStr, std::array<char, MAC_BYTES_LENGTH>& mac) noexcept;
        void generateMagicPackage(const std::array<char, MAC_BYTES_LENGTH>& mac,
                                  std::array<char, MAC_MAGIC_PACKAGE_LENGTH>& magicPackage) noexcept;
        bool sendMagicPackage(const std::array<char, MAC_BYTES_LENGTH>& mac,
                              const std::array<char, MAC_MAGIC_PACKAGE_LENGTH>& magicPackage,
                              const int port = DEFAULT_WOL_PORT) noexcept;

        void printHelpMessage() const;

        bool isDelimiter(const char c) const noexcept;

    private:
        std::vector<std::string> m_params;

        bool m_isParamsValid{false};
};

#endif // WOLAPPLICATION_H
