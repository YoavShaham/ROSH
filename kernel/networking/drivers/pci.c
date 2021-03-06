#include "pci.h"
#include "../../../libc/screen.h"

uint16_t read_word_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset);
uint32_t read_dword_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset);
pci_header_data* get_pci_device_data(uint8_t bus, uint8_t device);
device_data* get_pci_device(uint16_t device_id, uint16_t vendor_id);
device_data* get_ethernet_controller();
void read_device_bars(pci_header_data* header, uint8_t bus, uint8_t device);

uint16_t read_word_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset)
{
    uint32_t address = (uint32_t)(((uint32_t)bus << 16) | ((uint32_t)device << 11) |
              ((uint32_t)func << 8) | ((uint32_t)register_ofset & 0xFC) | ((uint32_t)0x80000000));

    // sending the data address to the pci config io register 
    outdw(CONFIG_ADDRESS, address);

    // returning 16 bit result
    return (uint16_t)((indw(0xCFC) >> ((register_ofset & 2) * 8)) & 0xFFFF);
}

uint32_t read_dword_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset)
{
    uint32_t address = (uint32_t)(((uint32_t)bus << 16) | ((uint32_t)device << 11) |
              ((uint32_t)func << 8) | ((uint32_t)register_ofset & 0xFC) | ((uint32_t)0x80000000));

    // sending the data address to the pci config io register 
    outdw(CONFIG_ADDRESS, address);

    // returning 32 bit result
    return indw(CONFIG_DATA) ;    
}

void write_dword_to_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset, uint32_t value) {
	uint32_t address = (uint32_t)(((uint32_t)bus << 16) | ((uint32_t)device << 11) |
              ((uint32_t)func << 8) | ((uint32_t)register_ofset & 0xFC) | ((uint32_t)0x80000000));

    // sending the data address to the pci config io register 
    outdw(CONFIG_ADDRESS, address);
	outdw(CONFIG_DATA, value);
}

pci_header_data* get_pci_device_data(uint8_t bus, uint8_t device)
{
    // allocating space for the pci device data
    pci_header_data* header = (pci_header_data*) kmalloc(sizeof(pci_header_data));

    // if device is valid
	header->vendor_id = read_word_from_pci(bus, device, 0, 0);
    if (header->vendor_id == 0xFFFF) {
		kfree(header);
        return NULL;
    } 

    // reading the pci header into the struct
    header->device_id = read_word_from_pci(bus, device, 0, 2);
    header->command = read_word_from_pci(bus, device, 0, 4);
    header->status = read_word_from_pci(bus, device, 0, 6);
    header->revision_id = read_word_from_pci(bus, device, 0, 8) & 0xFF;
    header->prog_if = read_word_from_pci(bus, device, 0, 8) >> 8;
    header->subclass = read_word_from_pci(bus, device, 0, 10) & 0xFF;
    header->class_code = read_word_from_pci(bus, device, 0, 10) >> 8;
    header->cache_line_size = read_word_from_pci(bus, device, 0, 0xC) & 0xFF;
    header->latency_timer = read_word_from_pci(bus, device, 0, 0xC) >> 8;
    header->header_type = read_word_from_pci(bus, device, 0, 0xE) & 0xFF;
    header->bist = read_word_from_pci(bus, device, 0, 0xE) >> 8;

    // reading device bars
    if(header->header_type == HEADER_DEFUALT)
    {
        header->interrupt_line = read_word_from_pci(bus, device, 0, 0x3C) & 0xFF;
        read_device_bars(header, bus, device); 
    }
    return header;   
}

device_data* get_pci_device(uint16_t device_id, uint16_t vendor_id)
{

    // going through the pci devices
    for(uint32_t bus = 0; bus < BUS_NUM; bus++)
    {
        for (uint8_t device = 0; device < DEVICE_NUM; device++)
        {
            // getting device header
            pci_header_data* header = (pci_header_data*)get_pci_device_data(bus, device);
            
            // if device exist
            if(header != NULL)
            {   
                // if requested device
                if(header->device_id == device_id &&
                header->vendor_id == vendor_id)
                {
                    device_data* data = (device_data*)kmalloc(sizeof(device_data));
                    data->bus_num = bus;
                    data->device_num = device;
                    data->device_header = header;
					kfree(header);
                    return data;
                }
				kfree(header);
            }
        }
    }
    // device wasn't found
    return NULL;
}

device_data* get_ethernet_controller()
{
    return get_pci_device(RTL8139_DEVICE_ID, RTL8139_VENDOR_ID);
}

void read_device_bars(pci_header_data* header, uint8_t bus, uint8_t device)
{

    // going through the header bars
    for(int i = 0; i < BAR_NUM; i++)
    {
        // getting the bar type
        header->bars[i].type = read_dword_from_pci(bus, device, 0, 0x10 + 4 * i) & 1;
        
        // if IO bar
        if (header->bars[i].type)
        {
            header->bars[i].value.io_address = read_dword_from_pci(bus, device, 0, 0x10 + 4 * i) & 0xFFFFFFF0;
        }

        // if memory address bar
        else
        {
            header->bars[i].value.address = read_dword_from_pci(bus, device, 0, 0x10 + 4 * i) & 0xFFFFFFFC;
        }
    }
}