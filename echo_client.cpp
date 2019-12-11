#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/socket.h>

#include <thread>

void usage() {

	printf("syntax : echo_client <host> <port>\n");

	printf("sample : echo_client 127.0.0.1 1234\n");

}

void recv_from_server(int sock) {

	while (true) {

		char message[1024];

		int len = recv(sock, message, 1023, 0);

		if (len <= 0) {

			perror("recv failed");

			break;

		}

		message[len] = 0;

		printf("server send : %s\n", message);

	}

}

 

int main(int argc, char *argv[]) {

	// 파일 디스크립터를 위한 변수

	int sock;

	char message[1024];

	int str_len;

	struct sockaddr_in serv_adr;

 

	if (argc != 3) {

		usage();

		return -1;

	}

 

	// 1. socket 하나를 생성한다.

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if (sock == -1)

	{

		perror("socekt fail");

		return 0;	

	}

 

	memset(&serv_adr, 0, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;

	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);

	serv_adr.sin_port = htons(atoi(argv[2]));

 

	// 2. socket을 이용해 server의 server socket(listen socket)에 연결을 요청한다.

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)

	{

		perror("connect fail");

		return 0;

	}

	else

		puts("Connected...........");

	std::thread t_recv = std::thread(recv_from_server, sock);

	t_recv.detach();

 

	while (1) {

		fputs("Input message(Q to quit): ", stdout);

		fgets(message, 1024, stdin);

 

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))

			break;

 

		// 3. 연결된 socket을 통해 server로부터 데이터를 송수신한다.

		if (send(sock, message, strlen(message), 0) <= 0) {

			perror("send fail");

			break;

		}

	}

 

	close(sock);

	return 0;

}


