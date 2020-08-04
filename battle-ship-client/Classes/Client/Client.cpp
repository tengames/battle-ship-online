/*
The MIT License

Copyright (c) 2013 kong <tengames.inc@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "Client.h"

using namespace std;

Client::Client(const char *host_name, int port)
{
	/*variables*/
	err_connect = false;
	err_hostname = false;
	err_send = false;
	err_socket = false;
	err_server = false;
	index = 0;

	/*initialize*/
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		err_socket = true;
	}
	else
	{
		server.sin_family = AF_INET;
		hp = gethostbyname(host_name);
		if (hp == 0)
		{
			err_hostname = true;
			close(sock);
		}
		else
		{
			memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
			server.sin_port = htons(port);
			if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0)
			{
				err_connect = true;
				close(sock);
			}
		}
	}
}

Client *Client::create(const char *host_name, int port)
{
	Client *client = new Client(host_name, port);
	if (client)
	{
		return client;
	}
	delete (client);
	return NULL;
}

bool Client::sendData(data_package_type type, const char *name)
{
	if (!err_socket && !err_hostname && !err_connect)
	{
		switch (type)
		{
		case SERVER_CONNECT_REQUEST:
			/*send request*/
			sendServerConnectRequest(sock, name);
			return true;

		case INVITE_PLAY:

		default:
			return false;
		}
	}
	return false;
}

data_package *Client::listen(void)
{
	struct timeval timeoutCounter;
	if (!err_socket && !err_hostname && !err_connect)
	{
		timeoutCounter.tv_sec = 0;
		timeoutCounter.tv_usec = 1;
		FD_ZERO(&readset);
		FD_SET(sock, &readset);
		select(sock + 1, &readset, NULL, NULL, &timeoutCounter);
		if (FD_ISSET(sock,&readset)) {
			if ((n = read(sock, buff, MAXLINE)) == 0)
			{
				/*server die*/
				err_server = true;
				return NULL;
			}
			else
			{
				data = (data_package*) buff;
				return data;
			}
		}
	}
	return NULL;
}

void Client::disconnect(void)
{
	if (!err_socket)
		close(sock);
}

bool Client::getErrSocket(void)
{
	return err_socket;
}

bool Client::getErrHostName(void)
{
	return err_hostname;
}

bool Client::getErrConnect(void)
{
	return err_connect;
}

bool Client::getErrSend(void)
{
	return err_send;
}

bool Client::getErrServer(void)
{
	return err_server;
}

int Client::getSock(void)
{
	return sock;
}

Client::~Client(void)
{
	delete (hp);
}

/* process */

void Client::println() {
	printf("\n");
}

void Client::printSocketAddressInformation(client_info client) {
	char temp[30];
	printf(" ip %s, port %d ,state : %d",
			inet_ntop(AF_INET, &client.client_sock_addr.sin_addr, temp,
					INET_ADDRSTRLEN), ntohs(client.client_sock_addr.sin_port),
			client.state);
}


int Client::isSocketAddrEmpty(struct sockaddr_in sockaddr) {
	char temp[30];
	const char *ip = inet_ntop(AF_INET, &sockaddr.sin_addr, temp,
			INET_ADDRSTRLEN);
	return strcmp(ip, "0.0.0.0") == 0;
}

void Client::printClientList(sockaddr_list client_list) {
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
void Client::sendServerConnectRequest(int targetSocket, const char *name) {
	data_package data;
	data.data_type = SERVER_CONNECT_REQUEST;
	strcpy(data.common_data.cli_info.name,name);
	send(targetSocket, (char*) &data, MAXLINE, 0);
}

void Client::sendSocketAddressList(int sockfd, sockaddr_list sock_list, int index) {
	data_package data;
	data.data_type = CLIENT_LIST;
	data.common_data.sock_list = sock_list;
	data.common_data.sock_list.index = index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void Client::sendInvitePlayerRequest(int sockfd, int player_index, const char *name) {
	data_package data;
	data.data_type = INVITE_PLAY;
	data.common_data.cli_info.index = player_index;
	strcpy(data.common_data.cli_info.name,name);
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void Client::sendConfirmPlay(int sockfd, int player_index) {
	data_package data;
	data.data_type = CONFIRM_INVITATION;
	data.common_data.cli_info.index= player_index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void Client::sendFailedRequest(int sockfd, int player_index) {
	data_package data;
	data.data_type = INVITE_FAILED;
	data.common_data.cli_info.index = player_index;
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void Client::sendChattingData(int sockfd, char chattingdata[], int player_index) {
	data_package data;
	data.data_type = CHATTING_DATA;
	data.common_data.chat_data.destinationPlayer = player_index;
	strcpy(data.common_data.chat_data.data, chattingdata);
	send(sockfd, (char*) &data, MAXLINE, 0);
}

void Client::sendPartnerDisconnectedNotification(int targetSock) {
	data_package data;
	data.data_type = PARTNER_DISCONNECTED_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendReadyConfirm(int targetSock) {
	data_package data;
	data.data_type = READY_CONFIRM;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendStartBattleDataPackage(int targetSock) {
	data_package data;
	data.data_type = START_BATTLE;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendReadyPartnerNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = READY_PARTNER_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendFirstTurnNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = FIRST_TURN;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendSecondTurnNotificationPackage(int targetSock) {
	data_package data;
	data.data_type = SECOND_TURN;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendShotDataPackage(int targetSock, int x,int y){
	data_package data;
	data.data_type = SHOT_DATA;
	data.common_data.shot_data.x = x;
	data.common_data.shot_data.y = y;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendIsHitNotificationPackage(int targetSock, int isHit){
	data_package data;
	data.data_type = IS_HIT_NOTI;
	data.common_data.isHit = isHit;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

void Client::sendLoseNotificationPackage(int targetSock){
	data_package data;
	data.data_type = LOSE_NOTI;
	send(targetSock, (char*) &data, MAXLINE, 0);
}

int Client::isClienStateEqual(client_info mClientInfo, client_state state) {
	if (mClientInfo.state == state)
		return 1;
	else
		return 0;
}

void Client::printClientState(client_state state) {
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
int Client::addClientToBattleRoom(Rooms *rooms, sockaddr_list *sockList, int client1,
		int client2) {
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

void Client::initRooms(Rooms *rooms) {
	int i;
	for (i = 0; i < 3; i++) {
		rooms->battle[i].state = EMPTY;
	}
}

char* Client::getBattleState(Battle_state state) {
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

void Client::printRoomStates(Rooms rooms) {
	int i;
	for (i = 0; i < 3; i++) {
		printf("Battle 1 (%s): player %d - player %d \n",
				getBattleState(rooms.battle[i].state),
				rooms.battle[i].players[0].player_index,
				rooms.battle[i].players[1].player_index);
	}
}
