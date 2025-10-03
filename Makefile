# Anim4DC - Dreamcast Raylib Animation Plugin
# Main project Makefile

.PHONY: all clean fox_demo basic_example help

# Default target
all: fox_demo

# Help target
help:
	@echo "Anim4DC - Dreamcast Raylib Animation Plugin"
	@echo ""
	@echo "Available targets:"
	@echo "  fox_demo      - Build the complete Fox animation demo"
	@echo "  fox_demo_cdi  - Build Fox demo and create CDI for hardware/emulator"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make fox_demo     # Build Fox demo ELF"
	@echo "  make fox_demo_cdi # Build and create CDI"
	@echo "  make clean        # Clean everything"

# Fox demo targets
fox_demo:
	@echo "Building Anim4DC Fox Demo..."
	cd examples/fox_demo && $(MAKE) clean && $(MAKE)
	@echo "Fox demo built successfully!"
	@echo "ELF location: examples/fox_demo/fox_demo.elf"

fox_demo_cdi: fox_demo
	@echo "Creating CDI for Fox demo..."
	cd examples/fox_demo && $(MAKE) cdi MKDCDISC="$(MKDCDISC)"
	@echo "CDI created successfully!"
	@echo "CDI location: examples/fox_demo/fox_demo.cdi"

# Clean all projects
clean:
	@echo "Cleaning Anim4DC projects..."
	cd examples/fox_demo && $(MAKE) clean
	@echo "Clean complete!"

# Install target (copy header to KOS addons system)
install:
	@echo "Installing Anim4DC header..."
	@if [ ! -d "$(KOS_BASE)/addons/include/anim4dc" ]; then \
		mkdir -p $(KOS_BASE)/addons/include/anim4dc; \
	fi
	cp include/anim4dc.h $(KOS_BASE)/addons/include/anim4dc/
	@echo "Anim4DC header installed to $(KOS_BASE)/addons/include/anim4dc/"

# Test target (run in Flycast if available)
test: fox_demo_cdi
	@echo "Testing Fox demo in Flycast..."
	@if command -v flycast >/dev/null 2>&1; then \
		flycast examples/fox_demo/fox_demo.cdi; \
	else \
		echo "Flycast not found. Please run examples/fox_demo/fox_demo.cdi manually."; \
	fi