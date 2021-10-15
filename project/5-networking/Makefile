
magic_string ?= test_123

bin:
	mkdir -p ./bin

build_client: bin
	gcc -o ./bin/client ./src/client.c

build_server: bin
	gcc -o ./bin/server ./src/server.c

clean:
	rm ./bin/* -rf

run: clean bin build_client build_server
	./bin/server &
	sleep 1
	./bin/client localhost $(magic_string)

capture:
	sudo tcpdump -Xi lo udp port 4950