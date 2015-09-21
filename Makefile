CFLAGS = -std=gnu99 -O3 -Werror
OUTPUT_OPTION = -o build/$@
OUTPUT_OPTION_BIN = -o bin/$@

all: mkdir neoid
	@echo "======== Done! ========"

clean: 
	@rm -rf build

mkdir:
	@mkdir -p build/bin; mkdir -p build/src;

neoid: src/neoid.o src/nid_event.o src/nid_genid.o src/nid_server.o
	cd build; $(CC) $(CFLAGS) $? $(OUTPUT_OPTION_BIN)
