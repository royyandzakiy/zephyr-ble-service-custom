#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gatt.h>
LOG_MODULE_REGISTER(app_ble);

/* Local */
#include <app_ble.h>
#include <services/mysensor.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)), // BT_LE_AD_NO_BREDR means cannot use Bluetooth Classic
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Failed to connect (err %u)", err);
        return;
    }

    LOG_INF("Connected");
    current_conn = bt_conn_ref(conn);

    // Request the shortest possible connection interval for highest throughput
    struct bt_le_conn_param param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MIN, // Minimum allowed connection interval (7.5 ms)
        .interval_max = BT_GAP_INIT_CONN_INT_MIN,
        .latency = 0,             // No slave latency
        .timeout = 400,           // Supervision timeout (400 * 10 ms = 4 seconds)
    };
    struct bt_conn_le_phy_param phy_pref = {
        .options = BT_CONN_LE_PHY_OPT_NONE,
        .pref_tx_phy = BT_GAP_LE_PHY_2M,
        .pref_rx_phy = BT_GAP_LE_PHY_2M,
    };

    struct bt_conn_le_data_len_param data_len_param = {
        .tx_max_len = BT_GAP_DATA_LEN_MAX, // Maximum Link Layer transmission payload size in bytes
        .tx_max_time = BT_GAP_DATA_TIME_MAX // Maximum Link Layer transmission payload time in us
    };

    int err_conn_param = bt_conn_le_param_update(current_conn, &param);
    if (err_conn_param) {
        LOG_WRN("Failed to update connection parameters (err %d)", err_conn_param);
    } else {
        LOG_INF("Connection parameters update requested (min/max interval: %d slots, latency: %d, timeout: %d)",
                param.interval_min, param.interval_max, param.latency, param.timeout);
    }

    int err_phy = bt_conn_le_phy_update(current_conn, &phy_pref);
    if (err_phy) {
        LOG_WRN("Failed to update PHY to 2M (err %d)", err_phy);
    } else {
        LOG_INF("PHY update to 2M requested.");
    }

    int err_dle = bt_conn_le_data_len_update(current_conn, &data_len_param);
    if (err_dle) {
        LOG_WRN("Failed to update data length extension (err %d)", err_dle);
    } else {
        LOG_INF("Data length extension update requested.");
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected (reason %u)", reason);
    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }
}

struct bt_conn_cb connection_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

int app_ble_init(void)
{
    int err;

    err = bt_conn_cb_register(&connection_callbacks);
    if (err) {
        LOG_ERR("Failed to register connection callbacks (err %d)", err);
        return err;
    }

    /* Enable BLE peripheral */
    err = bt_enable(NULL); // NULL is for bluetooth ready callback
    if (err)
    {
        LOG_ERR("Bluetooth init failed. Err: %i", err);
        return err;
    }

    if (IS_ENABLED(CONFIG_SETTINGS))
    {
        settings_load();
    }

    /* Start advertising */
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