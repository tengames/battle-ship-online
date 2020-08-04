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
#include "Scenes/SceneServer.h"

Client *SceneServer::client = NULL;

void SceneServer::onExit(void)
{
	this->unschedule(schedule_selector(SceneServer::update));
	CC_SAFE_RELEASE(lbServers);
	CC_SAFE_RELEASE(lbMenuLabels);
	this->removeAllChildrenWithCleanup(true);
}

// functions
CCScene* SceneServer::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneServer *layer = SceneServer::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneServer::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}
	// init params
	createBackground(); // layer 0
	initParams();

	// update game
	this->schedule(schedule_selector(SceneServer::update));

	return true;
}

void SceneServer::initParams(void)
{
	// initialize flags
	data = NULL;
	list_ip = NULL;
	canDisplayDialog = true;
	accept_index = 0;
	deny_index = 0;
	// initialize client
	initClient();
	// play background music
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("raw/ocean.mp3", true);
}

void SceneServer::initClient(void)
{
	// initialize client
	// TODO:
//	cout << "Ip Server: " << SceneMenu::tfIp->getString() << endl;
	cli_name = SceneMenu::tfName->getString();
	if (strcmp(cli_name, "") == 0) cli_name = "Vô Danh";
	client = Client::create(SceneMenu::tfIp->getString(), SERV_PORT);
	if (client->getErrSocket() || client->getErrHostName() || client->getErrConnect())
	{
		// error socket, host name, connect
		alertServer();
	}
	// send request login
	client->sendData(SERVER_CONNECT_REQUEST, cli_name);
}

void SceneServer::createBackground(void)
{
	// create background
	CCSprite *spBgGround = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background.jpg"));
	spBgGround->setPosition(ccp(400, 240));
	this->addChild(spBgGround, 0);

	CCSprite *spBgPaper = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_paper.png"));
	spBgPaper->setPosition(ccp(400, 240));
	this->addChild(spBgPaper, 0);

	// create buttons
	CCMenuItemImage *btBack = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/back.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/back_press.png"), this, menu_selector(SceneServer::backToMenu));
	btBack->setScale(0.8f);
	btBack->setPosition(ccp(100, 435));

	CCMenuItemImage *btRefresh = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/turn.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/turn_press.png"), this, menu_selector(SceneServer::refreshSever));
	btRefresh->setScale(0.8f);
	btRefresh->setPosition(ccp(745, 435));

	/*create list server*/
	lbServers = CCArray::createWithCapacity(6);
	lbServers->retain();

	lbMenuLabels = CCArray::createWithCapacity(6);
	lbMenuLabels->retain();

	for (unsigned int i = 0; i < 6; i++)
	{
		// initialize labels
		CCLabelTTF *lbServer = CCLabelTTF::create("<<<Empty server!>>>", "fonts/OctemberScriptVi.ttf", 32);
		lbServer->setHorizontalAlignment(kCCTextAlignmentCenter);
		lbServer->setColor(ccBLACK);
		lbServers->addObject(lbServer);
		// initialize buttons
		SEL_MenuHandler menu_handler;
		switch (i)
		{
		case 0:
			menu_handler = menu_selector(SceneServer::goToServer1);
			break;

		case 1:
			menu_handler = menu_selector(SceneServer::goToServer2);
			break;

		case 2:
			menu_handler = menu_selector(SceneServer::goToServer3);
			break;

		case 3:
			menu_handler = menu_selector(SceneServer::goToServer4);
			break;

		case 4:
			menu_handler = menu_selector(SceneServer::goToServer5);
			break;

		case 5:
			menu_handler = menu_selector(SceneServer::goToServer5);
			break;
		}

		CCMenuItemLabel *lbMn = CCMenuItemLabel::create(lbServer, this, menu_handler);
		lbMn->setPosition(ccp(400, 348-52*i));
		lbMenuLabels->addObject(lbMn);
	}

	menu = CCMenu::createWithArray(lbMenuLabels);
	menu->setPosition(CCPointZero);
	menu2 = CCMenu::create(btRefresh, btBack, NULL);
	menu2->setPosition(CCPointZero);
	this->addChild(menu, 0);
	this->addChild(menu2, 0);
}

