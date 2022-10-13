DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -it --name $DOCKER_NAME \
--restart always \
-p 8080:8080 \
-w /root/pikascript \
$DOCKER_NAME \
bash
