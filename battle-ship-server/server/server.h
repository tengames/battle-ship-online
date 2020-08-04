#include "pack.h"
#include <ifaddrs.h>
#include <arpa/inet.h>

typedef struct _confirm_waiting_clients {
	int confirm_clients[3];
	int waiting_clients[3];
} Confirm_waiting_clients;

void printSpaceArea() {
	printf("\t\t\t\t");
}

int addClientAddressToArray(sockaddr_list *client_list,
		struct sockaddr_in clientaddr) {
	int i;
	printf("\n%s NEW CLIENT CONNECTION", SPACE_AREA);
	for (i = 0; i < CLIENT_SIZE; i++) {
		struct sockaddr_in tempaddr = client_list->client[i].client_sock_addr;
		if (isSocketAddrEmpty(tempaddr) == 1) {
			memcpy(&client_list->client[i].client_sock_addr, &clientaddr,
					sizeof(struct sockaddr_in));
			printf("\nDetail : ");
			client_list->client[i].state = ROOM_ONLINE;
			printSocketAddressInformation(client_list->client[i]);
			printf("\n");
			return 1;
		}
	}
	printf("list client is full, can't add new client\n");
	return 0;
}

void deleteClientInRooms(Rooms* rooms, int i,
		sockaddr_list *client_sockaddr_list, int client_sock[]) {
	int j, partnerIndex;
	for (j = 0; j < 3; j++) {
		if (rooms->battle[j].state != EMPTY) {
			if (i == rooms->battle[j].players[0].player_index) {
				partnerIndex = rooms->battle[j].players[1].player_index;
				rooms->battle[j].state = EMPTY;
				client_sockaddr_list->client[partnerIndex].state = ROOM_ONLINE;
				printf(" send partner disconnected to client[%d] \n",
						partnerIndex);
				sendPartnerDisconnectedNotification(client_sock[partnerIndex]);
				break;
			} else if (i == rooms->battle[j].players[1].player_index) {
				partnerIndex = rooms->battle[j].players[0].player_index;
				rooms->battle[j].state = EMPTY;
				client_sockaddr_list->client[partnerIndex].state = ROOM_ONLINE;
				printf(" send partner disconnected to client[%d] \n",
						partnerIndex);
				sendPartnerDisconnectedNotification(client_sock[partnerIndex]);
				break;
			}
		}
	}

}

void handleDisconnectedClient(Rooms *rooms, int sockfd, int i, fd_set *allset,
		int client_sock[], sockaddr_list *client_sockaddr_list,
		Confirm_waiting_clients *CWClients) {
	printf("%s DISCONNECT CLIENT \n", SPACE_AREA);
	printf("client with : ");
	printSocketAddressInformation(client_sockaddr_list->client[i]);
	printf("    close the connection!\n");
	close(sockfd);
	FD_CLR(sockfd, allset);
	inet_pton(AF_INET, "0.0.0.0",
			&(client_sockaddr_list->client[i].client_sock_addr.sin_addr));
	deleteClientInRooms(rooms, i, client_sockaddr_list, client_sock);
	handleWaitingClientIfNeeded(CWClients, i, client_sock,
			client_sockaddr_list);
	client_sockaddr_list->client[i].client_sock_addr.sin_port = htons(0);
	client_sock[i] = -1;
	printClientList(*client_sockaddr_list);
}

void handleInvitePlayPackage(int i, data_package *data, int client_sock[],
		sockaddr_list *client_sockaddr_list, Confirm_waiting_clients *CWClients) {
	int requestedPlayerIndex = data->common_data.cli_info.index;
	int result;
	printf("client[%d] send play request to client[%d]! \n", i,
			requestedPlayerIndex);
	if (!isSocketAddrEmpty(
			client_sockaddr_list->client[requestedPlayerIndex].client_sock_addr)
			&& isClienStateEqual(
					client_sockaddr_list->client[requestedPlayerIndex],
					ROOM_ONLINE) == 1) {
		client_sockaddr_list->client[i].state = WAIT_RESPOND;
		client_sockaddr_list->client[requestedPlayerIndex].state = WAIT_RESPOND;
		sendInvitePlayerRequest(client_sock[requestedPlayerIndex], i,
				client_sockaddr_list->client[i].name);
		result = getEmptySpaceIndexInCWClientList(*CWClients);
		if (result == -1) {
			printf("!! Don't have enough space in Confirm Waiting Clients");
		} else {
			CWClients->confirm_clients[result] = requestedPlayerIndex;
			CWClients->waiting_clients[result] = i;
			printf(" client[%d] is waiting accpect confirm for client[%d]!!\n",
					CWClients->waiting_clients[result],
					CWClients->confirm_clients[result]);
		}
	} else {
		printf("client[%d] not available, request failied!\n",
				requestedPlayerIndex);
		sendFailedRequest(client_sock[i], requestedPlayerIndex);
		client_sockaddr_list->client[i].state = ROOM_ONLINE;
	}
}

