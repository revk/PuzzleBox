# Get docker container information
CONTAINER_PROJECT=$(shell grep LABEL Dockerfile | grep PROJECT | cut -d = -f2 | tr -d '"')
CONTAINER_NAME=$(shell grep LABEL Dockerfile | grep NAME | cut -d = -f2 | tr -d '"')
CONTAINER_TAG=$(shell grep LABEL Dockerfile | grep VERSION | cut -d = -f2| tr -d '"')
CONTAINER_STRING=$(CONTAINER_PROJECT)/$(CONTAINER_NAME):$(CONTAINER_TAG)

# HELP
# https://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help
help: ## This help.
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-25s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

.DEFAULT_GOAL := help

puzzlebox: puzzlebox.c ## Build the puzzlebox binary
ifeq ($(shell uname),Darwin)
	/usr/local/opt/gcc/bin/gcc-8 -L/usr/local/lib -I/usr/local/include -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
else
	cc -O -o $@ $< -lpopt -lm -g -D_GNU_SOURCE
endif

sif: ## Build the container
	mkdir -vp  source/logs/ ; \
	apptainer build \
		-F \
		/tmp/PuzzleBox.sif \
		PuzzleBox.def \
	| tee source/logs/apptainer-build-$(shell date +%F-%H%M).log

docker: ## Build the container
	mkdir -vp  source/logs/ ; \
	docker build \
		-t $(CONTAINER_STRING) \
		--label BUILDDATE=$(shell date +%F-%H%M) \
		--progress plain \
		-f Dockerfile . \
    | tee source/logs/build-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log && \
	docker inspect $(CONTAINER_STRING) > source/logs/inspect-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log

docker-multi: ## Build multiplatform
	mkdir -vp  source/logs/ ; \
	docker buildx build --no-cache --platform linux/amd64,linux/arm64/v8 \
		-t $(CONTAINER_STRING) \
		--label BUILDDATE=$(shell date +%F-%H%M) \
		--progress plain \
		-f Dockerfile .

run-docker: ## launch shell into the container, with this directory mounted to /opt/source
	docker run \
		--rm \
		-it \
		--name puzzlebox \
		--user root \
		--entrypoint /bin/bash \
		-v $(shell pwd):/opt/source \
		$(CONTAINER_STRING)
