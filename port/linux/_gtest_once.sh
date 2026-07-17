set -e

rm -f build/test/pikascript_test
sh make.sh
PIKA_GTEST_FILTER=${PIKA_GTEST_FILTER:--mqtt.*:network.*:socket.*:requests.*:jrpc.BlockingRequestBetweenTwoJRPC}
timeout 10m build/test/pikascript_test --gtest_filter="$PIKA_GTEST_FILTER"
