// services/mycommand.c
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

#include <services/mycommand.h>

LOG_MODULE_REGISTER(app_ble_mycommand_service);

// Storage for the characteristic value.
// Let's make it a simple 1-byte value for demonstration.
static uint8_t command_value = 0;

// Read callback function for the Command Characteristic
static ssize_t read_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
				   void *buf, uint16_t len, uint16_t offset)
{
	// Get the pointer to the characteristic value from the attribute's user_data
	const uint8_t *value = attr->user_data;

	// Use bt_gatt_attr_read to handle offset and length checks for reading
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(command_value));
}

// Write callback function for the Command Characteristic
static ssize_t write_mycommand(struct bt_conn *conn, const struct bt_gatt_attr *attr,
					const void *buf, uint16_t len, uint16_t offset,
					uint8_t flags)
{
	// Get the pointer to the characteristic value from the attribute's user_data
	uint8_t *value = attr->user_data;

	// Validate the length of the write data
	if (len != sizeof(command_value)) {
		LOG_WRN("Invalid write length for command characteristic (%d vs %d expected)", len, sizeof(command_value));
		// Return the GATT error for invalid attribute length
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	// Validate the offset
	if (offset > 0) {
		LOG_WRN("Invalid write offset for command characteristic (%d)", offset);
		// Return the GATT error for invalid offset
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	// Copy the data from the write buffer into our characteristic value storage
	memcpy(value, buf, sizeof(command_value));

	LOG_INF("Command characteristic written. New value: 0x%02x", *value);

	// At this point, you would typically process the command_value.
	// For example, you could use a switch statement to act on different command values:
	// switch (*value) {
	//     case 0x01:
	//         LOG_INF("Received command: Start something!");
	//         // Add your code to start an action
	//         break;
	//     case 0x02:
	//         LOG_INF("Received command: Stop something!");
	//         // Add your code to stop an action
	//         break;
	//     default:
	//         LOG_WRN("Received unknown command: 0x%02x", *value);
	//         break;
	// }

	// Return the number of bytes successfully written
	return len;
}


// Define the new GATT Service and its Characteristics
// This uses the Zephyr BT_GATT_SERVICE_DEFINE macro which automatically registers the service.
BT_GATT_SERVICE_DEFINE(mycommand_service,
	// Declare the Primary Service using the new Service UUID
	BT_GATT_PRIMARY_SERVICE(BT_UUID_MYCOMMAND_SERVICE),

	// Declare the Command Characteristic:
	// - Uses the new Characteristic UUID
	// - Properties: Read and Write (BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE)
	// - Permissions: Read and Write (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
	// - Read callback function: read_mycommand
	// - Write callback function: write_mycommand
	// - User data: Pointer to the storage for the characteristic value (&command_value)
	BT_GATT_CHARACTERISTIC(BT_UUID_MYCOMMAND_CHAR,
						   BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
						   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
						   read_mycommand, write_mycommand, &command_value),

	// Note: No BT_GATT_CCC is included here because the characteristic
	// should not support notifications or indications.
);

// If the main application needed to read the current value of the command characteristic,
// you could add a getter function here and declare it in the header file.
/*
uint8_t get_mycommand_value(void)
{
    return command_value;
}
*/