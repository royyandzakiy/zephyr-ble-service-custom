# Zephyr BLE Service (Notify, Read, Write)

## Installing Pre-Requisites
- ...

## Build, Run, Monitor
```bash
set ZEPHYR_BASE=%HOMEPATH%\zephyrproject\zephyr & call "%HOMEPATH%\zephyrproject\.venv\Scripts\activate.bat" & west zephyr-export

# ESP32S3 Devkit-C
west build -b esp32s3_devkitc/esp32s3/procpu -p always -d build_esp32s3_devkitc && west flash --build-dir build_esp32s3_devkitc --esp-device COM6 && python -m serial.tools.miniterm COM6 115200

# Xiao Sense
west build -b xiao_ble -p always -d build_xiao
python -m serial.tools.miniterm COM38 115200

# ESP32 Devkit-C Wroom
west build -b esp32_devkitc_wroom/esp32/procpu -p always -d build_esp32_devkitc && west flash --build-dir build_esp32_devkitc --esp-device COM3 && python -m serial.tools.miniterm COM3 115200
```

## Development Notes
- [ ] Add Installing Preprequisites
- [ ] Creating into a Dockerfile