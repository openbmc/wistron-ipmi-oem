#include "config.h"

#include "wistronoem.hpp"

#include <endian.h>
#include <ipmid/api.h>
#include <stdio.h>
#include <string.h>
#include <systemd/sd-bus.h>

#include "smbus.hpp"
#include "i2c.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <ipmid-host/cmd.hpp>
#include <memory>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/exception.hpp>

#define SWITCH_SLAVE_ADDRESS 112 //0x70
#define RISERF_SLAVE_ADDRESS 16 //0x10

void register_detect_riserf() __attribute__((constructor));

ipmi_ret_t ipmi_wistron_detect_riserf(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                     ipmi_request_t request, ipmi_response_t response,
                                     ipmi_data_len_t data_len, ipmi_context_t context)
{
    unsigned char command = 0;
    phosphor::smbus::Smbus smbus;
    unsigned char a[] = {0x00, 0x00};
    unsigned char b[] = {0x00, 0x01};
    unsigned char c[] = {0x01, 0x00};
    unsigned char d[] = {0x01, 0x01};


    auto init_cp0 = smbus.smbusInit(9);
    if (init_cp0 == -1)
    {
        std::cerr << "smbusInit fail!" << std::endl;
        return false;
    }
    smbus.SetSmbusCmdByte(9, SWITCH_SLAVE_ADDRESS, 0, 8);
    auto res_cp0 = smbus.GetSmbusCmdByte(9, RISERF_SLAVE_ADDRESS, command);

    auto init_cp1 = smbus.smbusInit(10);
    if (init_cp1 == -1)
    {
        std::cerr << "smbusInit fail!" << std::endl;
        return false;
    }
    smbus.SetSmbusCmdByte(10, SWITCH_SLAVE_ADDRESS, 0, 8);
    auto res_cp1 = smbus.GetSmbusCmdByte(10, RISERF_SLAVE_ADDRESS, command);

    if (res_cp0 < 0 && res_cp1 <0)
    {
        *data_len = sizeof(a);
        memcpy(response, &a, *data_len);
    }
    if (res_cp0 < 0 && res_cp1 >= 0)
    {
        *data_len = sizeof(b);
        memcpy(response, &b, *data_len);
    }
    if (res_cp0 >= 0 && res_cp1 < 0)
    {
        *data_len = sizeof(c);
        memcpy(response, &c, *data_len);
    }
    if (res_cp0 >= 0 && res_cp1 >= 0)
    {
        *data_len = sizeof(d);
        memcpy(response, &d, *data_len);
    }

    smbus.smbusClose(9);
    smbus.smbusClose(10);

    return IPMI_CC_OK;
}

void register_detect_riserf()
{
    printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", NETFUN_OEM, IPMI_CMD_DETECT_RISERF);
    ipmi_register_callback(NETFUN_OEM, IPMI_CMD_DETECT_RISERF, NULL, ipmi_wistron_detect_riserf,
                           SYSTEM_INTERFACE); 
    return;
}
