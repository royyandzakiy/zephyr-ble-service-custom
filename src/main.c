// main.c
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#include <services/mysensor.h>
#include <app_ble.h>

int data_global = 0;

void publish_data() {
    ++data_global;
    app_ble_publish(&data_global);
}

int main(void)
{
	int err = 0;

    printk("Hello World from printk!\n");
    LOG_INF("ADC Sensor BLE Peripheral");

    /* Setup BLE */
    err = app_ble_init();
	if (err)
        __ASSERT_MSG_INFO("Unable to init bluetooth library. Err: %i", err);

    while (true) {
        LOG_INF("Sending data %d", data_global);
        publish_data();
        k_msleep(1000);
    }

	return 0;
}