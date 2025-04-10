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

#include <app_ble.h>
#include <services/mysensor.h>

static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("Air Quality Wing service notifications %s", notif_enabled ? "enabled" : "disabled");
}

/* Make sure this is updated to match BT_GATT_SERVICE_DEFINE below */
enum app_ble_mysensor_char_position
{
    MYSENSOR_ADC_ATTR_POS = 2,
    // AQW_TEMP_ATTR_POS = 2,
    // AQW_HUMIDITY_ATTR_POS = 5,
    // AQW_VOC_ATTR_POS = 8,
    // AQW_PM25_ATTR_POS = 11,
};

/* Air Quality Wing Service Declaration */
BT_GATT_SERVICE_DEFINE(mysensor_service,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_MYSENSOR_SERVICE),

                       BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR_ADC, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, 
                                    BT_GATT_PERM_READ, NULL, NULL, NULL),
                       BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                    //    BT_GATT_CHARACTERISTIC(BT_UUID_AQW_TEMPERATURE, BT_GATT_CHRC_NOTIFY,
                    //                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
                    //    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb,
                    //                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                    //    BT_GATT_CHARACTERISTIC(BT_UUID_AQW_HUMIDITY, BT_GATT_CHRC_NOTIFY,
                    //                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
                    //    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb,
                    //                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                    //    BT_GATT_CHARACTERISTIC(BT_UUID_AQW_VOC, BT_GATT_CHRC_NOTIFY,
                    //                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
                    //    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb,
                    //                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                    //    BT_GATT_CHARACTERISTIC(BT_UUID_AQW_PM25, BT_GATT_CHRC_NOTIFY,
                    //                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
                    //    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb,
                    //                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), 
                        );

// void app_ble_mysensor_publish()
int mysensor_publish(struct bt_conn *conn, const int *data)
{
    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = NULL;

    // switch (data->type)
    // switch (99)
    // {
    // case MYSENSOR_ADC_SENSOR:
    //     attr = &mysensor_service.attrs[MYSENSOR_ADC_ATTR_POS];
    //     break;
    // default:
    //     return -EINVAL;
    //     break;
    // }

    attr = &mysensor_service.attrs[MYSENSOR_ADC_ATTR_POS];
    params.attr = attr;
    params.data = &data;
    params.len = sizeof(data);
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