#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <unistd.h>

#include <vector>

#include <set>

#include <mutex>

#include <thread>

using namespace std;

void thrfunc(int sock);

void thrfunc_b(int sock, set<int>* accp_sock, mutex* mut);

char message[1024];

set<int> accp_sock;

void usage() {

	printf("syntax : echo_server <port> [-b]\n");

	printf("sample : echo_server 1234 -b\n");

}

void error_handling(char *message) {

	fputs(message, stderr);

	fputc('\n', stderr);

	exit(1);

}

 

int listen_sock;

 

int main(int argc, char* argv[]) {

	if (!(argc == 2 || (argc == 3 && argv[2][0] == '-' && argv[2][1] == 'b'))) {

		usage();

		return -1;

	} 

	

	

	int str_len, i;

	int status;

	struct sockaddr_in serv_adr;

	struct sockaddr_in clnt_adr;	

	socklen_t clnt_adr_sz;

	clnt_adr_sz = sizeof(clnt_adr);

 

 

	listen_sock = socket(PF_INET, SOCK_STREAM, 0);

	if (listen_sock == -1)

		error_handling("socket() error");

 

	memset(&serv_adr, 0, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;

	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

	serv_adr.sin_port = htons(atoi(argv[1]));

 

	if (bind(listen_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)

		error_handling("bind() error");

	listen(listen_sock, 10);

	puts("client wait....");

	mutex mut;

	vector<thread> server;

	if (argc == 3) {

		while (1) {

			int accp = accept(listen_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

 

			if (accp < 0) {

				perror("accept fail");

				exit(0);

			}

			mut.lock();

			accp_sock.insert(accp);

			server.emplace_back(thread(thrfunc_b, accp, &accp_sock, &mut));
			server.back().detach();

			mut.unlock();

		}

	}

	else {

		while (1) {

			int accp = accept(listen_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

 

			if (accp < 0) {

				perror("accept fail");

				exit(0);

			}

			accp_sock.insert(accp);
			
			server.emplace_back(thread(thrfunc,accp));

			server.back().detach();

		}

	}

	close(listen_sock);

 

	return 0;

}

 

void thrfunc_b(int sock, set<int>* accp_sock, mutex* mut) {

	while (true) {

		int len = recv(sock, message, 1023, 0);

		if (len <= 0) {

			perror("recv fail");

			break;

		}

		mut->lock();

		message[len] = 0;

		printf("client send  = %s\n", message);

 

		for (auto it = accp_sock->begin(); it != accp_sock->end(); it++) {

			if (send(*it, message, strlen(message), 0) <= 0) {

				perror("send failed");

				it = accp_sock->erase(it);

				continue;

			}

		}

		mut->unlock();

 

		if (accp_sock->find(sock) == accp_sock->end()) break;

	}

 

}

 

void thrfunc(int sock) {

	while (true) {

		int len = recv(sock, message, 1023, 0);

		if (len <= 0) {

			perror("recv fail");

			break;

		}

		message[len] = 0;

		printf("client send  = %s\n", message);

		if (send(sock, message, strlen(message), 0) <= 0) {

			perror("send fail");

			break;

		}

	}

}
