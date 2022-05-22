DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -it --name $DOCKER_NAME \
--restart always \
-w /root/pikascript \
$DOCKER_NAME \
bash
