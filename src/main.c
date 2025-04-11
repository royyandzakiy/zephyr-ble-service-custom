#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/timeutil.h>

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
        if (conn) {
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
            if (ret) {
                LOG_ERR("Failed to send MySensor data (err %d)", ret);
            }

            sample_index = 0;

            // k_usleep(1); // Adjust as needed, experiment with small values
            k_sleep(K_MSEC(1000)); // Adjust as needed, experiment with small values
        } else {
            LOG_WRN("Not connected, waiting...");
            k_sleep(K_MSEC(1000)); // Wait if not connected
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

    sinewave_send_loop();
}