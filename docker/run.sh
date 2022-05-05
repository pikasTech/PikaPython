DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -it --name $DOCKER_NAME \
-w /root/pikascript \
$DOCKER_NAME \
bash
