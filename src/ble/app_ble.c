#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>

#include <app_ble.h>
#include <services/mysensor.h>

LOG_MODULE_REGISTER(app_ble);

/* ============== BASIC BLE ============== */
struct bt_conn *current_conn = NULL;

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)), // BT_LE_AD_NO_BREDR means cannot use Bluetooth Classic
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* ============== UPDATE PARAMS ============== */
static struct bt_gatt_exchange_params exchange_params;
static void exchange_func(struct bt_conn *conn, uint8_t att_err, struct bt_gatt_exchange_params *params);

static void update_phy(struct bt_conn *conn)
{
	int err;
	const struct bt_conn_le_phy_param preferred_phy = {
		.options = BT_CONN_LE_PHY_OPT_NONE,
		.pref_rx_phy = BT_GAP_LE_PHY_2M,
		.pref_tx_phy = BT_GAP_LE_PHY_2M,
	};
    #ifndef CONFIG_BOARD_ESP32S3_DEVKITC
	err = bt_conn_le_phy_update(conn, &preferred_phy);
    #endif // CONFIG_BOARD_ESP32S3_DEVKITC
	if (err) {
		LOG_ERR("bt_conn_le_phy_update() returned %d", err);
	}
}

static void update_data_length(struct bt_conn *conn)
{
	int err;
	struct bt_conn_le_data_len_param my_data_len = {
		.tx_max_len = BT_GAP_DATA_LEN_MAX,
		.tx_max_time = BT_GAP_DATA_TIME_MAX,
	};
    #ifndef CONFIG_BOARD_ESP32S3_DEVKITC
	err = bt_conn_le_data_len_update(current_conn, &my_data_len);
    #endif // CONFIG_BOARD_ESP32S3_DEVKITC
	if (err) {
		LOG_ERR("data_len_update failed (err %d)", err);
	}
}

static void update_mtu(struct bt_conn *conn)
{
	int err;
	exchange_params.func = exchange_func;

	err = bt_gatt_exchange_mtu(conn, &exchange_params);
	if (err) {
		LOG_ERR("bt_gatt_exchange_mtu failed (err %d)", err);
	}
}

static void exchange_func(struct bt_conn *conn, uint8_t att_err, struct bt_gatt_exchange_params *params)
{
    LOG_INF("MTU exchange %s", att_err == 0 ? "successful" : "failed");
    if (!att_err) {
        uint16_t payload_mtu = bt_gatt_get_mtu(conn) - 3;   // 3 bytes used for Attribute headers.
        LOG_INF("New MTU: %d bytes", payload_mtu);
    }
}

/* ============== CALLBACKS ============== */
void on_connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_ERR("Connection error %d", err);
		return;
	}
	LOG_INF("Connected");
	current_conn = bt_conn_ref(conn);
	/* STEP 1.1 - Declare a structure to store the connection parameters */
	struct bt_conn_info info;
	err = bt_conn_get_info(conn, &info);
	if (err) {
		LOG_ERR("bt_conn_get_info() returned %d", err);
		return;
	}
	/* STEP 1.2 - Add the connection parameters to your log */
	double connection_interval = info.le.interval*1.25; // in ms
	uint16_t supervision_timeout = info.le.timeout*10; // in ms
	LOG_INF("Connection parameters: interval %.2f ms, latency %d intervals, timeout %d ms", connection_interval, info.le.latency, supervision_timeout);
	/* STEP 7.2 - Update the PHY mode */
	update_phy(current_conn);
	/* STEP 13.5 - Update the data length and MTU */
	k_sleep(K_MSEC(1000));  // Delay added to avoid link layer collisions.
	update_data_length(current_conn);
	update_mtu(current_conn);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected. Reason %d", reason);
	bt_conn_unref(current_conn);
}

void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
	double connection_interval = interval*1.25;         // in ms
	uint16_t supervision_timeout = timeout*10;          // in ms
	LOG_INF("Connection parameters updated: interval %.2f ms, latency %d intervals, timeout %d ms", connection_interval, latency, supervision_timeout);
}

void on_le_phy_updated(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
	// PHY Updated
	if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_1M) {
		LOG_INF("PHY updated. New PHY: 1M");
	}
	else if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_2M) {
		LOG_INF("PHY updated. New PHY: 2M");
	}
	else if (param->tx_phy == BT_CONN_LE_TX_POWER_PHY_CODED_S8) {
		LOG_INF("PHY updated. New PHY: Long Range");
	}
}

void on_le_data_len_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info)
{
	uint16_t tx_len     = info->tx_max_len; 
	uint16_t tx_time    = info->tx_max_time;
	uint16_t rx_len     = info->rx_max_len;
	uint16_t rx_time    = info->rx_max_time;
	LOG_INF("Data length updated. Length %d/%d bytes, time %d/%d us", tx_len, rx_len, tx_time, rx_time);
}

struct bt_conn_cb connection_callbacks = {
    .connected = on_connected,
    .disconnected = on_disconnected,
    .le_param_updated   = on_le_param_updated,
    #ifndef CONFIG_BOARD_ESP32S3_DEVKITC
	.le_phy_updated     = on_le_phy_updated,
	.le_data_len_updated    = on_le_data_len_updated,
    #endif // CONFIG_BOARD_ESP32S3_DEVKITC
};

/* ============== EXTERNAL INTERFACES ============== */
int app_ble_init(void)
{
    int err;

    err = bt_conn_cb_register(&connection_callbacks);
    if (err) {
        LOG_ERR("Failed to register connection callbacks (err %d)", err);
        return err;
    }

    err = bt_enable(NULL); // NULL is for bluetooth ready callback
    if (err)
    {
        LOG_ERR("Bluetooth init failed. Err: %i", err);
        return err;
    }

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0); // advertise so that the client or mobile app can find this peripheral
    if (err)
    {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }

    LOG_INF("Bluetooth initialization complete!");
    return 0;
}

struct bt_conn *app_ble_get_connection(void)
{
    return current_conn;
}

int app_ble_mysensor_data_send(const uint8_t *data, const uint16_t len)
{
    struct bt_conn *current_conn = app_ble_get_connection();
    bt_mysensor_notify(current_conn, data, len);
}