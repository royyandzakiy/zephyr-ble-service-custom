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

LOG_MODULE_REGISTER(app_ble_mydevice_service);

// ------------------------------------
static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value);
static void app_ble_mycommand_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value);

static uint8_t command_value = 0;
static ssize_t read_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				   void *buf, uint16_t len, uint16_t offset);
static ssize_t write_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len, uint16_t offset,
					uint8_t flags);

BT_GATT_SERVICE_DEFINE(mydevice_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_MYDEVICE_SERVICE),

    BT_GATT_CHARACTERISTIC(BT_UUID_MYSENSOR_CHAR, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                BT_GATT_PERM_READ, NULL, NULL, NULL), // Read permissions might be needed if notifiable characteristics are also readable.
    BT_GATT_CCC(app_ble_mysensor_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), // CCC needs read/write permissions
    
	BT_GATT_CHARACTERISTIC(BT_UUID_MYCOMMAND_CHAR,
		BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
		BT_GATT_PERM_WRITE,
		NULL, write_mycommand, &command_value),
	BT_GATT_CCC(app_ble_mycommand_ccc_cfg_changed_cb, // this can be a callback, or just NULL to do nothing when the notify config changes
			BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), // CCC needs read/write permissions
);

// ------------------------------------
// Add a flag to track if notifications are enabled
static bool mysensor_notifications_enabled = false;
static bool mycommand_notifications_enabled = false;

static void app_ble_mysensor_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    // Update the notification state flag
    mysensor_notifications_enabled = notif_enabled;

    LOG_INF("MySensor Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

static void app_ble_mycommand_ccc_cfg_changed_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    mycommand_notifications_enabled = notif_enabled;

    LOG_INF("MyCommand Characteristic notifications %s", notif_enabled ? "enabled" : "disabled");
}

// Character Value Position (Make sure this is correct relative to the service definition)
// Primary Service (1) + Characteristic (1) + CCC (1) = 3. Index is 0-based.
enum app_ble_mysensor_char_position
{
	MYDEVICE_SERVICE = 0,
	MYSENSOR_CHARAC_DECLARATION = 1,
    MYSENSOR_CHARAC_VALUE = 2, // 0: Service UUID, 1: Characteristic UUID, 2: Characteristic Value, 3: CCCD
	MYSENSOR_CCCD = 3,
	MYCOMMAND_CHARAC_DECLARATION = 4,
    MYCOMMAND_CHARAC_VALUE = 5,
	MYCOMMAND_CCCD = 6,
};

// Expose a getter function for the notification state
bool bt_mysensor_are_notifications_enabled(void)
{
    return mysensor_notifications_enabled;
}

// Expose a getter function for the notification state
bool bt_mycommand_are_notifications_enabled(void)
{
    return mycommand_notifications_enabled;
}

int bt_mysensor_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len)
{
    // The main loop will now check for connection and notification state before calling this.
    // This function will primarily handle the actual GATT notify call and its immediate result.

    const struct bt_gatt_attr *attr = &mydevice_service.attrs[MYSENSOR_CHARAC_VALUE];
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

int bt_mycommand_notify(struct bt_conn *current_conn, const uint8_t *data, const uint16_t len)
{
    // The main loop will now check for connection and notification state before calling this.
    // This function will primarily handle the actual GATT notify call and its immediate result.

    const struct bt_gatt_attr *attr = &mydevice_service.attrs[MYCOMMAND_CHARAC_VALUE];
    struct bt_gatt_notify_params params = {
        // .attr = attr,
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

// ------------------------------------

// Read callback function for the Command Characteristic
static ssize_t read_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				   void *buf, uint16_t len, uint16_t offset)
{
	// Get the pointer to the characteristic value from the attribute's user_data
	const uint8_t *value = attr->user_data;

    LOG_INF("Read accessed, given this data: 0x%02x", value);

	// Use bt_gatt_attr_read to handle offset and length checks for reading
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(command_value));
}

static uint8_t cmd_response_data[4];

static ssize_t write_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len, uint16_t offset,
					uint8_t flags)
{
	uint8_t *value = attr->user_data;

		if (len != sizeof(command_value)) {
		LOG_WRN("Invalid write length for command characteristic (%d vs %d expected)", len, sizeof(command_value));
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset > 0) {
		LOG_WRN("Invalid write offset for command characteristic (%d)", offset);
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value, buf, sizeof(command_value));
	LOG_INF("Command characteristic written. New value: 0x%02x", *value);

	switch (*value) {
	    case 0x01:
	        LOG_INF("Received command: Start something!");
			cmd_response_data[0] = 0x09;
			bt_mycommand_notify(conn, cmd_response_data, sizeof(cmd_response_data));
	        LOG_INF("Sent Command Response Data: %d", cmd_response_data[0]);
	        break;
	    case 0x02:
	        LOG_INF("Received command: Stop something!");
	        break;
	    default:
	        LOG_WRN("Received unknown command: 0x%02x", *value);
	        break;
	}

	return len;
}