# Zephyr Custom BLE Service

This project is meant to learn on creating custom BLE services

What has been learned:
- `mysensor.h`:
    - Generated custom UUID, service to characteristics should have small difference for ease of identification
    - Macros are heavily used to expand code in a neat way
- `mysensor.c`:
    - One can change the characteristic properties by changing `_props` in `BT_GATT_CHARACTERISTIC(_uuid,_props,_perm,_read,_write,_user_data)`
    - To do a bluetooth Notify, one can use either `bt_gatt_notify_cb` which has a `bt_gatt_notify` inside