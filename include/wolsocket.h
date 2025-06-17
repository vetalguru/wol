#ifndef WOLSOCKET_H
#define WOLSOCKET_H

#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

class Socket {
public:
    static constexpr int SOCKET_ERROR = -1;

    explicit Socket(int aDomain, int aType, int aProtocol) {
        m_fd = ::socket(aDomain, aType, aProtocol);
        if (m_fd == SOCKET_ERROR) {
            throw std::runtime_error("socket creation failed");
        }
    }

    Socket(Socket&& aOther) noexcept
        : m_fd(aOther.m_fd) {
        aOther.m_fd = SOCKET_ERROR;
    }

    ~Socket() {
        closeSocket(m_fd);
    }

    int get() const { return m_fd; }

    Socket& operator = (Socket&& aOther) noexcept {
        if (this != &aOther) {
            closeSocket(m_fd);
            m_fd = std::move(aOther.m_fd);
            aOther.m_fd = SOCKET_ERROR;
        }
        return *this;
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

private:
    void closeSocket(int& aFd) {
        if (aFd != SOCKET_ERROR) {
            ::close(aFd);
            aFd = SOCKET_ERROR;
        }
    }

    int m_fd = SOCKET_ERROR;
};

#endif  // WOLSOCKET_H
