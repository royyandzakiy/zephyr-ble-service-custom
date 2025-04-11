#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_ble_mysensor_service);

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "../app_ble.h"
#include <services/mysensor.h>
struct bt_conn *current_conn = NULL;

static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("MySensor Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

static void app_ble_mysensor2_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("MySensor2 Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

static void app_ble_mysensor3_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("MySensor3 Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

static void hrmc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("HRS notifications %s", notif_enabled ? "enabled" : "disabled");
}

/* Make sure this is updated to match BT_GATT_SERVICE_DEFINE below */
enum app_ble_mysensor_char_position
{
    MYSENSOR_ATTR_POS = 2,
    HRS_ATTR_POS = 5,
    MYSENSOR2_ATTR_POS = 8,
    MYSENSOR3_ATTR_POS = 11,
};

/* Air Quality Wing Service Declaration */
BT_GATT_SERVICE_DEFINE(mysensor_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MYSENSOR_SERVICE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, 
                BT_GATT_PERM_READ, NULL, NULL, NULL),
    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

int bt_mysensor_notify(struct bt_conn *conn, const uint16_t data)
{
    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = NULL;

    static uint8_t combined_data[2];
    combined_data[0] = 0x06; // will stay fixed with this number
	combined_data[1] = data; // will change dynamically

    attr = &mysensor_service.attrs[MYSENSOR_ATTR_POS];
    params.attr = attr;
    params.data = &combined_data;
    params.len = sizeof(combined_data);
    params.func = NULL;

    if (!conn)
    {
        LOG_DBG("Notification send to all connected peers");
        return bt_gatt_notify_cb(NULL, &params); // NULL here means notify to all, aka broadcast to all connections. else specify specific connection handle to notify a specific client ONLY
    }
    else if (bt_gatt_is_subscribed(conn, attr, BT_GATT_CCC_NOTIFY))
    {
        return bt_gatt_notify_cb(conn, &params);
    }
    else
    {
        return -EINVAL;
    }
}

int app_ble_send_mysensor_data(const uint8_t *data, uint16_t len)
{
    if (!current_conn) {
        LOG_WRN("Not connected, cannot send MySensor data");
        return -ENOTCONN;
    }

    const struct bt_gatt_attr *attr = &mysensor_service.attrs[MYSENSOR_ATTR_POS];
    struct bt_gatt_notify_params params = {
        .attr = attr,
        .data = data,
        .len = len,
        .func = NULL,
    };

    if (bt_gatt_is_subscribed(current_conn, attr, BT_GATT_CCC_NOTIFY)) {
        int ret = bt_gatt_notify_cb(current_conn, &params);
        if (ret) {
            LOG_ERR("Failed to send MySensor notification (err %d)", ret);
            return ret;
        }
        return 0;
    } else {
        LOG_WRN("MySensor notifications not enabled on the client");
        return -EAGAIN;
    }
}

// void some_function_where_you_read_adc(uint16_t adc_value)
// {
//     // Assuming you want to send the raw 16-bit ADC value
//     if (app_ble_get_connection()) {
//         app_ble_send_mysensor_data((uint8_t *)&adc_value, sizeof(adc_value));
//     } else {
//         LOG_WRN("Not connected, ADC data not sent");
//     }
// }

// // If you want to pack more data (e.g., multiple ADC readings)
// #define ADC_BUFFER_SIZE 10
// uint16_t adc_buffer[ADC_BUFFER_SIZE];
// int adc_index = 0;

// void another_adc_reading_function(uint16_t new_adc_value)
// {
//     adc_buffer[adc_index++] = new_adc_value;
//     if (adc_index == ADC_BUFFER_SIZE) {
//         if (app_ble_get_connection()) {
//             app_ble_send_mysensor_data((uint8_t *)adc_buffer, sizeof(adc_buffer));
//         } else {
//             LOG_WRN("Not connected, ADC buffer not sent");
//         }
//         adc_index = 0;
//     }
// }

// int app_ble_send_mysensor_data(const uint8_t *data, uint16_t len)
// {
//     const struct bt_gatt_attr *attr = &mysensor_service.attrs[MYSENSOR_ATTR_POS];
//     struct bt_gatt_notify_params params = {
//         .attr = attr,
//         .data = data,
//         .len = len,
//         .func = NULL,
//     };

//     if (bt_gatt_is_subscribed(NULL, attr, BT_GATT_CCC_NOTIFY)) {
//         int ret = bt_gatt_notify_cb(NULL, &params);
//         if (ret) {
//             LOG_ERR("Failed to send MySensor notification (err %d)", ret);
//             return ret;
//         }
//         return 0;
//     } else {
//         LOG_WRN("MySensor notifications not enabled on the client");
//         return -EAGAIN;
//     }
// }