all: client server thanks

client:
	clang++ ./client/*.cpp ./shared/*.cpp -o ./client/client -std=c++17

server:
	clang++ ./server/*.cpp ./shared/*.cpp -o ./server/server -std=c++17

clean:
	rm -rf ./client/client && rm -rf ./server/server

thanks:
	echo "Make Done!"

.PHONY: all client server clean
.DEFAULT_GOAL: all