void handleConfirmInvitationPackage(int i, data_package *data, Rooms* rooms,
		int client_sock[], sockaddr_list *client_sockaddr_list,
		Confirm_waiting_clients *CWClients) {
	int requestedPlayerIndex = data->common_data.cli_info.index;
	int result;
	printf("client[%d] appect play request with client[%d]! \n", i,
			requestedPlayerIndex);
	if (!isSocketAddrEmpty(
			client_sockaddr_list->client[requestedPlayerIndex].client_sock_addr)) {
		if (1
				== addClientToBattleRoom(rooms, client_sockaddr_list,
						requestedPlayerIndex, i)) {
			printf(
					"create battle successfully! Send Confirm package to requested client!\n");
			printRoomStates(*rooms);
			sendConfirmPlay(client_sock[requestedPlayerIndex], client_sock[i]);
			result = getIndexInCWClientList(i, CWClients);
			if (result == -1) {
				printf("get Index failed!\n");
			}
			{
				printf("Reset CWClients in index %d\n", result);
				resetEmptySpaceInCWClientsIndex(result, CWClients);
			}
		} else {
			printf("create battle failed! (Not handle this case!)\n");
		}
	} else {
		printf("but client[%d] has been disconnected \n", requestedPlayerIndex);
		sendPartnerDisconnectedNotification(client_sock[i]);
	}
}

void handleInviteFailedPackage(data_package *data, int i, int client_sock[],
		sockaddr_list *client_sockaddr_list, Confirm_waiting_clients *CWClients) {
	int result;
	int requestedPlayerIndex = data->common_data.cli_info.index;
	printf("client[%d] refused play request with client[%d]! \n", i,
			requestedPlayerIndex);
	if (!isSocketAddrEmpty(
			client_sockaddr_list->client[requestedPlayerIndex].client_sock_addr)) {
		sendFailedRequest(client_sock[requestedPlayerIndex], i);
		client_sockaddr_list->client[requestedPlayerIndex].state = ROOM_ONLINE;
		client_sockaddr_list->client[i].state = ROOM_ONLINE;
		result = getIndexInCWClientList(i, CWClients);
		if (result == -1) {
			printf("get Index failed!\n");
		}
		{
			printf("Reset CWClients in index %d\n", result);
			resetEmptySpaceInCWClientsIndex(result, CWClients);
		}
	}
}

void handleReadyPackage(int i, int client_sock[], Rooms *rooms,
		sockaddr_list *client_sockaddr_list) {
	int j;
	printf(" Player %d sent ready confirm! \n ", i);
	for (j = 0; j < 3; j++) {
		printf(" finding player in battle %d (state : %s)\n", j,
				getBattleState(rooms->battle[j].state));
		if (rooms->battle[j].state == BUILD_MAP_PHASE) {
			if (i == rooms->battle[j].players[0].player_index) {
				rooms->battle[j].players[0].state = READY;
				if (rooms->battle[j].players[1].state == READY) {
					rooms->battle[j].state = BATTLING_PHASE;
					rooms->battle[j].players[0].state = BATTLING;
					rooms->battle[j].players[1].state = BATTLING;
					client_sockaddr_list->client[i].state = BATTLING;
					client_sockaddr_list->client[rooms->battle[j].players[1].player_index].state =
							BATTLING;
					sendStartBattleDataPackage(client_sock[i]);
					sendStartBattleDataPackage(
							client_sock[rooms->battle[j].players[1].player_index]);
					sendFirstTurnNotificationPackage(client_sock[i]);
					sendSecondTurnNotificationPackage(
							client_sock[rooms->battle[j].players[1].player_index]);
					break;
				} else {
					sendReadyPartnerNotificationPackage(
							client_sock[rooms->battle[j].players[1].player_index]);
				}
			} else if (i == rooms->battle[j].players[1].player_index) {
				rooms->battle[j].players[1].state = READY;
				if (rooms->battle[j].players[0].state == READY) {
					rooms->battle[j].state = BATTLING_PHASE;
					rooms->battle[j].players[0].state = BATTLING;
					rooms->battle[j].players[1].state = BATTLING;
					client_sockaddr_list->client[i].state = BATTLING;
					client_sockaddr_list->client[rooms->battle[j].players[0].player_index].state =
							BATTLING;
					sendStartBattleDataPackage(client_sock[i]);
					sendStartBattleDataPackage(
							client_sock[rooms->battle[j].players[0].player_index]);
					sendFirstTurnNotificationPackage(client_sock[i]);
					sendSecondTurnNotificationPackage(
							client_sock[rooms->battle[j].players[0].player_index]);
					break;
				} else {
					sendReadyPartnerNotificationPackage(
							client_sock[rooms->battle[j].players[0].player_index]);
				}
			}
		}
	}
}

