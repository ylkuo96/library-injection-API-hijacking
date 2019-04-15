hw2.so: hw2.cpp
	g++ -o hw2.so -shared -fPIC hw2.cpp -ldl

test: hw2.so
	#MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ../testcases/file
	#MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ../testcases/dir
	#MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ../testcases/fscanf
	#MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ../testcases/getline
	#MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ../testcases/stdio
	MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./hw2.so ls

tst: tst.cpp
	g++ -o tst -Wall -g tst.cpp
