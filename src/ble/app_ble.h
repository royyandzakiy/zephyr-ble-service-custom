#ifndef _APP_BLE_H
#define _APP_BLE_H

#include <services/mysensor.h>

int app_ble_init(void);
struct bt_conn *app_ble_get_connection(void);
int app_ble_send_mysensor_data(const uint8_t *data, uint16_t len);

extern struct bt_conn *current_conn;

#endif