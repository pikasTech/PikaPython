#!/bin/bash

DOCKER_NAME=pikadev

docker rm $DOCKER_NAME -f

# 解析命令行参数
while getopts ":p:s:t:r:" opt; do
  case $opt in
    p) frpc_port=$OPTARG;;
    s) frpc_server_addr=$OPTARG;;
    t) frpc_token=$OPTARG;;
    r) frpc_remote_port=$OPTARG;;
    \?) echo "Invalid option -$OPTARG" >&2;;
  esac
done

# 检查是否设置了frpc_port参数
if [ -n "$frpc_port" ]; then
    # 启动frpc
    echo "Starting frpc..."
    docker run -d --name $DOCKER_NAME \
    --restart always \
    -w /root/pikascript \
    -e FRPC_REMOTE_PORT=$frpc_remote_port \
    -e FRPC_SERVER_ADDR=$frpc_server_addr \
    -e FRPC_SERVER_PORT=$frpc_server_port \
    -e FRPC_TOKEN=$frpc_token \
    $DOCKER_NAME \
    /usr/local/bin/frpc -c /etc/frpc.ini
    echo "frpc started."
else
    # 不启动frpc
    echo "frpc_port is not set. Skipping frpc startup."
    docker run -it --name $DOCKER_NAME \
    --restart always \
    -w /root/pikascript \
    $DOCKER_NAME \
    bash
fi

