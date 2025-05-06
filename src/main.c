// main.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/timeutil.h>
#include <errno.h> // Include errno.h for error code definitions

#include <app_ble.h> // Include for BLE functionality
#include <services/mysensor.h>

LOG_MODULE_REGISTER(main);

/* ============== SINEWAVE GENERATOR ============== */
#define SAMPLING_RATE 10000U // Hz (adjust as needed, but be realistic about BLE limits)
#define PI 3.14159265358979323846
#define FREQUENCY 10.0f      // Hz (frequency of the sine wave)
#define AMPLITUDE 16383.0f   // Max value for a signed 16-bit integer

// Determine the maximum payload size based on the maximum ATT MTU (251 bytes)
// and accounting for the ATT header (3 bytes).
#define MAX_PAYLOAD_SIZE (251 - 3)
#define BYTES_PER_SAMPLE 2 // Assuming 16-bit ADC data

// Calculate the number of samples we can fit in one notification
#define SAMPLES_PER_NOTIFICATION (MAX_PAYLOAD_SIZE / BYTES_PER_SAMPLE)

uint32_t timestamp = 0;
int16_t sine_value;
uint8_t data_buffer[MAX_PAYLOAD_SIZE];
int sample_index = 0;

void sinewave_send_loop() {
    int ret;
    LOG_INF("Sine wave generator started");

    while (1) {
        struct bt_conn *conn = app_ble_get_connection();

        // Only attempt to send if connected AND notifications are enabled
        if (conn && bt_mysensor_are_notifications_enabled()) {
            // Generate sine wave data and send it as fast as possible
            for (int i = 0; i < SAMPLES_PER_NOTIFICATION; ++i) {
                float time = (float)timestamp / SAMPLING_RATE;
                sine_value = (int16_t)(AMPLITUDE * sinf(2.0f * PI * FREQUENCY * time));

                // Pack the 16-bit sample into the data buffer (little-endian)
                data_buffer[sample_index++] = (uint8_t)(sine_value & 0xFF);
                data_buffer[sample_index++] = (uint8_t)((sine_value >> 8) & 0xFF);

                timestamp++;
            }

            ret = app_ble_mysensor_data_send(data_buffer, sizeof(data_buffer));

            // Log errors, but ignore -ENOTCONN (not connected) and -EAGAIN (notifications not enabled)
            // The service function now handles the -EAGAIN check internally before calling bt_gatt_notify_cb,
            // but keeping the check here doesn't hurt and provides clarity.
            // The service function bt_mysensor_notify will return -ENOTCONN or -EAGAIN if not applicable.
            if (ret && ret != -ENOTCONN && ret != -EAGAIN) {
                // Convert negative Zephyr error to positive errno for clearer logging
                LOG_ERR("Failed to send MySensor data (err %d)", -ret);
            } else if (ret == 0) {
                // Optional: Log successful send (uncomment if desired)
                // LOG_INF("Sent sinewave data");
            }

            sample_index = 0;

            // k_usleep(1); // Adjust as needed, experiment with small values
            k_sleep(K_MSEC(1000)); // Adjust as needed, experiment with small values
        } else {
            // If not connected or notifications not enabled, just wait.
            // LOG_WRN("Waiting for connection or notifications to be enabled..."); // Optional log
            k_sleep(K_MSEC(1000)); // Wait if not connected or not subscribed
        }
    }
}

/* ============== COUNTER GENERATOR ============== */
static uint32_t tick_counter = 0;
static uint8_t counter_data_buffer[4]; // Buffer to hold a 32-bit counter

void counter_send_loop() {
    int ret;
    LOG_INF("Tick counter generator started");

    while (1) {
        struct bt_conn *conn = app_ble_get_connection();

        // Only attempt to send if connected AND notifications are enabled
        if (conn && bt_mysensor_are_notifications_enabled()) {
            // Increment the counter
            tick_counter++;

            // Prepare the data buffer with the current counter value (little-endian)
            counter_data_buffer[0] = (uint8_t)(tick_counter & 0xFF);
            counter_data_buffer[1] = (uint8_t)((tick_counter >> 8) & 0xFF);
            counter_data_buffer[2] = (uint8_t)((tick_counter >> 16) & 0xFF);
            counter_data_buffer[3] = (uint8_t)((tick_counter >> 24) & 0xFF);

            // Send the notification with the counter value
            ret = app_ble_mysensor_data_send(counter_data_buffer, sizeof(counter_data_buffer));

            // Log errors, but ignore -ENOTCONN (not connected) and -EAGAIN (notifications not enabled)
            // The service function now handles the -EAGAIN check internally before calling bt_gatt_notify_cb,
            // but keeping the check here doesn't hurt and provides clarity.
            // The service function bt_mysensor_notify will return -ENOTCONN or -EAGAIN if not applicable.
            if (ret && ret != -ENOTCONN && ret != -EAGAIN) {
                // Convert negative Zephyr error to positive errno for clearer logging
                LOG_ERR("Failed to send MySensor data (err %d)", -ret);
            } else if (ret == 0) {
                // Optional: Log successful send (uncomment if desired)
                // LOG_INF("Sent counter: %u", tick_counter);
            }

            // Wait for 1 second before sending the next value
            k_sleep(K_MSEC(1000));
        } else {
            // If not connected or notifications not enabled, just wait.
            // The service function now handles the connection and notification state check.
            // We can optionally add a log here if we want to see when it's waiting.
            // LOG_WRN("Waiting for connection or notifications to be enabled..."); // Optional log
            k_sleep(K_MSEC(1000)); // Wait if not connected or not subscribed
        }
    }
}

/* ============== MAIN ============== */
void main(void)
{
    int ret;

    ret = app_ble_init();
    if (ret) {
        LOG_ERR("BLE initialization failed (err %d)", ret);
        return;
    }

    // sinewave_send_loop(); // Uncomment if you want to use the sinewave generator

    counter_send_loop();
}