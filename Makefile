default: dev

deps:
	conan install --install-folder cmake-build-debug/ . --update

dev:
	DOCKER_BUILDKIT=1 docker-compose --project-name claire \
 		--file .devcontainer/docker-compose.clion.yml up --detach
	ssh-keygen -f "${HOME}/.ssh/known_hosts" -R "[localhost]:2222"

dev-build:
	DOCKER_BUILDKIT=1 docker build -t claire/remote-cpp-env:0.1.0 -f .devcontainer/Dockerfile.clion .

dev-up:
	DOCKER_BUILDKIT=1 docker-compose --project-name claire
		--file .devcontainer/docker-compose.clion.yml up --force-recreate --build --detach
