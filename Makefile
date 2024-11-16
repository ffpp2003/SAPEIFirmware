# Configuración
BOARD_TYPE = arduino:avr:nano
PORT = /dev/ttyUSB0
BUILD_DIR = build
BAUD = 115200

# Archivo fuente principal
SRC_FILE = $(wildcard *.ino)

# Archivos en src/SAPEICore excepto Error.h
FILES_TO_RENAME = $(filter-out src/SAPEICore/Error.h, $(wildcard src/SAPEICore/*))

# Target por defecto
all: rename_files build restore_files

# Renombrar archivos (agregar ".off")
rename_files:
	@for file in $(FILES_TO_RENAME); do \
		mv $$file $$file.off; \
	done

# Restaurar archivos a sus nombres originales
restore_files:
	@for file in $(FILES_TO_RENAME); do \
		mv $$file.off $$file; \
	done

# Compilar el programa
build:
	arduino-cli compile --fqbn $(BOARD_TYPE) --output-dir $(BUILD_DIR) $(SRC_FILE)

# Cargar el programa en el Arduino Nano
upload: build
	arduino-cli upload --fqbn $(BOARD_TYPE) -p $(PORT) -i $(BUILD_DIR)/$(SRC_FILE).hex


monitor:
	arduino-cli monitor -p $(PORT) --config $(BAUD)

# Limpiar los archivos de compilación
clean:
	rm -rf $(BUILD_DIR)

