# Compile https://github.com/igrr/mkspiffs
mkspiffs -c ./firmware/tictactoe/data --size 2072576 --page 256 --block 8192 -- spiffs.bin

# Install via `brew install esptool`
esptool.py --chip esp8266 --port /dev/cu.usbserial-1410 --baud 460800 --before default_reset --after hard_reset write_flash 0x200000 spiffs.bin

# Configured from parameters shown in Arduino IDE:
#
# data    : /Users/duane/Dropbox/tictactoe/firmware/tictactoe/data
# size    : 2024
# page    : 256
# block   : 8192
# /player.txt
# upload  : /var/folders/bl/vmhly6rn6kbd7hphg614m4b80000gn/T/arduino_build_276330/tictactoe.spiffs.bin
# address  : 0x200000
# reset    : --before default_reset --after hard_reset
# port     : /dev/cu.usbserial-1410
# speed    : 921600
# python   : python
# uploader : /Users/duane/Library/Arduino15/packages/esp8266/hardware/esp8266/2.6.1/tools/upload.py
#
# Notes:
#  - `size` is 2024kb = 2072576
#    - note at https://github.com/esp8266/arduino-esp8266fs-plugin/issues/51 says size should be 2076672
#    - I'm using 2072576 because = 2024*1024
#  - although speed is set to 921600, Arduino Makefile says 460800 (see https://www.perturb.org/code/Arduino-Makefile)
#  - the 0x200000 address comes from the fact that we are programming a WeMos ESP8266 and 2MB of its 4MB flash is reserved for filesystem
