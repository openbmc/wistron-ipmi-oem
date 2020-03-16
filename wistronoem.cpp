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

#define SWITCH_SLAVE_ADDRESS 112 /* Hexadecimal value:0x70 */
#define RISERF_SLAVE_ADDRESS 16  /* Hexadecimal value:0x10 */

void register_detect_riserf() __attribute__((constructor));

ipmi_ret_t ipmi_wistron_detect_riserf(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                      ipmi_request_t request, ipmi_response_t response,
                                      ipmi_data_len_t data_len, ipmi_context_t context)
{
    phosphor::smbus::Smbus smbus;

    auto init_cp0 = smbus.smbusInit(9);
    if (init_cp0 == initError)
    {
        std::cerr << "smbusInit fail!" << "\n";
        return false;
    }
    smbus.SetSmbusCmdByte(9, SWITCH_SLAVE_ADDRESS, 0, 8);
    auto res_cp0 = smbus.GetSmbusCmdByte(9, RISERF_SLAVE_ADDRESS, command);

    auto init_cp1 = smbus.smbusInit(10);
    if (init_cp1 == initError)
    {
        std::cerr << "smbusInit fail!" << "\n";
        return false;
    }
    smbus.SetSmbusCmdByte(10, SWITCH_SLAVE_ADDRESS, 0, 8);
    auto res_cp1 = smbus.GetSmbusCmdByte(10, RISERF_SLAVE_ADDRESS, command);

    // CPU0 & CPU1 are not present.
    if (res_cp0 < 0 && res_cp1 <0)
    {
        *data_len = sizeof(a);
        memcpy(response, &a, *data_len);
    }

    // CPU0 isn't present but CPU1 is present.
    if (res_cp0 < 0 && res_cp1 >= 0)
    {
        *data_len = sizeof(b);
        memcpy(response, &b, *data_len);
    }

    // CPU0 is present but CPU1 isn't present.
    if (res_cp0 >= 0 && res_cp1 < 0)
    {
        *data_len = sizeof(c);
        memcpy(response, &c, *data_len);
    }

    // CPU0 & CPU1 are present.
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
    ipmi_register_callback(NETFUN_OEM, IPMI_CMD_DETECT_RISERF, NULL,
                           ipmi_wistron_detect_riserf, SYSTEM_INTERFACE); 
}
