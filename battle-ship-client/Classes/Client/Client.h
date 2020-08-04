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
#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Pack.h"

using namespace std;

class Client {
private:
	struct sockaddr_in server;
	struct hostent *hp;
	server_connect_request scr;
	data_package *data;
	sockaddr_list* client_list;
	fd_set readset;
	char buff[4096];
	char choose[300];
	int requestedPlayer, chattingPlayer;
	int sock, n;
	bool err_socket, err_hostname, err_connect, err_send, err_server;

public:
	unsigned int index;
	Client(const char *host_name, int port);
	static Client *create(const char *host_name, int port);
	virtual ~Client(void);
	bool sendData(data_package_type type, const char *name);
	data_package *listen(void);
	void disconnect(void);
	bool getErrSocket(void);
	bool getErrHostName(void);
	bool getErrConnect(void);
	bool getErrSend(void);
	bool getErrServer(void);
	int getSock(void);

	int isSocketAddrEmpty(struct sockaddr_in sockaddr);
	void printClientList(sockaddr_list client_list);
	void printSocketAddressInformation(client_info client);
	void sendServerConnectRequest(int targetSocket, const char *name); //changed
	void sendSocketAddressList(int sockfd, sockaddr_list sock_list, int index);
	void sendInvitePlayerRequest(int sockfd, int player_index, const char *name);
	void sendConfirmPlay(int sockfd, int player_index);
	void sendFailedRequest(int sockfd, int player_index);
	void sendChattingData(int sockfd, char chattingdata[], int player_index);
	void sendPartnerDisconnectedNotification(int targetSock);
	void sendReadyConfirm(int sockfd);
	void sendStartBattleDataPackage(int targetSock);
	void sendReadyPartnerNotificationPackage(int targetSock);
	void sendFirstTurnNotificationPackage(int targetSock);
	void sendSecondTurnNotificationPackage(int targetSock);
	void sendShotDataPackage(int targetSock, int x,int y);
	void sendIsHitNotificationPackage(int targetSock, int isHit);
	void sendLoseNotificationPackage(int targetSock);
	int isClienStateEqual(client_info mClientInfo, client_state state);
	void printClientState(client_state state);
	void println();
	int addClientToBattleRoom(Rooms *room,sockaddr_list *sockList, int client1,int client2);
	void initRooms(Rooms *rooms);
	char* getBattleState(Battle_state state);
	void printRoomStates(Rooms rooms);

};

#endif /* CLIENT_H_ */
