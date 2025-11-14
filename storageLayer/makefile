CC      = cc
CFLAGS  = -Wall -Wextra -std=c99 -g -Iheaders
TARGET  = storageLayer
OBJ_DIR = build
SRC_DIRS = . tcpClient headers proto

SOURCES := $(sort $(shell find $(SRC_DIRS) -type f -name '*.c' | sed 's|^\./||'))
OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SOURCES))

LIBS = -lprotobuf-c -lmicrohttpd -lpthread
$(TARGET): $(OBJECTS)
	@echo "🔧 Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "✅ Build complete → $(TARGET)"
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "🧩 Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all run clean
