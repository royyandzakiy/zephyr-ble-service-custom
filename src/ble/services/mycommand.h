// services/mycommand.h
#ifndef _MYCOMMAND_SERVICE_H
#define _MYCOMMAND_SERVICE_H

#include <zephyr/bluetooth/uuid.h>
#include "additionals.h"

/* ============== UUID ============== */
// Generate custom UUIDs here https://www.uuidgenerator.net/
// New Service UUID: A0B1C2D3-E4F5-6789-ABCD-EF0123456789

#define BT_UUID_MYCOMMAND_SERVICE_VAL \
	BT_UUID_128_ENCODE(0xa0b1c2d3, 0xe4f5, 0x6789, 0xabcd, 0xef0123456789)

// New Characteristic UUID (Read/Write): 11223344-5566-7788-99AA-BBCCDDeeff00
#define BT_UUID_MYCOMMAND_CHAR_VAL \
	BT_UUID_128_ENCODE(0x11223344, 0x5566, 0x7788, 0x99aa, 0xbbccddeeff00)

// 3. Construct into bt_uuid object
#define BT_UUID_MYCOMMAND_SERVICE 	BT_UUID_DECLARE_128(BT_UUID_MYCOMMAND_SERVICE_VAL)
#define BT_UUID_MYCOMMAND_CHAR 		BT_UUID_DECLARE_128(BT_UUID_MYCOMMAND_CHAR_VAL)

// No external function declarations needed for this simple read/write service
// unless the main app needs to interact with the characteristic value directly.

#endif /*_MYCOMMAND_SERVICE_H*/