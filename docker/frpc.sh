sed -i "s|%FRPC_SERVER_ADDR%|${FRPC_SERVER_ADDR}|" /etc/frpc.ini && \
sed -i "s|%FRPC_SERVER_PORT%|${FRPC_SERVER_PORT}|" /etc/frpc.ini && \
sed -i "s|%FRPC_REMOTE_PORT%|${FRPC_REMOTE_PORT}|" /etc/frpc.ini && \
sed -i "s|%FRPC_TOKEN%|${FRPC_TOKEN}|" /etc/frpc.ini
cat /etc/frpc.ini
/usr/local/bin/frpc -c /etc/frpc.ini
