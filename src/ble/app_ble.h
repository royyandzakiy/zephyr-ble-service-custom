#ifndef _APP_BLE_H
#define _APP_BLE_H

int app_ble_init(void);
struct bt_conn *app_ble_get_connection(void);
int app_ble_mysensor_data_send(const uint8_t *data, const uint16_t len);

#endif