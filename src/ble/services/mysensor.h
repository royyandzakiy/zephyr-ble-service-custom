#ifndef _MYSENSOR_SERVICE_H
#define _MYSENSOR_SERVICE_H

#include <app_ble.h>

// generate custom UUIDs here https://www.uuidgenerator.net/

#define BT_UUID_MYSENSOR_SERVICE_VAL \
    BT_UUID_128_ENCODE(0xd0b30001, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

/* Characterisitcs for sensors */
#define BT_UUID_MYSENSOR_VAL \
    BT_UUID_128_ENCODE(0xd0b30002, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)
#define BT_UUID_MYSENSOR2_VAL \
    BT_UUID_128_ENCODE(0xd0b30003, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)
#define BT_UUID_MYSENSOR3_VAL \
    BT_UUID_128_ENCODE(0xd0b30004, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

#define BT_UUID_MYSENSOR_SERVICE    BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_SERVICE_VAL)
#define BT_UUID_MYSENSOR            BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_VAL)
#define BT_UUID_MYSENSOR2           BT_UUID_DECLARE_128(BT_UUID_MYSENSOR2_VAL)
#define BT_UUID_MYSENSOR3           BT_UUID_DECLARE_128(BT_UUID_MYSENSOR3_VAL)

int bt_mysensor_notify(struct bt_conn *conn, const uint16_t data);
int bt_mysensor2_notify(const uint16_t data);
int bt_mysensor3_notify(const uint16_t data);
int bt_hrs_notify(uint16_t heartrate);

#endif /*_MYSENSOR_SERVICE_H*/