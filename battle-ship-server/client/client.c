#include "pack.h"
#define DATA "Hello Server!"

int createSocket() {
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("Socket failed!\n");
		exit(1);
	}
	return s;
}

void initServerSocketAddr(struct sockaddr_in* server, char* hostip, int socket) {
	struct hostent *hp;
	server->sin_family = AF_INET;
	hp = gethostbyname(hostip);
	if (hp == 0) {
		perror("gethostbyname failed\n");
		close(socket);
		exit(1);
	}
	memcpy(&server->sin_addr, hp->h_addr, hp->h_length);
	server->sin_port = htons(SERV_PORT);
}

void connectServer(struct sockaddr_in* server, int sock) {
	if (connect(sock, (struct sockaddr*) server, sizeof(*server)) < 0) {
		perror("Connect failed!\n");
		close(sock);
		exit(1);
	}
}

int max(int i1, int i2) {
	if (i1 > i2)
		return i1;
	else
		return i2;
}

void printMenu() {
	printf("\n\n");
	printf("\t\t Ban muon lam gi tiep theo? \n");
	printf("\t\t 1. Ket noi den 1 trong cac client tren\n");
	printf("\t\t 2. Lam moi danh sach cac player\n");
	printf("\t\t 10. Thoat\n");
}

int checkLose(int map[],int size){
	int i, j;
	for(i = 0;i < size;i++)
		for(j = 0;j < size;j++){
			if (map[i*size+j] == 1)
				return 0;
		}
	return 1;
}