void handleChattingDataPackage(int i, data_package *data, int client_sock[],
		Rooms* rooms) {
	int j;
	printf(
			"sending chatting data from client[%d] to client[%d] with data : %s !\n",
			i, data->common_data.chat_data.destinationPlayer,
			data->common_data.chat_data.data);
	for (j = 0; j < 3; j++) {
		if (i == rooms->battle[j].players[0].player_index) {
			sendChattingData(
					client_sock[rooms->battle[j].players[1].player_index],
					data->common_data.chat_data.data,
					data->common_data.chat_data.destinationPlayer);
			break;
		} else if (i == rooms->battle[j].players[1].player_index) {
			sendChattingData(
					client_sock[rooms->battle[j].players[0].player_index],
					data->common_data.chat_data.data,
					data->common_data.chat_data.destinationPlayer);
			break;
		}
	}
}

int findPartnerIndexInRooms(Rooms rooms, int i) {
	int j, partnerIndex = -1;
	for (j = 0; j < 3; j++) {
		if (i == rooms.battle[j].players[0].player_index) {
			partnerIndex = rooms.battle[j].players[1].player_index;
			break;
		} else if (i == rooms.battle[j].players[1].player_index) {
			partnerIndex = rooms.battle[j].players[0].player_index;
			break;
		}
	}
	return partnerIndex;
}

void handleShotDataPackage(int i, Rooms *rooms, data_package data,
		int client_sock[]) {
	int partnerIndex = findPartnerIndexInRooms(*rooms, i);
	int x = data.common_data.shot_data.x;
	int y = data.common_data.shot_data.y;
	printf(" client[%d] shoot in position %d-%d \n", i, x, y);
	sendShotDataPackage(client_sock[partnerIndex], x, y);
}

void handleIsHitNotificationPackage(int i, Rooms* rooms, data_package data,
		int client_sock[]) {
	int partnerIndex = findPartnerIndexInRooms(*rooms, i);
	int isHit = data.common_data.isHit;
	sendIsHitNotificationPackage(client_sock[partnerIndex], isHit);

}

void handleLoseNotificationPackage(int i, Rooms* rooms, int client_sock[]) {
	int partnerIndex = findPartnerIndexInRooms(*rooms, i);
	sendLoseNotificationPackage(client_sock[partnerIndex]);
	printf("send\n");
}

void emptyBattleInRoom(int i, Rooms *rooms, sockaddr_list *client_sockaddr_list) {
	int j, partnerIndex;
	for (j = 0; j < 3; j++) {
		if (i == rooms->battle[j].players[0].player_index) {
			partnerIndex = rooms->battle[j].players[1].player_index;
			rooms->battle[j].state = EMPTY;
			rooms->battle[j].players[0].player_index = -1;
			rooms->battle[j].players[1].player_index = -1;
			client_sockaddr_list->client[i].state = ROOM_ONLINE;
			client_sockaddr_list->client[partnerIndex].state = ROOM_ONLINE;
			return;
		} else if (i == rooms->battle[j].players[1].player_index) {
			partnerIndex = rooms->battle[j].players[0].player_index;
			rooms->battle[j].state = EMPTY;
			rooms->battle[j].players[0].player_index = -1;
			rooms->battle[j].players[1].player_index = -1;
			client_sockaddr_list->client[i].state = ROOM_ONLINE;
			client_sockaddr_list->client[partnerIndex].state = ROOM_ONLINE;
			return;
		}
	}
}

void initCWClientList(Confirm_waiting_clients *CWClients) {
	int i;
	for (i = 0; i < 3; i++) {
		CWClients->confirm_clients[i] = -1;
	}
}

int getIndexInCWClientList(int clientOrder, Confirm_waiting_clients CWClients) {
	int i = 0;
	printf("find %d : \n", clientOrder);
	for (i = 0; i < 3; i++) {
//		printf(" i : %d \n",CWClients.confirm_clients[i]);
		if (clientOrder == CWClients.confirm_clients[i])
			return i;
	}
	return -1;
}

int getEmptySpaceIndexInCWClientList(Confirm_waiting_clients CWClients) {
	int i = 0;
	for (i = 0; i < 3; i++) {
		if (-1 == CWClients.confirm_clients[i])
			return i;
	}
	return -1;
}

void resetEmptySpaceInCWClientsIndex(int index,
		Confirm_waiting_clients *CWClients) {
	CWClients->confirm_clients[index] = -1;
	CWClients->waiting_clients[index] = -1;
}

void handleWaitingClientIfNeeded(Confirm_waiting_clients *CWClients,
		int disClientIndex, int client_socks[], sockaddr_list *client_sock_list) {
	int index, waitingClient;
	index = getIndexInCWClientList(disClientIndex, *CWClients);
	if (index == -1)
		return;
	waitingClient = CWClients->waiting_clients[index];
	client_sock_list->client[waitingClient].state = ROOM_ONLINE;
	printf("Send partner disconnected noti to waiting client (client[%d])!\n",
			waitingClient);
	sendPartnerDisconnectedNotification(client_socks[waitingClient]);
	resetEmptySpaceInCWClientsIndex(disClientIndex, CWClients);
}

