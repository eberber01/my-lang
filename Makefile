DOCKER_IMAGE=eberber01/riscv32-compiler-ci
MOUNT_DIR=my-lang
BUILD_DIR=build

.PHONY: release debug build format clean dev image

release: 
	cmake -S . -B $(BUILD_DIR) -DRV32=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

debug: 
	cmake -S . -B $(BUILD_DIR) -DRV32=1 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build:
	cmake --build $(BUILD_DIR)

format:
	cmake --build build --target format

clean:
	rm -rf $(BUILD_DIR)

dev: image
	docker run -it \
  -v $(PWD):/$(MOUNT_DIR) \
  -w /$(MOUNT_DIR) \
  $(DOCKER_IMAGE)

image:
	docker build -t $(DOCKER_IMAGE) .