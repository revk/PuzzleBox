SHELL := /usr/bin/env bash
# Get docker container information
DOCKER_REPO ?= docker.io
DOCKER_BIN := $(shell type -p docker || type -p nerdctl || type -p nerdctl.lima || exit)
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
	$(DOCKER_BIN) run --rm -i \
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

envs: ## show the environments
	$(shell echo -e "${CONTAINER_STRING}\n\t${CONTAINER_PROJECT}\n\t${CONTAINER_NAME}\n\t${CONTAINER_TAG}")

sif: ## Build the container
	mkdir -vp  source/logs/ ; \
	apptainer build \
		-F \
		/tmp/PuzzleBox.sif \
		PuzzleBox.def \
	| tee source/logs/apptainer-build-$(shell date +%F-%H%M).log

docker: ## Build the docker image locally.
	$(call run_hadolint)
	git pull --recurse-submodules;\
	mkdir -vp source/logs/ ; \
	DOCKER_BUILDKIT=1 \
	$(DOCKER_BIN) build \
		-t $(CONTAINER_STRING) \
		--cache-from $(CONTAINER_STRING) \
		--progress plain \
		--label org.opencontainers.image.created=$(shell date +%F-%H%M) 2>&1 \
		-f Dockerfile . \
    | tee source/logs/build-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log && \
	$(DOCKER_BIN) inspect $(CONTAINER_STRING) > source/logs/inspect-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(shell date +%F-%H%M).log

setup-multi: ## setup $(DOCKER_BIN) multiplatform
	$(DOCKER_BIN) buildx create --name buildx-multi-arch ; $(DOCKER_BIN) buildx use buildx-multi-arch

docker-multi: ## Multi-platform build.
	$(call setup-multi)
	$(call run_hadolint)
	git pull --recurse-submodules; \
	mkdir -vp  source/logs/ ; \
	$(DOCKER_BIN) build --platform linux/amd64,linux/arm64/v8 . \
		--cache-from $(CONTAINER_STRING) \
		-t $(CONTAINER_STRING) \
		--label org.opencontainers.image.created=$(shell date +%F-%H%M) 2>&1 \
		--progress plain \
		--push 2>&1 \
	| tee source/logs/build-multi-$(CONTAINER_PROJECT)-$(CONTAINER_NAME)_$(CONTAINER_TAG)-$(LOGDATE).log


run: ## launch shell into the container, with this directory mounted to /opt/source
	$(DOCKER_BIN) run \
		--rm \
		-it \
		--name puzzlebox \
		--user root \
		--entrypoint /bin/bash \
		-v $(shell pwd):/opt/source \
		$(CONTAINER_STRING)
pull: ## Pull Docker image
	@echo 'pulling $(CONTAINER_STRING)'
	$(DOCKER_BIN) pull $(CONTAINER_STRING)

publish: ## Push server image to remote
	[ "${C_IMAGES}" ] || \
		make docker
	@echo 'pushing $(CONTAINER_STRING) to $(DOCKER_REPO)'
	$(DOCKER_BIN) push $(CONTAINER_STRING)


docker-lint: ## Check files for errors
	$(call run_hadolint)

# Commands for extracting information on the running container
_IMAGES := $(DOCKER_BIN) images ${CONTAINER_STRING} --format "{{.ID}}"
GET_CONTAINER := $(DOCKER_BIN) ps -a --filter "name=${CONTAINER_NAME}" --no-trunc
GET_ID := ${GET_CONTAINER} --format {{.ID}}
GET_STATUS := ${GET_CONTAINER} --format {{.Status}} | cut -d " " -f1
