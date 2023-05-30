#!/bin/bash

# 解析命令行参数
while getopts ":p:s:t:r:" opt; do
  case $opt in
    p) frpc_server_port=$OPTARG;;
    s) frpc_server_addr=$OPTARG;;
    t) frpc_token=$OPTARG;;
    r) frpc_remote_port=$OPTARG;;
    \?) echo "Invalid option -$OPTARG" >&2;;
  esac
done

DOCKER_NAME="pikadev"
if [ -n "$frpc_remote_port" ]; then
    DOCKER_NAME+="_$frpc_remote_port"
fi

# 检查是否设置了frpc_server_port参数
if [ -n "$frpc_server_port" ]; then
    # 启动frpc
    echo "Starting frpc..."
    docker rm $DOCKER_NAME -f
    docker run -it --name $DOCKER_NAME \
    --restart always \
    -w /root/pikascript \
    -e FRPC_REMOTE_PORT=$frpc_remote_port \
    -e FRPC_SERVER_ADDR=$frpc_server_addr \
    -e FRPC_SERVER_PORT=$frpc_server_port \
    -e FRPC_TOKEN=$frpc_token \
    pikadev \
    bash /etc/frpc.sh
else
    # 不启动frpc
    echo "frpc_server_port is not set. Skipping frpc startup."
    docker rm $DOCKER_NAME -f
    docker run -it --name $DOCKER_NAME \
    --restart always \
    -w /root/pikascript \
    pikadev \
    bash
fi