int main(int argc, char *argv[]) {
	client_state state;
	int isChooseClient = 0;
	int server_sock, n;
	int x, y;
	struct sockaddr_in server;
	server_connect_request scr;
	data_package *data;
	sockaddr_list* client_list;
	fd_set readset;
	char buff[4096];
	char choose[300];
	int requestedPlayer, chattingPlayer;
	int isMyTurn = 0, isHit;
	int map[3][3] = {{0,0,0},{1,1,1},{0,0,0}};
	server_sock = createSocket();
	initServerSocketAddr(&server, argv[1], server_sock);
	connectServer(&server, server_sock);

	sendServerConnectRequest(server_sock,"tran my");
	while (1) {
		FD_ZERO(&readset);
		FD_SET(server_sock, &readset);
		FD_SET(fileno(stdin), &readset);
		select(max(server_sock, fileno(stdin)) + 1, &readset, NULL, NULL, NULL);
		if (FD_ISSET(server_sock, &readset)) {
			if ((n = read(server_sock, buff, MAXLINE)) == 0) {
				printf(" server terminated prematurely");
				exit(1);
			} else {
				data = (data_package*) buff;
				switch (data->data_type) {
				case CLIENT_LIST:
					printClientList(data->common_data.sock_list);
					state = ROOM_ONLINE;
					printMenu();
					break;
				case INVITE_PLAY:
					state = CONFIRM_PLAY_REQUEST;
					requestedPlayer = data->common_data.cli_info.index;
					printf(
							"client[%d] : %s want to play with you! accpect his/her request? (y/n) \n",
							requestedPlayer,data->common_data.cli_info.name);
					break;
				case CONFIRM_INVITATION:
					if (state == WAIT_RESPOND) {
						printf("he/she appect your play request!\n");
						printf("Ban xac nhan la da xep xong tau?? (y/n) : \n");
						chattingPlayer = data->common_data.cli_info.index;
						state = BUILD_MAP;
					}
					break;
				case INVITE_FAILED:
					if (state == WAIT_RESPOND) {
						printf("client[%d] refused your play request!\n",data->common_data.cli_info.index);
						isChooseClient = 0;
						state = ROOM_ONLINE;
						printMenu();
					}
					break;
				case READY_PARTNER_NOTI:
					printf("Ban choi cua ban xac nhan da xep xong tau!\n");
					break;
				case FIRST_TURN:
					printf("first turn\n");
					isMyTurn = 1;
					printf("Choose number from 1 to 9 to shot!\n");
					break;
				case SECOND_TURN:
					printf("second turn\n");
					isMyTurn = 0;
					break;
				case START_BATTLE:
					printf(
							"Tran chien bat dau, ban co the chat voi doi phuong!\n");
					state = BATTLING;
					break;
				case CHATTING_DATA:
					if (state == BATTLING) {
						printf("your chatting friend typed : %s\n",
								data->common_data.chat_data.data);
					}
					break;
				case SHOT_DATA:
					x = data->common_data.shot_data.x;
					y = data->common_data.shot_data.y;
					printf("Partner shot in position %d-%d !\n",x,y);
					if (map[x][y] == 1){
						map[x][y] = 2;
						if (checkLose(map,3) == 1){
							printf("You lose! (send Win notification to partner client!) \n");
							sendLoseNotificationPackage(server_sock);
							state = ROOM_ONLINE;
							isChooseClient = 0;
							printMenu();
						}else{
							printf(" HIT! partner can continuously shot !!\n");
							sendIsHitNotificationPackage(server_sock,1);
						}
					}else{
						printf(" NOT HIT! It's your move! \n");
						isMyTurn = 1;
					}
					break;
				case IS_HIT_NOTI:
					isHit = data->common_data.isHit;
					if (isHit == 1){
						printf("You has been hit the partner, you can continuously shot! \n");
						isMyTurn = 1;
					}else{
						printf("You has't been hit the partner, you must wait the partner finish his turn!\n ");
						isMyTurn = 0;
					}
					break;
				case LOSE_NOTI:
					printf("You wont! Congratulation! Back to menu! \n");
					state = ROOM_ONLINE;
					isChooseClient = 0;
					printMenu();
					break;
				case PARTNER_DISCONNECTED_NOTI:
					printf(
							"Your partner has been disconnected! You are brought back to room!\n");
					state = ROOM_ONLINE;
					isChooseClient = 0;
					printMenu();
					break;
				default:
					break;
				}

			}
		}
		if (FD_ISSET(fileno(stdin), &readset)) {
			fgets(choose, 300, stdin);
			switch (state) {
			case ROOM_ONLINE:
				if (isChooseClient == 0) {
					if (strcmp(choose, "1\n") == 0) {
						printf("Hay nhap 1 so thu tu cua client (0-5)");
						isChooseClient = 1;
					} else if (strcmp(choose, "10\n") == 0) {
						exit(1);
					} else if (strcmp(choose, "2\n") == 0) {
						sendServerConnectRequest(server_sock,"tran my");
					}
				} else if (isChooseClient == 1) {
					if (strcmp(choose, "0\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 0,"tranmy");
					} else if (strcmp(choose, "1\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 1,"tranmy");
					} else if (strcmp(choose, "2\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 2,"tranmy");
					} else if (strcmp(choose, "3\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 3,"tranmy");
					} else if (strcmp(choose, "4\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 4,"tranmy");
					} else if (strcmp(choose, "5\n") == 0) {
						printf("Cho doi server hoi dap .... \n");
						sendInvitePlayerRequest(server_sock, 5,"tranmy");

					}
					state = WAIT_RESPOND;
				}
				break;
			case CONFIRM_PLAY_REQUEST:
				if (strcmp(choose, "y\n") == 0) {
					sendConfirmPlay(server_sock, requestedPlayer);
					state = BUILD_MAP;
					printf("Ban xac nhan la da xep xong tau?? (y/n) : ");
				} else {
					sendFailedRequest(server_sock, requestedPlayer);
					state = ROOM_ONLINE;
					printMenu();
				}
				break;
			case BUILD_MAP:
				if (strcmp(choose, "y\n") == 0) {
					sendReadyConfirm(server_sock);
					printf("Ban da xac nhan da xep xong tau ...!");
				} else {
					/* CHua xu li */;
//					sendFailedRequest(server_sock,requestedPlayer);
//					client_state = CHOOSE_MENU;
//					printMenu();
				}
				break;
			case BATTLING:
//				sendChattingData(server_sock, choose, chattingPlayer);
//				printf("(sent chat data!)\n");
				if (isMyTurn == 1) {
					printf("shoot %s ",choose);
					if (strcmp(choose, "1\n") == 0)
						sendShotDataPackage(server_sock, 0, 0);
					else if (strcmp(choose, "2\n") == 0)
						sendShotDataPackage(server_sock, 1, 0);
					else if (strcmp(choose, "3\n") == 0)
						sendShotDataPackage(server_sock, 2, 0);
					else if (strcmp(choose, "4\n") == 0)
						sendShotDataPackage(server_sock, 0, 1);
					else if (strcmp(choose, "5\n") == 0)
						sendShotDataPackage(server_sock, 1, 1);
					else if (strcmp(choose, "6\n") == 0)
						sendShotDataPackage(server_sock, 2, 1);
					else if (strcmp(choose, "7\n") == 0)
						sendShotDataPackage(server_sock, 0, 2);
					else if (strcmp(choose, "8\n") == 0)
						sendShotDataPackage(server_sock, 1, 2);
					else if (strcmp(choose, "9\n") == 0)
						sendShotDataPackage(server_sock, 2, 2);
					else
						printf(" shoot point invalid! \n");
				} else {
					printf(" It's not your move, please wait...!\n");
				}

				break;
			default:
//				printf("default");
				break;
			}
			printf("\n");
		}
	}

	return 0;
}
