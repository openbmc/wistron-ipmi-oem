#pragma once

#include <ipmid/api.h>
#include <stdint.h>

#include <map>
#include <string>

// IPMI commands for net functions.
enum ipmi_netfn_oem_cmds
{
    IPMI_CMD_DETECT_RISERF = 0x01,
};


ipmi_ret_t ipmi_wistron_detect_riserf(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                              ipmi_request_t request, ipmi_response_t response,
                              ipmi_data_len_t data_len, ipmi_context_t context);

