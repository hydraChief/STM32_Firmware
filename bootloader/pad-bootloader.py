BOOTLOADER_SIZE= 0x8000
BOOTLOADER_FILE_NAME='bootloader.bin'

with open(BOOTLOADER_FILE_NAME, 'rb') as f:
    bootloader_data = f.read()

padding=bytes([0xff for _ in range(BOOTLOADER_SIZE - len(bootloader_data))])

with open(BOOTLOADER_FILE_NAME, 'wb') as f:
    f.write(bootloader_data + padding)