#ifndef PACK_H
#define PACK_H
#define SPACE_AREA "\t\t\t"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#define SERV_PORT 5000
#define LISTEN_QUEUE 10
#define MAXLINE 1024
#define CLIENT_SIZE 6
#define CHATTING_SIZE 200

typedef enum _client_state {
	ROOM_ONLINE, WAIT_RESPOND, CONFIRM_PLAY_REQUEST, BUILD_MAP, READY, BATTLING, END_GAME
} client_state;

typedef enum _battle_state{
	EMPTY, BUILD_MAP_PHASE, BATTLING_PHASE
}Battle_state;


typedef enum _data_package_type {
	SERVER_CONNECT_REQUEST,
	CLIENT_LIST,
	INVITE_PLAY,
	CONFIRM_INVITATION,
	INVITE_FAILED,
	CHATTING_DATA,
	PARTNER_DISCONNECTED_NOTI,
	READY_CONFIRM,
	READY_PARTNER_NOTI,
	START_BATTLE,
	SHOT_DATA,
	FIRST_TURN,
	SECOND_TURN,
	LOSE_NOTI,
	IS_HIT_NOTI,
	FAILED_CONFIRM,
} data_package_type;

typedef struct _shot_data{
	int x;
	int y;
}Shot_data;

typedef struct _room_player_info{
	int player_index;
	client_state state;
}Room_Player;

typedef struct _Battle{
	Room_Player players[2];
	Battle_state state;
}Battle;

typedef struct _rooms {
	Battle battle[3];
} Rooms;


typedef struct _sv_connect_request {
	int request; // 0 neu muon ngat ket noi - xoa khoi danh sach, 1 neu muon ket noi den server - hien len danh sach
} server_connect_request;

typedef struct sockaddr_in SA;

/*changed*/
typedef struct _client_info {
	SA client_sock_addr;
	client_state state;
	char name[30];
	int index;
} client_info;

typedef struct _ip_list {
	client_info client[CLIENT_SIZE];
	int index;
} sockaddr_list;

typedef struct _chatting_data {
	char data[CHATTING_SIZE];
	int destinationPlayer;
} chatting_data;

/* changed */
typedef union _common_data_struct {
	server_connect_request scr;
	sockaddr_list sock_list;
//	int request_player_index;
	int confirm_player_index;
	chatting_data chat_data;
	Shot_data shot_data;
	int isHit;
	client_info cli_info;
} common_data_struct;

/*
 * data type :
 */

typedef struct _data_package {
	data_package_type data_type;
	common_data_struct common_data;
} data_package;


//int addClientAddressToArray(sockaddr_list *client_list, struct sockaddr_in clientaddr);
int isSocketAddrEmpty(struct sockaddr_in sockaddr);
void printClientList(sockaddr_list client_list);
void printSocketAddressInformation(client_info client);
void sendServerConnectRequest(int targetSocket,char name[]); //changed
void sendSocketAddressList(int sockfd, sockaddr_list sock_list, int index);
void sendInvitePlayerRequest(int sockfd, int player_index,char name[]);
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


#endif
