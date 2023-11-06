all: client server thanks

client:
	clang++ ./client/*.cpp ./shared/*.cpp -o ./run_client -std=c++17

server:
	clang++ ./server/*.cpp ./shared/*.cpp -o ./run_server -std=c++17

clean:
	rm ./run_server && rm ./run_client

thanks:
	echo "Make Done!"

.PHONY: all client server clean
.DEFAULT_GOAL: all
