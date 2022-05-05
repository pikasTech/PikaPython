DOCKER_NAME=pikadev

docker rmi $DOCKER_NAME

docker build -t $DOCKER_NAME .
