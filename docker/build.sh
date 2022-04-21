DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f
docker rmi $DOCKER_NAME

docker build -t $DOCKER_NAME .
docker run -it --name $DOCKER_NAME \
--restart=always \
-v $PWD/..:/usr/src/pikascript \
-w /usr/src/pikascript/port/linux \
$DOCKER_NAME \
bash
# sh pull-core.sh && sh init.sh && sh gtest.sh && sh run.sh && bash
