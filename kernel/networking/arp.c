#include "arp.h"

device_address g_address_cache[ARP_CACHE_LEN];

extern uint8_t g_src_mac[6];
extern uint32_t g_self_ip;

void parse_arp_packet(arp_packet* packet, uint32_t packet_len);
int find_arp_device(device_address* device);
void create_and_send_arp(uint32_t src_ip, uint32_t dest_ip, uint8_t src_mac[6], uint8_t dst_mac[6], uint16_t opcode);

void parse_arp_packet(arp_packet* packet, uint32_t packet_len)
{
    //  assigning the computers addresses
    uint32_t src_ip = packet->srcpr;
    uint32_t dst_ip = packet->dstpr;
    uint8_t src_mac[6] = {0};
    uint8_t dst_mac[6] = {0};

    // setting mac addresses
    memcpy(src_mac, packet->srchw, sizeof(uint8_t[6]));
    memcpy(dst_mac, packet->dsthw, sizeof(uint8_t[6]));

    if(packet->opcode == OPERATION_ARP_REQUEST)
    {
        
        device_address* request_device = (device_address*)kmalloc(sizeof(device_address));
        for(int i = 0; i < ARP_CACHE_LEN; i++)
        {   
            // if requested device found
            if(g_address_cache[i].ip_address == dst_ip)
            {
                request_device->ip_address = dst_ip;
                memcpy(request_device->mac_address, g_address_cache[i].mac_address, sizeof(uint8_t[6]));
                break;
            }
        } 

        if(request_device->ip_address != NULL)
        {
            
        }
    }

    // if arp replay to our previous arp request 
    else if(packet->opcode == OPERATION_ARP_REPLAY)
    {   
        // going through the arp cache
        for(int i = 0; i < ARP_CACHE_LEN; i++)
        {  
            // if saving the device address
            if(strncmp((char*)&(g_address_cache[i]), (char*)kmalloc(sizeof(device_address)), sizeof(device_address)) == 0)
            {
                g_address_cache[i].ip_address = packet->srcpr;
                memcpy(g_address_cache[i].mac_address, packet->srchw, sizeof(uint8_t[6]));
                break;
            }
        }
    }
}

int find_arp_device(device_address* device)
{
    for(int i = 0; i < ARP_CACHE_LEN; i++)
    {   
        // if device found
        if(strncmp((char*)&(g_address_cache[i]), (char*)device, sizeof(device_address)))
        {
            return i;
        }
    }

    // device wasn't found
    return -1;
}

void create_and_send_arp(uint32_t src_ip, uint32_t dest_ip, uint8_t src_mac[6], uint8_t dst_mac[6], uint16_t opcode)
{
    arp_packet* packet = (arp_packet*)kmalloc(sizeof(arp_packet));

    // setting ethernet and ip
    packet->hlen = ETHERNET_HLEN;
    packet->plen = IPV4_PLEN;

    // setting ethernet hardware type
    packet->htype = 1;

    // setting ipv4 protocol
    packet->ptype = 4;

    // setting arp opcode
    packet->opcode = opcode;

    // setting mac addresses
    memcpy(packet->srchw, src_mac, 6);
    memcpy(packet->dsthw, dst_mac, 6); 

    // setting ip addresses
    packet->srcpr = src_ip;
    packet->dstpr = dest_ip;

    // sending arp packet
    send_ethernet_packet((uint8_t*)packet, sizeof(arp_packet), HEADER_TYPE_ARP, dst_mac);
}