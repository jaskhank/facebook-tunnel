/*
 *  Hans - IP over ICMP
 *  Copyright (C) 2009 Friedrich Schöller <hans@schoeller.se>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tun.h"

#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
//#define __FAVOR_BSD
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

using namespace std;

Tun::Tun(const char *device, int mtu)
{
    this->mtu = mtu;

    if (device != NULL)
    {
        strncpy(this->device, device, VTUN_DEV_LEN);
        this->device[VTUN_DEV_LEN] = 0;
    }
    else
        this->device[0] = 0;

    fd = tun_open(this->device);
    if (fd == -1) {
        // couldnt create tunnel device
    };

    char cmdline[512];
    snprintf(cmdline, sizeof(cmdline), "/sbin/ifconfig %s mtu %u", this->device, mtu);
    if (system(cmdline) != 0)
        syslog(LOG_ERR, "could not set tun device mtu");
}

Tun::~Tun()
{
    tun_close(fd, device);
}

/*string Tun::formatIp(uint32_t ip) {
    char buffer[16];
    sprintf(buffer, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    return buffer;
}

void Tun::setIp(uint32_t ip, uint32_t destIp, bool includeSubnet)
{
    char cmdline[512];
    string ips = formatIp(ip);
    string destIps = formatIp(destIp);

#ifdef LINUX
    snprintf(cmdline, sizeof(cmdline), "/sbin/ifconfig %s %s netmask 255.255.255.0", device, ips.c_str());
#else
    snprintf(cmdline, sizeof(cmdline), "/sbin/ifconfig %s %s %s netmask 255.255.255.255", device, ips.c_str(), destIps.c_str());
#endif

//    system("route add 176.9.168.100 gw 10.1.1.1");

    if (system(cmdline) != 0)
        syslog(LOG_ERR, "could not set tun device ip address");

#ifndef LINUX
    if (includeSubnet)
    {
        snprintf(cmdline, sizeof(cmdline), "/sbin/route add %s/24 %s", destIps.c_str(), destIps.c_str());
        if (system(cmdline) != 0)
            syslog(LOG_ERR, "could not add route");
    }
#endif
}

void Tun::write(const char *buffer, int length) {

  IpHeader *header = (IpHeader *)buffer;

  if( header->ip_p == IPPROTO_TCP ) {

  // sizeof( struct tcphdr ) // tcp_header->doff*4 (si el offset es 8 la cabecera + info esta en 32

  //  printf("Paquete:\t%s -> %s\n", dest_ip.c_str(), source_ip.c_str() );
  //  printf("Puertos:\t%u <-> %u\n\n", htons( tcpheader->dest ), htons( tcpheader->source ));

    facebook->send_packet( buffer, length );

  };

}

int Tun::read(char *buffer)
{
    int length = tun_read(fd, buffer, mtu);
    if (length == -1)
        syslog(LOG_ERR, "error reading from tun: %s", strerror(errno));
    return length;
}

int Tun::read(char *buffer, uint32_t &sourceIp, uint32_t &destIp)
{
    int length = read(buffer);

    IpHeader *header = (IpHeader *)buffer;
    sourceIp = ntohl(header->ip_src.s_addr);
    destIp = ntohl(header->ip_dst.s_addr);

    return length;
}
*/