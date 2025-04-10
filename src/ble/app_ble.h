#ifndef _APP_BLE_H
#define _APP_BLE_H

// #include <zephyr/drivers/sensor.h>

#include <services/mysensor.h>

// struct app_ble_payload
// {
//     enum mysensor_sensor_type type;
//     struct sensor_value value;
//     uint64_t ts;
// };

int app_ble_init(void);
int app_ble_publish(const int *data);
// int app_ble_publish_sensor_data(const struct app_ble_payload *data);

#endif