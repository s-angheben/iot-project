all:
	gcc -I./ board_sim.c mqtt_wrapper.c dht11.c  c_utils.c -lrt -l paho-mqtt3c -o board_sim.o

coverage: 
	gcc -fprofile-arcs -ftest-coverage -I./ board_sim.c mqtt_wrapper.c dht11.c  c_utils.c -lrt -l paho-mqtt3c -o board_sim.o

checkmemory:
	gcc  -g -I./ board_sim.c mqtt_wrapper.c dht11.c  c_utils.c -lrt -l paho-mqtt3c -o board_sim.o

