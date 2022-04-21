DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -it --name $DOCKER_NAME \
--restart=always \
-v $PWD/..:/usr/src/pikascript \
-w /usr/src/pikascript/port/linux \
$DOCKER_NAME \
bash
