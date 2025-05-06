#ifndef _MYSENSOR_SERVICE_H
#define _MYSENSOR_SERVICE_H

#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h> // Needed for struct bt_conn
#include "additionals.h"

// generate custom UUIDs here https://www.uuidgenerator.net/

// static const winrt::guid NEXUS_SERVICE_UUID = {
//     0x0483dadd, 0x6c9d, 0x6ca9, { 0x5d, 0x41, 0x03, 0xad, 0x4f, 0xff, 0x4a, 0xbb }
// };

// // Nexus notify characteristic UUID (16-bit value 0xF001, canonicalized)
// static const winrt::guid NEXUS_NOTIFY_CHAR_UUID = {
//     0x0000f001, 0x0000, 0x1000, { 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
// };

// D0B30001-6723-449F-8800-FA6B48E49A5B

/* ============== UUID ============== */
// 1. UUID for the BLE Service
#ifndef _ADDITIONALS_H_
#define BT_UUID_MYSENSOR_SERVICE_VAL \
BT_UUID_128_ENCODE(0xd0b30001, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

// 2. UUID for each BLE Characterisitcs as Sensor Value
#define BT_UUID_MYSENSOR_VAL \
BT_UUID_128_ENCODE(0xd0b30002, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)
#endif // _ADDITIONALS_H_

// 3. construct into bt_uuid object
#define BT_UUID_MYSENSOR_SERVICE    BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_SERVICE_VAL)
#define BT_UUID_MYSENSOR            BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_VAL)

// 4. expose notify functions
int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len);
bool bt_mysensor_are_notifications_enabled(void); 

#endif /*_MYSENSOR_SERVICE_H*/