cd Release && ninja -j0 && cd ..
Release/performance/pikascript_performance
gprof Release/performance/pikascript_performance gmon.out > report.txt
