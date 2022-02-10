# config
OUT_NAME=result.txt

# clear
rm $OUT_NAME

# make the project
make clean
make printf_demo_ALL

# launch qemu service
make printf_demo_QEMURUN_TEL > qemu.out 2>&1 & echo $! > qemu.pid
sleep 1
# launch netcat client
nc -d 0.0.0.0 7777 > $OUT_NAME 2>&1 & echo $! > nc.pid
sleep 1

# kill the processes
kill `cat nc.pid`
kill `cat qemu.pid`
rm nc.pid
rm qemu.pid 
rm qemu.out

# print the result
echo ======================================
echo [info]: qemu benchmark output
echo --------------------------------------
cat $OUT_NAME
echo ======================================
