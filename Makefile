CFLAGS = -std=gnu99 -O3 -Werror -v
BUILD_DIR = build
OUTPUT_OPTION = -o $(BUILD_DIR)/$@
OUTPUT_OPTION_BIN = -o bin/$@

all: mkdir neoid
	@echo;echo "======== Success! ========";echo;

clean: 
	@rm -rf $(BUILD_DIR)

mkdir:
	@mkdir -p $(BUILD_DIR)/bin; mkdir -p $(BUILD_DIR)/src;

neoid: src/neoid.o src/nid_event.o src/nid_genid.o src/nid_server.o
	cd $(BUILD_DIR); $(CC) $(CFLAGS) $? $(OUTPUT_OPTION_BIN)
