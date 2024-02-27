SHELL := /usr/bin/env bash
# Get docker container information
# Date for log files
LOGDATE := $(shell date +%F-%H%M)

# pull the name from the docker file - these labels *MUST* be set
CONTAINER_PROJECT ?= $(shell grep org.opencontainers.image.vendor Dockerfile | cut -d = -f2 |  tr -d '"\\ ')
CONTAINER_NAME ?= $(shell grep org.opencontainers.image.ref.name Dockerfile  | cut -d = -f2 |  tr -d '"\\ ')
CONTAINER_TAG ?= $(shell grep org.opencontainers.image.version Dockerfile    | cut -d = -f2 |  tr -d '"\\ ')
CONTAINER_STRING ?= $(CONTAINER_PROJECT)/$(CONTAINER_NAME):$(CONTAINER_TAG)

C_ID = $(shell ${GET_ID})
C_IMAGES = $(shell ${GET_IMAGES})

define run_hadolint
	@echo ''
	@echo '> Dockerfile$(1) ==========='
	docker run --rm -i \
	-e HADOLINT_FAILURE_THRESHOLD=error \
	-e HADOLINT_IGNORE=DL3042,DL3008,DL3015,DL3048 \
	hadolint/hadolint < Dockerfile$(1)
endef

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
	$(call run_hadolint)
	git pull --recurse-submodules;\
	mkdir -vp source/logs/ ; \
	DOCKER_BUILDKIT=1 \
	docker build \
		--cache-from $(CONTAINER_STRING) \
		-t $(CONTAINER_STRING) \
		--label oci.opencontainers.image.created=$(shell date +%F-%H%M) \
		--progress plain \
		-f Dockerfile . \
    | tee source/logs/build-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log && \
	docker inspect $(CONTAINER_STRING) > source/logs/inspect-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log

docker-multi: ## Multi-platform build.
	$(call run_hadolint)
	mkdir -vp  source/logs/ ; \
	docker buildx build --platform linux/amd64,linux/arm64/v8 . \
		--cache-from $(CONTAINER_STRING) \
		-t $(CONTAINER_STRING) \
		--label BUILDDATE=$(shell date +%F-%H%M) \
		--progress plain \
		--push 2>&1 \
	| tee source/logs/build-multi-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(LOGDATE).log

setup-multi: ## setup docker multiplatform
	docker buildx create --name buildx-multi-arch ; docker buildx use buildx-multi-arch

run-docker: ## launch shell into the container, with this directory mounted to /opt/source
	docker run \
		--rm \
		-it \
		--name puzzlebox \
		--user root \
		--entrypoint /bin/bash \
		-v $(shell pwd):/opt/source \
		$(CONTAINER_STRING)
docker-lint: ## Check files for errors
	$(call run_hadolint)

# Commands for extracting information on the running container
GET_IMAGES := docker images ${CONTAINER_STRING} --format "{{.ID}}"
GET_CONTAINER := docker ps -a --filter "name=${CONTAINER_NAME}" --no-trunc
GET_ID := ${GET_CONTAINER} --format {{.ID}}
GET_STATUS := ${GET_CONTAINER} --format {{.Status}} | cut -d " " -f1
