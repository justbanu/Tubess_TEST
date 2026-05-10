# --- Variabel ---
CC = gcc
# -Wall untuk nampilin semua warning, -Iheader untuk ngasih tau gcc cari file .h di folder header/
CFLAGS = -Wall -Wextra -Iheader
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/alprog1

# Otomatis ngambil semua file .c di dalam folder src/
SRC = $(wildcard $(SRC_DIR)/*.c)

# --- Target Commands ---

# Target default (kalau cuma ngetik 'make')
all: build

# Target build: Bikin folder build (kalau belum ada) lalu compile
build: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
	@echo "Build sukses! Executable ada di $(TARGET)"

# Target run: Otomatis build dulu (kalau ada perubahan), baru dijalanin
run: build
	@echo "Menjalankan program..."
	@echo "----------------------"
	./$(TARGET)

# Target clean: Hapus isi folder build
clean:
	rm -rf $(BUILD_DIR)/*
	@echo "Folder build sudah dibersihkan."