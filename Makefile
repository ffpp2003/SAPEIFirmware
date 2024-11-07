# Configuración
BOARD_TYPE = arduino:avr:nano
PORT = /dev/ttyUSB0
BUILD_DIR = build

# Archivo fuente principal
SRC_FILE = $(wildcard *.ino)

# Target para compilar el programa
build:
	arduino-cli compile --fqbn $(BOARD_TYPE) --output-dir $(BUILD_DIR) $(SRC_FILE)

# Target para cargar el programa en el Arduino Nano
upload: build
	arduino-cli upload --fqbn $(BOARD_TYPE) -p $(PORT) -i $(BUILD_DIR)/$(SRC_FILE).hex

# Target para limpiar los archivos de compilación
clean:
	rm -rf $(BUILD_DIR)

