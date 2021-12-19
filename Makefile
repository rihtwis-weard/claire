.PHONY: default dev dev-build

default: dev

dev:
	DOCKER_BUILDKIT=1 docker-compose --project-name claire_workspace --file .devcontainer/docker-compose.dev.yml up --detach

dev-build:
	DOCKER_BUILDKIT=1 docker-compose --project-name claire_workspace --file .devcontainer/docker-compose.dev.yml up --detach --build

