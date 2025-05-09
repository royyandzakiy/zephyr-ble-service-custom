#ifndef _MYDEVICE_SERVICE_H
#define _MYDEVICE_SERVICE_H

#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h> // Needed for struct bt_conn
#include "additionals.h"

// generate custom UUIDs here https://www.uuidgenerator.net/

/* ============== UUID ============== */
#ifndef _ADDITIONALS_H_
#define BT_UUID_MYDEVICE_SERVICE_VAL \
BT_UUID_128_ENCODE(0xd0b30001, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

#define BT_UUID_MYSENSOR_CHAR_VAL \
BT_UUID_128_ENCODE(0xd0b30002, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

#define BT_UUID_MYCOMMAND_CHAR_VAL \
BT_UUID_128_ENCODE(0xb30c0458, 0xe98a, 0x4eaa, 0x873f, 0xd774bb779626)
#endif // _ADDITIONALS_H_

#define BT_UUID_MYDEVICE_SERVICE    BT_UUID_DECLARE_128(BT_UUID_MYDEVICE_SERVICE_VAL)
#define BT_UUID_MYSENSOR_CHAR       BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_CHAR_VAL)
#define BT_UUID_MYCOMMAND_CHAR 		BT_UUID_DECLARE_128(BT_UUID_MYCOMMAND_CHAR_VAL)

int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len);
int bt_mycommand_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len);
bool bt_mysensor_are_notifications_enabled(void); 
bool bt_mycommand_are_notifications_enabled(void); 

#endif /*_MYDEVICE_SERVICE_H*/