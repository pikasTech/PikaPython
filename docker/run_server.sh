DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker run -itd --name $DOCKER_NAME \
--restart always \
-p 8080:80 \
-w /root/pikascript \
$DOCKER_NAME \
code-server --port 80 --host 0.0.0.0 --auth password

