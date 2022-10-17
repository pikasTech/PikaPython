cd Release && ninja -j16 && cd ..
Release/performance/pikascript_performance
gprof Release/performance/pikascript_performance gmon.out > report.txt
