all: client server

client:
	clang++ ./client/*.cpp *.cpp -o ./client/client -std=c++17

server:
	clang++ ./server/*.cpp *.cpp -o ./server/server -std=c++17

clean:
	rm -rf ./client/client && rm -rf ./server/server

.PHONY: all client server clean
.DEFAULT_GOAL: all
