#include "pack.h"

void println() {
	printf("\n");
}

void printSocketAddressInformation(client_info client) {
	char temp[30];
	printf(" ip %s, port %d ,state : %d",
			inet_ntop(AF_INET, &client.client_sock_addr.sin_addr, temp,
					INET_ADDRSTRLEN), ntohs(client.client_sock_addr.sin_port),
			client.state);
}


int isSocketAddrEmpty(struct sockaddr_in sockaddr) {
	char temp[30];
	const char *ip = inet_ntop(AF_INET, &sockaddr.sin_addr, temp,
			INET_ADDRSTRLEN);
	return strcmp(ip, "0.0.0.0") == 0;
}

void printClientList(sockaddr_list client_list) {
	char temp[30];
	int i;
	printf("\n%s PRINT CLIENT'S LIST (%d clients) : \n",SPACE_AREA, CLIENT_SIZE);
	for (i = 0; i < CLIENT_SIZE; i++) {
		client_info client = client_list.client[i];
		if (isSocketAddrEmpty(client.client_sock_addr) == 0) {
			printf("client[%d] : %s - ip : %s - port %d, state :", i,client.name,
					inet_ntop(AF_INET, &client.client_sock_addr.sin_addr, temp,
							INET_ADDRSTRLEN),
					ntohs(client.client_sock_addr.sin_port));
			printClientState(client.state);
			println();
		} else
			printf("client[%d] : empty\n", i);
	}
}

/* changed */
void sendServerConnectRequest(int targetSocket,char name[]) {
	data_package data;
	data.data_type = SERVER_CONNECT_REQUEST;
	strcpy(data.common_data.cli_info.name,name);
	send(targetSocket, (char*) &data, MAXLINE, 0);
}

void sendSocketAddressList(int sockfd, sockaddr_list sock_list, int index) {
	data_package data;
	data.data_type = CLIENT_LIST;
	data.common_data.sock_list = sock_list;
	data.common_data.sock_list.index = index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void sendInvitePlayerRequest(int sockfd, int player_index,char name[]) {
	data_package data;
	data.data_type = INVITE_PLAY;
	data.common_data.cli_info.index = player_index;
	strcpy(data.common_data.cli_info.name,name);
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void sendConfirmPlay(int sockfd, int player_index) {
	data_package data;
	data.data_type = CONFIRM_INVITATION;
	data.common_data.cli_info.index= player_index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void sendFailedRequest(int sockfd, int player_index) {
	data_package data;
	data.data_type = INVITE_FAILED;
	data.common_data.cli_info.index = player_index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void sendChattingData(int sockfd, char chattingdata[], int player_index) {
	data_package data;
	data.data_type = CHATTING_DATA;
	data.common_data.chat_data.destinationPlayer = player_index;
	strcpy(data.common_data.chat_data.data, chattingdata);
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void sendPartnerDisconnectedNotification(int targetSock) {
	data_package data;
	data.data_type = PARTNER_DISCONNECTED_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendReadyConfirm(int targetSock) {
	data_package data;
	data.data_type = READY_CONFIRM;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendStartBattleDataPackage(int targetSock) {
	data_package data;
	data.data_type = START_BATTLE;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendReadyPartnerNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = READY_PARTNER_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendFirstTurnNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = FIRST_TURN;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendSecondTurnNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = SECOND_TURN;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendShotDataPackage(int targetSock, int x,int y){
	data_package data;
	data.data_type = SHOT_DATA;
	data.common_data.shot_data.x = x;
	data.common_data.shot_data.y = y;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendIsHitNotificationPackage(int targetSock, int isHit){
	data_package data;
	data.data_type = IS_HIT_NOTI;
	data.common_data.isHit = isHit;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void sendLoseNotificationPackage(int targetSock){
	data_package data;
	data.data_type = LOSE_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

int isClienStateEqual(client_info mClientInfo, client_state state) {
//	printf(" compare state : %d - %d\n", mClientInfo.state, state);
	if (mClientInfo.state == state)
		return 1;
	else
		return 0;
}

void printClientState(client_state state) {
	switch (state) {
	case ROOM_ONLINE:
		printf(" online ");
		break;
	case WAIT_RESPOND:
		printf(" respond waiting ");
		break;
	case BUILD_MAP:
		printf(" build map ");
		break;
	case READY:
		printf(" ready ");
		break;
	case BATTLING:
		printf(" battling with partner ");
		break;

	default:
		break;
	}
}

/**
 * return 1 if success adding, 0 else
 */
int addClientToBattleRoom(Rooms *rooms, sockaddr_list *sockList, int client1,
		int client2) {
//	printRoomStates(*rooms);
	int i;
	for (i = 0; i < 3; i++) {
		if (rooms->battle[i].state == EMPTY) {
			sockList->client[client1].state = BUILD_MAP;
			sockList->client[client2].state = BUILD_MAP;
			rooms->battle[i].players[0].player_index = client1;
			rooms->battle[i].players[0].state = BUILD_MAP;
			rooms->battle[i].players[1].player_index = client2;
			rooms->battle[i].players[1].state = BUILD_MAP;
			rooms->battle[i].state = BUILD_MAP_PHASE;
			return 1;
		}
	}
	return 0;
}

void initRooms(Rooms *rooms) {
	int i;
	for (i = 0; i < 3; i++) {
		rooms->battle[i].state = EMPTY;
	}
}

char* getBattleState(Battle_state state) {
	switch (state) {
	case EMPTY:
		return (char*) "empty";
	case BUILD_MAP_PHASE:
		return (char*) "build map phase";
	case BATTLING_PHASE:
		return (char*) "battling phase";
	default:
		printf("battle state not availale");
		break;
	}
	return (char*) "null";
}

void printRoomStates(Rooms rooms) {
	int i;
	for (i = 0; i < 3; i++) {
		printf("Battle 1 (%s): player %d - player %d \n",
				getBattleState(rooms.battle[i].state),
				rooms.battle[i].players[0].player_index,
				rooms.battle[i].players[1].player_index);
	}
}
