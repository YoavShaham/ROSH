#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
#include "../../memory/heap.h"
#include "../../../libc/memory.h"
#include "../../../libc/string.h"

#define DHCP_MAGIC_COOKIE 0x63538263
#define DHCP_XID 0x26F30339

// dhcp message type
#define DHCP_DISCOVER 0X1
#define DHCP_OFFER 0X2
#define DHCP_REQUEST 0X3
#define DHCP_ACK 0X5

// dhcp options 
#define DHCP_PACKET_TYPE_OPTION 0X35
#define DHCP_REQUESTED_IP_ADDRESS 0X32
#define DHCP_PARAMETER_REQUEST_LIST 0x37
#define DHCP_SERVER_IP 0X36

typedef struct dhcp_packet{
    // identifing flags of the current dhcp message
    uint8_t op_flag;
    uint8_t htype_flag;
    uint8_t hlen_flag;
    uint8_t hops_flags;

    // constanst value
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;

    uint32_t ciaddr;    // client ip address
    uint32_t yiaddr;    // your ip address
    uint32_t siaddr;    // server ip address
    uint32_t giaddr;    // gatway ip address
    uint32_t chaddr[4]; // client hardwre address
    uint8_t always_0[192];
    uint32_t magic_cookie;  // token for the dhcp packet
    uint8_t options[60];
}__attribute__((packed)) dhcp_packet;

void parse_dhcp(dhcp_packet* packet);

/*
    This function sends an dhcp discovery messages for dhcp servers in lan
*/
void dhcp_discover();

/*
    This function recives dhcp offer packet parses it and sends dhcp request
    @param recived packet: the dhcp offer packet
*/
void dhcp_request(dhcp_packet* recived_packet);

/*
	print net info
*/
void print_net_info();

#endif