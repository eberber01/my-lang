DOCKER_IMAGE=eberber01/riscv32-compiler-ci
MOUNT_DIR=my-lang
BUILD_DIR=build

release: 
	cmake -B $(BUILD_DIR) -DRV32=1 -DCMAKE_BUILD_TYPE=Release

debug: 
	cmake -B $(BUILD_DIR) -DRV32=1 -DCMAKE_BUILD_TYPE=Debug

build:
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

dev: image
	docker run -it \
  -v $(PWD):/$(MOUNT_DIR) \
  -w /$(MOUNT_DIR) \
  $(DOCKER_IMAGE)

image:
	docker build -t $(DOCKER_IMAGE) .