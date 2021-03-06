#include "ethernet_driver.h"
#include "../protocols/ethernet.h"

ethernet_device* g_ethernet_device;
uint8_t g_ts_reg[] = {0x20, 0x24, 0x28, 0x2C};
uint8_t g_tc_reg[] = {0x10, 0x14, 0x18, 0x1C};
uint32_t g_curr_rx = 0; 
uint8_t g_src_mac[6] = {0};
uint8_t rx_buffer_copy[0x600];

void initialize_ethernet_driver();
void send_packet(void* content, uint32_t packet_len);
void read_mac_address();


void initialize_ethernet_driver()
{
    g_ethernet_device = (ethernet_device*)kmalloc(sizeof(ethernet_device));
    
    // getting ethernet device data
    g_ethernet_device->ethernet_device_data = get_ethernet_controller();

    // setting ethernet device io address
    g_ethernet_device->io_base = g_ethernet_device->ethernet_device_data->device_header->bars[0].value.io_address;

    uint32_t io_base = g_ethernet_device->io_base;

	// enable PCI bus mastering
	g_ethernet_device->ethernet_device_data->device_header->command |= 0b100;		// set bit
	uint32_t new_command = ((uint32_t)g_ethernet_device->ethernet_device_data->device_header->status) << 16;		// add status register
	new_command += g_ethernet_device->ethernet_device_data->device_header->command;		// add command register
	write_dword_to_pci(g_ethernet_device->ethernet_device_data->bus_num, g_ethernet_device->ethernet_device_data->device_num, 0, 4, new_command);

    // turning on the ethernet device
    outb(io_base + IO_CONFIG1_OFFSET, 0);

    // reseting ethernet device
    // outb(io_base + IO_CMD_OFFSET, 0x10);

    // waiting for reseting to finish
    // while(inb(io_base + IO_CMD_OFFSET) & 0x10 !=0){}

    //TODO 
    // make sure that setting the virt and not phys address is ok
    //TODO
    // allocating packet input buffer 
    uint32_t virtual_input_buffer = (uint32_t)NET_RX;
    
    // setting buffer for input packets
    outdw(io_base + IO_RBSTART_OFFSET, (uint32_t)virtual_input_buffer);
    g_ethernet_device->rx_buff = virtual_input_buffer;

    // setting flags for interrupts
    outw(io_base + IO_IMR_OFFSET, 0x0005);

    // configure recive buffer
    outdw(io_base + IO_RCR_CONFIGURE, 0x8F);

    g_ethernet_device->curr_reg = 0;

	// reading the mac address
    read_mac_address();

    // setting src mac address
    memcpy(g_src_mac, g_ethernet_device->mac_address, 6);

    // setting interrupt handler
    set_interrupt(g_ethernet_device->ethernet_device_data->device_header->interrupt_line + IRQ0, network_handler);

	// setting recive and transmits
    outb(io_base + IO_CMD_OFFSET, 0x0C);
}

void network_handler(registers_t* registers)
{
    uint16_t isr_value = inw(g_ethernet_device->io_base + IO_ISR_OFFSET);
    // if packet recived (ROK bit set)
    if(isr_value & 1)
    {
		uint16_t* temp = (uint16_t*)(g_ethernet_device->rx_buff + g_curr_rx);

        // getting the packet len
        uint16_t packet_length = *(temp + 1) - 4;

		temp += 2;

		memcpy(rx_buffer_copy, temp, packet_length);

		parse_ethernet_packet((ethernet_packet*)rx_buffer_copy, packet_length);

        g_curr_rx = (g_curr_rx + packet_length + 0xb) & (~3);
		g_curr_rx -= g_curr_rx > RX_BUFFER_SIZE ? RX_BUFFER_SIZE : 0;

        // setting the new rx buffer address
        outw(g_ethernet_device->io_base + IO_CAPR, g_curr_rx - 16);
    }
	outw(g_ethernet_device->io_base + IO_ISR_OFFSET, 0x5);
}

void send_packet(void* content, uint32_t packet_len)
{
    uint32_t transmit_buff = (uint32_t)kmalloc(packet_len);

    // making sure and allocating space for packet in kernel heap
    memcpy((void*)transmit_buff, content, packet_len);

    // sending packet data
    outdw((uint16_t)g_ethernet_device->io_base + g_ts_reg[g_ethernet_device->curr_reg], transmit_buff);
    
    // sending packet len
    outdw((uint16_t)g_ethernet_device->io_base + g_tc_reg[g_ethernet_device->curr_reg], packet_len);
    
    // only 4 elements in array
    g_ethernet_device->curr_reg = (g_ethernet_device->curr_reg + 1) % 4; 

	kfree((void*)transmit_buff);
}

void read_mac_address()
{
    // reading the mac address
    g_ethernet_device->mac_address[0] = inb(g_ethernet_device->io_base); 
    g_ethernet_device->mac_address[1] = inb(g_ethernet_device->io_base + 1); 
    g_ethernet_device->mac_address[2] = inb(g_ethernet_device->io_base + 2); 
    g_ethernet_device->mac_address[3] = inb(g_ethernet_device->io_base + 3); 
    g_ethernet_device->mac_address[4] = inb(g_ethernet_device->io_base + 4); 
    g_ethernet_device->mac_address[5] = inb(g_ethernet_device->io_base + 5);   
}