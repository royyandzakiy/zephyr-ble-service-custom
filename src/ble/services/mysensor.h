#ifndef _MYSENSOR_SERVICE_H
#define _MYSENSOR_SERVICE_H

// generate custom UUIDs here https://www.uuidgenerator.net/

/* ============== UUID ============== */
// 1. UUID for the BLE Service
#define BT_UUID_MYSENSOR_SERVICE_VAL \
    BT_UUID_128_ENCODE(0xd0b30001, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

// 2. UUID for each BLE Characterisitcs as Sensor Value
#define BT_UUID_MYSENSOR_VAL \
    BT_UUID_128_ENCODE(0xd0b30002, 0x6723, 0x449f, 0x8800, 0xfa6b48e49a5b)

// 3. construct into bt_uuid object
#define BT_UUID_MYSENSOR_SERVICE    BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_SERVICE_VAL)
#define BT_UUID_MYSENSOR            BT_UUID_DECLARE_128(BT_UUID_MYSENSOR_VAL)

// 4. expose notify functions
int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len);

#endif /*_MYSENSOR_SERVICE_H*/