void SceneServer::update(void)
{
	// check connect to server
	if (client->getErrServer())
	{
		if (canDisplayDialog)
		{
			// show dialog
			alertServer();
			canDisplayDialog = false;
		}
	}
	data = client->listen();
	if (data != NULL)
	{
		/*get client index*/
		client->index = data->common_data.sock_list.index;
		switch (data->data_type)
		{
		case CLIENT_LIST:
			list_ip = &data->common_data.sock_list;
			if (list_ip != NULL)
			{
				/*display server*/
				char temp[30];
				struct sockaddr_in clientaddr;
				const char *ip;
				int port;

				for (unsigned int i = 0; i < lbServers->count(); i++)
				{
					CCLabelTTF *lbServer = dynamic_cast<CCLabelTTF*>(lbServers->objectAtIndex(i));
					CCMenuItemLabel *lbMenu = dynamic_cast<CCMenuItemLabel*>(lbMenuLabels->objectAtIndex(i));
					lbMenu->setEnabled(true);
					lbServer->setHorizontalAlignment(kCCTextAlignmentCenter);
					memcpy(&clientaddr,&list_ip->client[i],sizeof(SA));
					ip = inet_ntop(AF_INET, &clientaddr.sin_addr, temp, INET_ADDRSTRLEN);
					port = ntohs(clientaddr.sin_port);
					sprintf(temp, "%s - %d", ip, port);
					if (strcmp(temp, "0.0.0.0 - 0") == 0)
					{
						// set touch false
						lbMenu->setEnabled(false);
						lbMenu->setString("<<<Empty server!>>>");
					}
					else if (list_ip->client[i].state == BUILD_MAP)
					{
						lbMenu->setEnabled(false);
					}
					else if (list_ip->client[i].state == WAIT_RESPOND)
					{
						lbMenu->setEnabled(false);
					}
					else if (list_ip->client[i].state == BATTLING)
					{
						lbMenu->setEnabled(false);
					}
					else
					{
						if (client->index == i)
						{
							lbServer->setString("<<<         YOU        >>>");
							// set touch false
							lbMenu->setEnabled(false);
						}
						else
						{
							lbServer->setString((const char*)(&list_ip->client[i].name));
							lbServer->setColor(ccBLUE);
						}
					}
				}
			}
			break;

		case INVITE_PLAY:
			client->sendServerConnectRequest(client->getSock(), cli_name);
			alertConfirmRequest(data->common_data.cli_info.name);
			deny_index = data->common_data.cli_info.index;
			accept_index = data->common_data.cli_info.index;
			break;

		case CONFIRM_INVITATION:
			// accept request, change scene
			CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneArrangement::scene(), false));
			break;

		case INVITE_FAILED:
			accept_index = data->common_data.cli_info.index;
			hideAlertSendRequest();
			break;

		case PARTNER_DISCONNECTED_NOTI:
			hideAlertSendRequest();
			break;

		default:
			break;
		}
	}
}

void SceneServer::alertSendRequest(void)
{
	// disable touch in menu
	menu->setTouchEnabled(false);
	menu2->setTouchEnabled(false);
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor, 1);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	// text for dialog

	CCLabelTTF *lbAlert = CCLabelTTF::create("Đang gửi lời mời!", "fonts/OctemberScriptVi.ttf", 32);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 180));
	spDialog->addChild(lbAlert);

}

