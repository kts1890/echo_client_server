all: echo_client echo_server

echo_client: echo_client.cpp
	g++ -o echo_client echo_client.cpp -pthread -std=gnu++11

echo_server: echo_server.cpp
	g++ -o echo_server echo_server.cpp -pthread -std=gnu++11

clean:
	rm echo_server
	rm echo_client
