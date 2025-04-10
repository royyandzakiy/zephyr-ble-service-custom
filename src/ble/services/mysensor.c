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

    BT_GATT_CHARACTERISTIC(BT_UUID_HRS_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
                BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(hrmc_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR2, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, 
                BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(app_ble_mysensor2_ccc_cfg_changed_cb,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR3, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, 
                BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(app_ble_mysensor3_ccc_cfg_changed_cb,
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

int bt_mysensor2_notify(uint16_t data)
{
	int rc;

	rc = bt_gatt_notify(NULL, &mysensor_service.attrs[MYSENSOR2_ATTR_POS], &data, sizeof(data));

	return rc == -ENOTCONN ? 0 : rc;
}

int bt_mysensor3_notify(uint16_t data)
{
	int rc;

	rc = bt_gatt_notify(NULL, &mysensor_service.attrs[MYSENSOR3_ATTR_POS], &data, sizeof(data));

	return rc == -ENOTCONN ? 0 : rc;
}

int bt_hrs_notify(uint16_t heartrate)
{
	int rc;
	static uint8_t hrm[2];

	hrm[0] = 0x06; /* uint8, sensor contact */
	hrm[1] = heartrate;

	rc = bt_gatt_notify(NULL, &mysensor_service.attrs[HRS_ATTR_POS], &hrm, sizeof(hrm));

	return rc == -ENOTCONN ? 0 : rc;
}