void SceneServer::hideAlertSendRequest(void)
{
	// delete old dialog
	this->removeChild(lyColor);
	this->removeChild(spDialog);
	// create new
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor, 1);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	// text for dialog
	char str_client[80];
	sprintf(str_client, "%s", "Đối phương đã từ chối chơi!");
	CCLabelTTF *lbAlert = CCLabelTTF::create(str_client, "fonts/OctemberScriptVi.ttf", 28);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneServer::hideAlertFailRequest));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326, 120));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneServer::hideAlertFailRequest(CCObject *pSender)
{
	// play background music
	SimpleAudioEngine::sharedEngine()->playEffect("raw/ocean.mp3", true);
	// remove dialog
	this->removeChild(lyColor);
	this->removeChild(spDialog);
	// enable menu
	menu->setTouchEnabled(true);
	menu2->setTouchEnabled(true);
}

void SceneServer::alertConfirmRequest(char *name)
{
	// disable touch in menu
	menu->setTouchEnabled(false);
	menu2->setTouchEnabled(false);
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor, 1);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	// text for dialog
	char str_client[80];
	sprintf(str_client, "%s %s", name, "gửi lời mời!");
	CCLabelTTF *lbAlert = CCLabelTTF::create(str_client, "fonts/OctemberScriptVi.ttf", 30);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneServer::acceptRequest));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326-120, 120));

	CCMenuItemImage *btNo = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/no.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/no_press.png"), this, menu_selector(SceneServer::denyRequest));
	btNo->setScale(0.8f);
	btNo->setPosition(ccp(326+120, 120));

	CCMenu *menu = CCMenu::create(btYes, btNo, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);

}

void SceneServer::acceptRequest(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// accept request
	client->sendConfirmPlay(client->getSock(), accept_index);
	// change scene
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneArrangement::scene(), false));
}

void SceneServer::denyRequest(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// deny request
	client->sendFailedRequest(client->getSock(), accept_index);
	// hide alert
	this->removeChild(lyColor);
	this->removeChild(spDialog);
	// enable menu
	menu->setTouchEnabled(true);
	menu2->setTouchEnabled(true);
}

void SceneServer::alertServer(void)
{
	// clear server
	for (unsigned int i = 0; i < lbServers->count(); i++)
	{
		CCLabelTTF *lbServer = dynamic_cast<CCLabelTTF*>(lbServers->objectAtIndex(i));
		lbServer->setString("");
	}
	// disable touch in menu
	menu->setTouchEnabled(false);
	menu2->setTouchEnabled(false);
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor, 1);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	// text for dialog
	CCLabelTTF *lbAlert = CCLabelTTF::create("Không thể kết nối đến Server!", "fonts/OctemberScriptVi.ttf", 28);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneServer::hideAlertServer));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326, 120));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneServer::hideAlertServer(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// touch enable
	menu->setTouchEnabled(false);
	menu2->setTouchEnabled(true);
	this->removeChild(lyColor);
	this->removeChild(spDialog);
}

void SceneServer::goToServer1(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 0, cli_name);
	alertSendRequest();
}

void SceneServer::goToServer2(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 1, cli_name);
	alertSendRequest();
}

void SceneServer::goToServer3(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 2, cli_name);
	alertSendRequest();
}

void SceneServer::goToServer4(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 3, cli_name);
	alertSendRequest();
}

void SceneServer::goToServer5(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 4, cli_name);
	alertSendRequest();
}

void SceneServer::goToServer6(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	client->sendInvitePlayerRequest(client->getSock(), 5, cli_name);
	alertSendRequest();
}

void SceneServer::backToMenu(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// disconnect
	client->disconnect();
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneMenu::scene(), true));
}

void SceneServer::refreshSever(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// check error
	if (client->getErrConnect())
	{
		// touch enable
		menu->setTouchEnabled(true);
		client->disconnect();
		initClient();
	}
	else
	{
		if (client->getErrServer())
		{
			// touch enable
			menu->setTouchEnabled(true);
			client->disconnect();
			initClient();
		}
		else
		{
			bool is_sent = false;
			is_sent = client->sendData(SERVER_CONNECT_REQUEST, cli_name);
			if (!is_sent)
			{
				alertServer();
			}
			else
			{
				// touch enable
				menu->setTouchEnabled(true);
			}
		}
	}
	// display dialog
	canDisplayDialog = true;
}
