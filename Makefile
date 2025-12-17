DOCKER_IMAGE=eberber01/riscv32-compiler-ci
MOUNT_DIR=my-lang

dev: image
	docker run -it \
  -v $(PWD):/$(MOUNT_DIR) \
  -w /$(MOUNT_DIR) \
  $(DOCKER_IMAGE)

image:
	docker build -t $(DOCKER_IMAGE) .