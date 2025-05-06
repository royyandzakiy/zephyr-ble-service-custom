// services/mysensor.c
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

#include <app_ble.h> // Keep this if needed for app_ble_get_connection or other app_ble functions
#include <services/mysensor.h>

LOG_MODULE_REGISTER(app_ble_mysensor_service);

// Add a flag to track if notifications are enabled
static bool mysensor_notifications_enabled = false;

// Notification (CCC) Change Callback
static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    // Update the notification state flag
    mysensor_notifications_enabled = notif_enabled;

    LOG_INF("MySensor Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

// Character Value Position (Make sure this is correct relative to the service definition)
// Primary Service (1) + Characteristic (1) + CCC (1) = 3. Index is 0-based.
enum app_ble_mysensor_char_position
{
    MYSENSOR_ATTR_POS = 2, // 0: Service UUID, 1: Characteristic UUID, 2: Characteristic Value, 3: CCCD
};

// Service & Characteristic Declaration
BT_GATT_SERVICE_DEFINE(mysensor_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MYSENSOR_SERVICE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                BT_GATT_PERM_READ, NULL, NULL, NULL), // Read permissions might be needed if notifiable characteristics are also readable.
    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), // CCC needs read/write permissions
);

// Expose a getter function for the notification state
bool bt_mysensor_are_notifications_enabled(void)
{
    return mysensor_notifications_enabled;
}


int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len)
{
    // The main loop will now check for connection and notification state before calling this.
    // This function will primarily handle the actual GATT notify call and its immediate result.

    const struct bt_gatt_attr *attr = &mysensor_service.attrs[MYSENSOR_ATTR_POS];
    struct bt_gatt_notify_params params = {
        .attr = attr,
        .data = data,
        .len = len,
        .func = NULL, // Or a callback function if you need confirmation of TX complete
    };

    // Use bt_gatt_notify_cb for the actual notification
    int ret = bt_gatt_notify_cb(current_conn, &params);
    if (ret) {
        // This log will now only appear for actual transmission errors, not due to
        // notifications being disabled or not connected.
        LOG_ERR("Failed to send MySensor notification (err %d)", ret);
        return ret;
    }

    return 0; // Success
}