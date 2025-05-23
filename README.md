# Zephyr BLE Service (Notify, Read, Write)

## Installing Pre-Requisites
- ...

## Build, Run, Monitor
```bash
set ZEPHYR_BASE=%HOMEPATH%\zephyrproject\zephyr & call "%HOMEPATH%\zephyrproject\.venv\Scripts\activate.bat" & west zephyr-export

# ESP32S3 Devkit-C
west build -b esp32s3_devkitc/esp32s3/procpu -p always -d build_esp32s3_devkitc && west flash --build-dir build_esp32s3_devkitc --esp-device COM6 && python -m serial.tools.miniterm COM6 115200
west build -b esp32s3_devkitc/esp32s3/procpu           -d build_esp32s3_devkitc && west flash --build-dir build_esp32s3_devkitc --esp-device COM6 && python -m serial.tools.miniterm COM6 115200

# Xiao Sense
# west build -b xiao_ble -p always -d build_xiao # then copy build_xiao/zephyr/zephyr.uf2
west build -b xiao_ble/nrf52840/sense -p always -d build_xiao -- -DCONFIG_BOOT_DELAY=5000 # tap boot 2x, then copy build_xiao/zephyr/zephyr.uf2 to usb
python -m serial.tools.miniterm COM38 115200

# Nice!Nano Pro Micro NRF52840
west build -b promicro_nrf52840/nrf52840/uf2 -d build_promicro && west flash --build-dir build_promicro --port COM6 && west monitor --port COM40 --baud 115200

# ESP32 Devkit-C Wroom
west build -b esp32_devkitc_wroom/esp32/procpu -p always -d build_esp32_devkitc && west flash --build-dir build_esp32_devkitc --esp-device COM3 && python -m serial.tools.miniterm COM3 115200
```

## Development Notes
- [ ] Add Installing Preprequisites
- [ ] Creating into a Dockerfile