#ifndef _MYSENSOR_SERVICE_H
#define _MYSENSOR_SERVICE_H

#include <app_ble.h>

#define BT_UUID_MYSENSOR_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x13370001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

/* Characterisitcs for sensors */
#define BT_UUID_MYSENSOR_VAL \
    BT_UUID_128_ENCODE(0x13370002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_MYSENSOR2_VAL \
    BT_UUID_128_ENCODE(0x13370003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_MYSENSOR3_VAL \
    BT_UUID_128_ENCODE(0x13370004, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)

#define BT_UUID_MYSENSOR_SERVICE    BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_SERVICE_VAL)
#define BT_UUID_MYSENSOR            BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_VAL)
#define BT_UUID_MYSENSOR2           BT_UUID_DECLARE_128(BT_UUID_MYSENSOR2_VAL)
#define BT_UUID_MYSENSOR3           BT_UUID_DECLARE_128(BT_UUID_MYSENSOR3_VAL)

int bt_mysensor_notify(struct bt_conn *conn, const uint16_t data);
int bt_mysensor2_notify(const uint16_t data);
int bt_mysensor3_notify(const uint16_t data);
int bt_hrs_notify(uint16_t heartrate);

#endif /*_MYSENSOR_SERVICE_H*/