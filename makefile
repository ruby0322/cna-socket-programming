all: client server thanks

client:
	clang++ ./client/*.cpp ./shared/*.cpp -o ./run_client -std=c++17 -L/opt/homebrew/opt/openssl/lib -I/opt/homebrew/opt/openssl/include -lssl -lcrypto -Wno-deprecated

server:
	clang++ ./server/*.cpp ./shared/*.cpp -o ./run_server -std=c++17 -L/opt/homebrew/opt/openssl/lib -I/opt/homebrew/opt/openssl/include -lssl -lcrypto -Wno-deprecated

clean:
	rm ./run_server && rm ./run_client

thanks:
	echo "Make Done!"

.PHONY: all client server clean
.DEFAULT_GOAL: all
