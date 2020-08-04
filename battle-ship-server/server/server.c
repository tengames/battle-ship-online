#include "server.h"

sockaddr_list client_sockaddr_list;

int createListenFD() {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		perror("Failed to create socket!\n");
		exit(1);
	}
	return listenfd;
}

void bindServerAddress(struct sockaddr_in* server, int listenfd) {
	char temp[30];
	server->sin_family = AF_INET;
	server->sin_addr.s_addr = INADDR_ANY;
	server->sin_port = htons(SERV_PORT);

	/*call bin*/
	if (bind(listenfd, (struct sockaddr*) server, sizeof(*server))) {
		perror("bind failed!\n");
		exit(1);
	}
}

void listenDeclare(int listenfd) {
	listen(listenfd, LISTEN_QUEUE);
}

void initForSelectProceduce(int listenfd, fd_set *allset, int client[],
		int *maxfd, int* maxi) {
	*maxfd = listenfd; /* initialize */
	*maxi = -1; /* index into client[] array */
	int i;
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; /* -1 indicates available entry */
	FD_ZERO(allset);
	FD_SET(listenfd, allset);
}

void handleClientConnection(int listenfd, sockaddr_list *client_list,
		int client[], fd_set *allset, int* maxfd, int* maxi) {
	struct sockaddr_in clientaddr;
	int clilen = sizeof(clientaddr);
	int connfd;
	int i;
	int isAvaialbeSpaceExist = 0;
	for(i = 0; i< CLIENT_SIZE;i++){
		if (isSocketAddrEmpty(client_list->client[i].client_sock_addr))
			isAvaialbeSpaceExist = 1;
	}

	connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clilen);
	if (isAvaialbeSpaceExist == 0){
		printf("*** NEW CLIENT WANT TO CONNECT, BUT CLIENT SIZE IS MAX, REFUSE HIM!! \n");
		close(connfd);
		return;
	}
	addClientAddressToArray(client_list, clientaddr);
//	printClientList(*client_list);
	for (i = 0; i < FD_SETSIZE; i++)
		if (client[i] < 0) {
			client[i] = connfd; /* save descriptor */
			break;
		}
	if (i == FD_SETSIZE) {
		perror("too many clients");
		exit(1);
	}
	FD_SET(connfd, allset);
	/* add new descriptor to set */
	if (connfd > *maxfd)
		*maxfd = connfd; /* for select */
	if (i > *maxi)
		*maxi = i; /* max index in client[] array */

}

void checkClients(Rooms* rooms, int client_sock[], int maxi, fd_set *rset,
		fd_set *allset, int* nready, Confirm_waiting_clients *CWClients) {
	data_package* data;
	int i, sockfd, n;
	char buff[1024];
	for (i = 0; i <= maxi; i++) { /* check all clients for data */
		if ((sockfd = client_sock[i]) < 0)
			continue;
		if (FD_ISSET(sockfd, rset)) {
			if ((n = read(sockfd, buff, MAXLINE)) == 0) {
				/*4connection closed by client */
				handleDisconnectedClient(rooms, sockfd, i, allset, client_sock,
						&client_sockaddr_list,CWClients);

			} else {
//				printf("server receved %d byte data !\n", n);
				data = (data_package*) buff;
				switch (data->data_type) {
				case SERVER_CONNECT_REQUEST:
					strcpy(client_sockaddr_list.client[i].name,data->common_data.cli_info.name);
					sendSocketAddressList(sockfd, client_sockaddr_list, i);
					client_sockaddr_list.client[i].state = ROOM_ONLINE;
					printClientList(client_sockaddr_list);
					break;
				case INVITE_PLAY:
					handleInvitePlayPackage(i, data, client_sock,
							&client_sockaddr_list,CWClients);
					break;
				case CONFIRM_INVITATION:
					handleConfirmInvitationPackage(i, data, rooms, client_sock,
							&client_sockaddr_list,CWClients);
					break;
				case INVITE_FAILED:
					handleInviteFailedPackage(data, i, client_sock,
							&client_sockaddr_list,CWClients);
					break;
				case READY_CONFIRM:
					printRoomStates(*rooms);
					handleReadyPackage(i, client_sock, rooms,
							&client_sockaddr_list);
					break;
				case CHATTING_DATA:
					handleChattingDataPackage(i, data, client_sock, rooms);
					break;
				case SHOT_DATA:
					handleShotDataPackage(i, rooms, *data, client_sock);
					break;
				case IS_HIT_NOTI:
					handleIsHitNotificationPackage(i, rooms, *data,
							client_sock);
					break;
				case LOSE_NOTI:
// 					printf("Nhan duoc goi tin bao thua!\n");
					
					handleLoseNotificationPackage(i, rooms, client_sock);
					emptyBattleInRoom(i, rooms, &client_sockaddr_list);
					break;
				default:
					printf("data type %d invalid!\n", data->data_type);
					break;
				}
			}

			if (--(*nready) <= 0)
				break; /* no more readable descriptors */
		}
	}
}

void printMyIp(void) {
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}
	/* Walk through linked list, maintaining head pointer so we
	 can free list later */
	printf("\n%s SERVER CONNECTION IP \n",SPACE_AREA);

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		family = ifa->ifa_addr->sa_family;

		/* Display interface name and family (including symbolic
		 form of the latter for the common families) */
		if (family == AF_INET)
			printf("%s  address family: %d%s\n", ifa->ifa_name, family,
					(family == AF_PACKET) ? " (AF_PACKET)" :
					(family == AF_INET) ? " (AF_INET)" :
					(family == AF_INET6) ? " (AF_INET6)" : "");

		/* For an AF_INET* interface address, display the address */

		if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr,
					(family == AF_INET) ?
							sizeof(struct sockaddr_in) :
							sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL,
					0, NI_NUMERICHOST);
			if (s != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			printf("\taddress: <%s>\n", host);
		}
	}

	freeifaddrs(ifaddr);
}

int main(int argc, char *argv[]) {
	/*variables*/
	int listenfd;
	int maxi, maxfd, nready, client[FD_SETSIZE];
	struct sockaddr_in server;
	fd_set rset, allset;
	Rooms rooms;
	Confirm_waiting_clients CWClients;

	printf(
			"=================================== BATTLE-SHIP SERVER ======================================\n");
	initCWClientList(&CWClients);
	initRooms(&rooms);

	listenfd = createListenFD();

	bindServerAddress(&server, listenfd);

	/*listen*/
	listenDeclare(listenfd);

	printMyIp();

	initForSelectProceduce(listenfd, &allset, client, &maxfd, &maxi);
	/*accept*/
	for (;;) {
		rset = allset; /* structure assignment */
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) { /* new client connection */
			handleClientConnection(listenfd, &client_sockaddr_list, client,
					&allset, &maxfd, &maxi);
			if (nready <= 1)
				continue; /* no more readable descriptors */
		}

		checkClients(&rooms, client, maxi, &rset, &allset, &nready,&CWClients);

	}

	return 0;
}
