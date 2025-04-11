#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <app_ble.h>
#include <services/mysensor.h>

LOG_MODULE_REGISTER(app_ble_mysensor_service);

// Notification (CCC) Change Callback
static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("MySensor Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

// Character Value Position
enum app_ble_mysensor_char_position
{
    MYSENSOR_ATTR_POS = 2,
};

// Service & Characteristic Declaration
BT_GATT_SERVICE_DEFINE(mysensor_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MYSENSOR_SERVICE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, 
                BT_GATT_PERM_READ, NULL, NULL, NULL),
    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len)
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
        // LOG_WRN("MySensor notifications not enabled on the client");
        return -EAGAIN;
    }
}