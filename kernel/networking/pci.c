#include "pci.h"

uint16_t read_dword_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset);
pci_header_data* get_pci_device_data(uint8_t bus, uint8_t device);

uint16_t read_dword_from_pci(uint8_t bus, uint8_t device, uint8_t func, uint8_t register_ofset)
{
    uint32_t data_address = (((uint32_t)bus) << 16 |    // aligning bus 
    ((uint32_t)device << 11) |                         // aligning device
    ((uint32_t)func) << 8 |                            // aliging func
    (register_ofset & 0xFC)                            // and func other than 2 first bits
    );

    // sending the data address to the pci config io register 
    outdw(CONFIG_ADDRESS, data_address);

    // returning 16 bit result
    return (indw(CONFIG_DATA) >> ((register_ofset & 2) * 8)) & 0xFFFF;    
}

pci_header_data* get_pci_device_data(uint8_t bus, uint8_t device)
{
    // allocating space for the pci device data
    pci_header_data* header = (pci_header_data*) kmalloc(sizeof(pci_header_data));

    // if device is valid
    if ((header->vendor_id = read_dword_from_pci(bus, device, 0, 0)) == 0xFFFF) {
        return NULL;
    } 

    // reading the pci header into the struct
    header->device_id = read_dword_from_pci(bus, device, 0, 2);
    header->command = read_dword_from_pci(bus, device, 1, 4);
    header->status = read_dword_from_pci(bus, device, 1, 6);
    header->revision_id = read_dword_from_pci(bus, device, 2, 8) & 0xFF;
    header->prog_if = read_dword_from_pci(bus, device, 2, 8) >> 8;
    header->subclass = read_dword_from_pci(bus, device, 2, 10) & 0xFF;
    header->class_code = read_dword_from_pci(bus, device, 2, 10) >> 8;
    header->cache_line_size = read_dword_from_pci(bus, device, 3, 0XC) & 0xFF;
    header->latency_timer = read_dword_from_pci(bus, device, 3, 0XC) >> 8;
    header->header_type = read_dword_from_pci(bus, device, 3, 0XE) & 0xFF;
    header->bist = read_dword_from_pci(bus, device, 3, 0XE) >> 8;
    return header;   
}