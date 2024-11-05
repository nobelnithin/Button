/*
 * Basic macros and functions for Bluetooth Host Controller Interface Layer.
 *
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"

#define HCI_H4_CMD_PREAMBLE_SIZE           (4)

/*  HCI Command Opcode group Field (OGF) */
#define HCI_GRP_HOST_CONT_BASEBAND_CMDS    (0x03 << 10)            /* 0x0C00 */
#define HCI_GRP_BLE_CMDS                   (0x08 << 10)

/*  HCI Command Opcode Command Field (OCF) */
#define HCI_OCF_RESET                   (0x0003)
#define HCI_OCF_SET_EVT_MASK            (0x0001)
#define HCI_OCF_SET_RANDOM_ADDR         (0x0005)
#define HCI_OCF_WRITE_ADV_ENABLE        (0x000A)
#define HCI_OCF_WRITE_ADV_DATA          (0x0008)
#define HCI_OCF_WRITE_ADV_PARAMS        (0x0006)
#define HCI_OCF_WRITE_SCAN_PARAM        (0x000B)
#define HCI_OCF_WRITE_SCAN_ENABLE       (0x000C)
#define HCI_OCF_READ_WHITE_LIST_SIZE    (0x000F)
#define HCI_OCF_CLEAR_ACCEPT_LIST       (0x0010)
#define HCI_OCF_ADD_TO_ACCEPT_LIST      (0x0011)

/*  HCI Command Opcode Command Field (OCF) */
#define HCI_RESET                       (HCI_OCF_RESET | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
#define HCI_SET_EVT_MASK                (HCI_OCF_SET_EVT_MASK | HCI_GRP_HOST_CONT_BASEBAND_CMDS)

/*  HCI Random Address Set */
#define HCI_BLE_SET_RANDOM_ADDR      (HCI_OCF_SET_RANDOM_ADDR | HCI_GRP_BLE_CMDS)
/* Advertising Commands. */
#define HCI_BLE_WRITE_ADV_ENABLE        (HCI_OCF_WRITE_ADV_ENABLE | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_DATA          (HCI_OCF_WRITE_ADV_DATA | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_PARAMS        (HCI_OCF_WRITE_ADV_PARAMS | HCI_GRP_BLE_CMDS)
/* Scan commands */
#define HCI_BLE_WRITE_SCAN_PARAM        (HCI_OCF_WRITE_SCAN_PARAM | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_SCAN_ENABLE       (HCI_OCF_WRITE_SCAN_ENABLE | HCI_GRP_BLE_CMDS)

/* Filter Update commands*/
#define HCI_BLE_READ_WHITE_LIST_SIZE     (HCI_OCF_READ_WHITE_LIST_SIZE | HCI_GRP_BLE_CMDS)
#define HCI_BLE_CLEAR_ACCEPT_LIST        (HCI_OCF_CLEAR_ACCEPT_LIST | HCI_GRP_BLE_CMDS)
#define HCI_BLE_ADD_TO_ACCEPT_LIST       (HCI_OCF_ADD_TO_ACCEPT_LIST | HCI_GRP_BLE_CMDS)

/* HCI Command length. */
#define HCIC_PARAM_SIZE_WRITE_ADV_ENABLE        (1)
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS    (15)
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA      (31)
#define HCIC_PARAM_SIZE_BLE_SET_RANDOM_ADDR     (6)
#define HCIC_PARAM_SIZE_BLE_ADD_TO_WHITE_LIST   (7)
#define HCIC_PARAM_SIZE_SET_EVENT_MASK          (8)
#define HCIC_PARAM_SIZE_BLE_WRITE_SCAN_PARAM    (7)
#define HCIC_PARAM_SIZE_BLE_WRITE_SCAN_ENABLE   (2)

/* LE Meta Events. */
#define LE_META_EVENTS                          (0x3E)
#define HCI_LE_ADV_REPORT                       (0x02)

#define LE_CMD_COMPLETE_EVENT                   (0x0E)

#define BD_ADDR_LEN     (6)                     /* Device address length */
typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */

#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define BDADDR_TO_STREAM(p, a)   {int ijk; for (ijk = 0; ijk < BD_ADDR_LEN;  ijk++) *(p)++ = (uint8_t) a[BD_ADDR_LEN - 1 - ijk];}
#define ARRAY_TO_STREAM(p, a, len) {int ijk; for (ijk = 0; ijk < len;        ijk++) *(p)++ = (uint8_t) a[ijk];}

