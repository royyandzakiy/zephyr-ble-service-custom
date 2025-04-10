// main.c
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

#include <app_ble.h>
#include <services/mysensor.h>

LOG_MODULE_REGISTER(main);

/* ========= BLE Characteristic Notify Callers ========= */
void mysensor_notify() {
    static uint8_t mysensor_data = 100U;

    mysensor_data++;
    
    printk("mysensor_data sent through ble: %d\n", mysensor_data);
    bt_mysensor_notify(NULL, mysensor_data);
}

void mysensor2_notify() {
    static uint16_t mysensor2_data = 10U;

    mysensor2_data++;
    
    printk("mysensor2_data sent through ble: %d\n", mysensor2_data);
    bt_mysensor2_notify(mysensor2_data);
}

void mysensor3_notify() {
    static uint16_t mysensor3_data = 70U;

    mysensor3_data++;
    
    printk("mysensor3_data sent through ble: %d\n", mysensor3_data);
    bt_mysensor3_notify(mysensor3_data);
}

static void hrs_notify(void)
{
	static uint8_t heartrate = 90U;

	/* Heartrate measurements simulation */
	heartrate++;
	if (heartrate == 160U) {
		heartrate = 90U;
	}

    printk("heartrate sent through ble: %d\n", heartrate);
    bt_hrs_notify(heartrate);
}

/* ========= Main ========= */

int main(void)
{
	int err = 0;

    LOG_INF("MySensor BLE Peripheral");

    /* Setup BLE */
    err = app_ble_init();
	if (err)
        __ASSERT_MSG_INFO("Unable to init bluetooth library. Err: %i", err);

    while (true) {        
        mysensor_notify();
        mysensor2_notify();
        mysensor3_notify();
        hrs_notify();

        k_msleep(1000);
    }

	return 0;
}