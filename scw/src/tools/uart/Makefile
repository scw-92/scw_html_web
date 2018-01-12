buildrootHome=/home/sbc_7109_455/aplex/buildroot/buildroot-2017.02.3
CC=arm-linux-gnueabihf-g++
all:
	$(CC) -I${buildrootHome}/output/build/libwebsockets-v2.1.1/lib \
		-I${buildrootHome}/output/build/libwebsockets-v2.1.1/ \
			-I${buildrootHome}/output/target/include/ \
		 -I${buildrootHome}/output/build/jsoncpp-1.7.7/include \
		 -I${buildrootHome}/output/host/usr/include \
		-L${buildrootHome}/output/target/lib/ \
		-L${buildrootHome}/output/target/usr/lib/ \
		-lwebsockets \
		-ljsoncpp \
		-lpthread \
		-lz \
		-lssl \
		-lcrypto \
		-std=c++11 \
		my_new_socket.cpp \
		-o lws_uart
.PHONY: clean
clean:
	rm lws_uart
