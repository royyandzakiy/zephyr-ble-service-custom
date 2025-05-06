# Zephyr BLE Service (Notify, Read, Write)

## Build, Run, Monitor
```bash
set ZEPHYR_BASE=%HOMEPATH%\zephyrproject\zephyr & call "%HOMEPATH%\zephyrproject\.venv\Scripts\activate.bat" & west zephyr-export
west build -b esp32s3_devkitc/esp32s3/procpu -p always -d build_esp32s3_devkitc && west flash --build-dir build_esp32s3_devkitc --esp-device COM6
python -m serial.tools.miniterm COM6 115200
```