enum {
    H4_TYPE_COMMAND = 1,
    H4_TYPE_ACL     = 2,
    H4_TYPE_SCO     = 3,
    H4_TYPE_EVENT   = 4
};

/**
 * @brief Writes reset bit in buf and returns size of input buffer after
 *        writing in it.
 *
 * @param buf Input buffer to write which will be sent to controller.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_reset(uint8_t *buf);

/**
 * @brief   This function is used to request the Controller to start or stop advertising.
 *
 * @param buf         Input buffer to write which will be sent to controller.
 * @param adv_enable  1 to enable advertising and 0 to disable.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_enable(uint8_t *buf, uint8_t adv_enable);

/**
 * @brief   This function is used by the Host to set the advertising parameters.
 *
 * @param  buf               Input buffer to write which will be sent to controller.
 * @param  adv_int_min       Minimum advertising interval.
 * @param  adv_int_max       Maximum advertising interval.
 * @param  adv_type          Advertising type.
 * @param  addr_type_own     Own advertising type.
 * @param  addr_type_peer    Peer device's address type.
 * @param  peer_addr         Peer device's BD address.
 * @param  channel_map       Advertising channel map.
 * @param  adv_filter_policy Advertising Filter Policy.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_param(uint8_t *buf, uint16_t adv_int_min, uint16_t adv_int_max,
                                    uint8_t adv_type, uint8_t addr_type_own,
                                    uint8_t addr_type_peer, bd_addr_t peer_addr,
                                    uint8_t channel_map, uint8_t adv_filter_policy);

/**
 * @brief    This function is used to set the data used in advertising packets that have a data field.
 *
 * @param   buf       Input buffer to write which will be sent to controller.
 * @param   data_len  Length of p_data.
 * @param   p_data    Data to be set.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_data(uint8_t *buf, uint8_t data_len, uint8_t *p_data);

/**
 * @brief  This function is used to control which LE events are generated by the HCI for the Host.
 *         The event mask allows the Host to control which events will interrupt it.
 *
 * @param  buf          Input buffer to write which will be sent to controller.
 * @param  evt_mask     8 byte data as per spec.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_set_evt_mask(uint8_t *buf, uint8_t *evt_mask);

/**
 * @brief   This function is used to set the scan parameters.
 *
 * @param   buf              Input buffer to write which will be sent to controller.
 * @param   scan_type        Active or Passive scanning.
 * @param   scan_interval    Set scan_interval.
 * @param   scan_window      Set scan_window.
 * @param   own_addr_type    Set own address type.
 * @param   filter_policy    Scanning filter policy.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_scan_params(uint8_t *buf, uint8_t scan_type,
                                      uint16_t scan_interval, uint16_t scan_window, uint8_t own_addr_type,
                                      uint8_t filter_policy);

/**
 * @brief   This function is used to set the data used in advertising packets that have a data field.
 *
 * @param   buf                 Input buffer to write which will be sent to controller.
 * @param   scan_enable         Enable or disable scanning.
 * @param   filter_duplicates   Filter duplicates enable or disable.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_scan_enable(uint8_t *buf, uint8_t scan_enable,
                                      uint8_t filter_duplicates);

/**
 * @brief  This function is used to add a single device to the Filter Accept List stored in the Controller.
 *
 * @param buf            Input buffer to write which will be sent to controller.
 * @param addr_type      device's address type. 0x00 for public and 0x01 for random.
 * @param peer_addr      device's BD address.
 * @return               Size of buf after writing into it.
 */
uint16_t make_cmd_ble_add_to_filter_accept_list(uint8_t *buf, uint8_t addr_type, bd_addr_t addr);

/**
 * @brief  This function is used to clear the white list stored in the controller.
 *
 * @param buf            Input buffer to write which will be sent to controller.
 * @return               Size of buf after writing into it.
 */
uint16_t make_cmd_ble_clear_white_list(uint8_t *buf);

/**
 * @brief This function is used to set random address.
 *
 * @param buf            Input buffer to write which will be sent to controller.
 * @param addr           Random address to be set.
 * @return               Size of buf after writing into it.
 */
uint16_t make_cmd_set_random_address(uint8_t *buf, bd_addr_t addr);

#ifdef __cplusplus
}
#endif
