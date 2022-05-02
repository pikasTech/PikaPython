DOCKER_NAME=pikadev

docker rmi $DOCKER_NAME

cd .. && docker build -t $DOCKER_NAME .
