// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2011-2012 Jassycoin Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_NETBASE_H
#define BITCOIN_NETBASE_H

#include <string>
#include <vector>

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <ifaddrs.h>
#endif
#ifdef BSD
#include <netinet/in.h>
#endif

#include "serialize.h"
#include "compat.h"

extern int nConnectTimeout;


// IP address (IPv6, or IPv4 using mapped IPv6 range (::FFFF:0:0/96))
class CNetAddr
{
    protected:
        unsigned char ip[16]; // in network byte order

    public:
        CNetAddr();
        CNetAddr(const struct in_addr& ipv4Addr);
        explicit CNetAddr(const char *pszIp, bool fAllowLookup = false);
        explicit CNetAddr(const std::string &strIp, bool fAllowLookup = false);
        void Init();
        void SetIP(const CNetAddr& ip);
        bool IsIPv4() const;    // IPv4 mapped address (::FFFF:0:0/96, 0.0.0.0/0)
        bool IsRFC1918() const; // IPv4 private networks (10.0.0.0/8, 192.168.0.0/16, 172.16.0.0/12)
        bool IsRFC3849() const; // IPv6 documentation address (2001:0DB8::/32)
        bool IsRFC3927() const; // IPv4 autoconfig (169.254.0.0/16)
        bool IsRFC3964() const; // IPv6 6to4 tunneling (2002::/16)
        bool IsRFC4193() const; // IPv6 unique local (FC00::/15)
        bool IsRFC4380() const; // IPv6 Teredo tunneling (2001::/32)
        bool IsRFC4843() const; // IPv6 ORCHID (2001:10::/28)
        bool IsRFC4862() const; // IPv6 autoconfig (FE80::/64)
        bool IsRFC6052() const; // IPv6 well-known prefix (64:FF9B::/96)
        bool IsRFC6145() const; // IPv6 IPv4-translated address (::FFFF:0:0:0/96)
        bool IsLocal() const;
        bool IsRoutable() const;
        bool IsValid() const;
        bool IsMulticast() const;
        std::string ToString() const;
        std::string ToStringIP() const;
        int GetByte(int n) const;
        int64 GetHash() const;
        bool GetInAddr(struct in_addr* pipv4Addr) const;
        std::vector<unsigned char> GetGroup() const;
        void print() const;

#ifdef USE_IPV6
        CNetAddr(const struct in6_addr& pipv6Addr);
        bool GetIn6Addr(struct in6_addr* pipv6Addr) const;
#endif

        friend bool operator==(const CNetAddr& a, const CNetAddr& b);
        friend bool operator!=(const CNetAddr& a, const CNetAddr& b);
        friend bool operator<(const CNetAddr& a, const CNetAddr& b);

        IMPLEMENT_SERIALIZE
            (
             READWRITE(FLATDATA(ip));
            )
};

class CService : public CNetAddr
{
    protected:
        unsigned short port; // host order

    public:
        CService();
        CService(const CNetAddr& ip, unsigned short port);
        CService(const struct in_addr& ipv4Addr, unsigned short port);
        CService(const struct sockaddr_in& addr);
        explicit CService(const char *pszIpPort, int portDefault, bool fAllowLookup = false);
        explicit CService(const char *pszIpPort, bool fAllowLookup = false);
        explicit CService(const std::string& strIpPort, int portDefault, bool fAllowLookup = false);
        explicit CService(const std::string& strIpPort, bool fAllowLookup = false);
        void Init();
        void SetPort(unsigned short portIn);
        unsigned short GetPort() const;
        bool GetSockAddr(struct sockaddr_in* paddr) const;
        friend bool operator==(const CService& a, const CService& b);
        friend bool operator!=(const CService& a, const CService& b);
        friend bool operator<(const CService& a, const CService& b);
        std::vector<unsigned char> GetKey() const;
        std::string ToString() const;
        std::string ToStringPort() const;
        std::string ToStringIPPort() const;
        void print() const;

#ifdef USE_IPV6
        CService(const struct in6_addr& ipv6Addr, unsigned short port);
        bool GetSockAddr6(struct sockaddr_in6* paddr) const;
        CService(const struct sockaddr_in6& addr);
#endif

        IMPLEMENT_SERIALIZE
            (
             CService* pthis = const_cast<CService*>(this);
             READWRITE(FLATDATA(ip));
             unsigned short portN = htons(port);
             READWRITE(portN);
             if (fRead)
                 pthis->port = ntohs(portN);
            )
};

bool LookupHost(const char *pszName, std::vector<CNetAddr>& vIP, int nMaxSolutions = 0, bool fAllowLookup = true);
bool LookupHostNumeric(const char *pszName, std::vector<CNetAddr>& vIP, int nMaxSolutions = 0);
bool Lookup(const char *pszName, CService& addr, int portDefault = 0, bool fAllowLookup = true);
bool Lookup(const char *pszName, std::vector<CService>& vAddr, int portDefault = 0, bool fAllowLookup = true, int nMaxSolutions = 0);
bool LookupNumeric(const char *pszName, CService& addr, int portDefault = 0);
bool ConnectSocket(const CService &addr, SOCKET& hSocketRet, int nTimeout = nConnectTimeout);

// Settings
extern int fUseProxy;
extern CService addrProxy;

#endif
