DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -it --name $DOCKER_NAME \
-w /pikascript/port/linux \
$DOCKER_NAME \
bash
