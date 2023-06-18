cd build && rm ./test/pikascript_test -f && ninja
cd .. && cp ./build/boot/demo06-pikamain/pikascript_demo06-pikamain package/pikascript/pika
python3 generate_stack_usage_report.py
