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
#ifndef SCENESERVER_H_
#define SCENESERVER_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SceneMenu.h"
#include "SceneArrangement.h"
#include "../Client/Client.h"
#include "../Client/Pack.h"

using namespace std;
using namespace CocosDenshion;
USING_NS_CC;

class SceneServer : public CCLayer {
public:
	virtual void onExit(void);
	virtual bool init(void);
	static CCScene *scene(void);
	CREATE_FUNC(SceneServer);
	static Client *client;

private:
	void initParams(void);
	void createBackground(void);
	void update(void);
	void initClient(void);
	void alertServer(void);
	void hideAlertServer(CCObject *pSender);
	void alertSendRequest(void);
	void hideAlertSendRequest(void);
	void hideAlertFailRequest(CCObject *pSender);
	void alertConfirmRequest(char *name);
	void acceptRequest(CCObject *pSender);
	void denyRequest(CCObject *pSender);
	void goToServer1(CCObject *pSender);
	void goToServer2(CCObject *pSender);
	void goToServer3(CCObject *pSender);
	void goToServer4(CCObject *pSender);
	void goToServer5(CCObject *pSender);
	void goToServer6(CCObject *pSender);
	void backToMenu(CCObject *pSender);
	void refreshSever(CCObject *pSender);

	CCMenu *menu, *menu2;
	CCSprite *spDialog;
	CCArray *lbServers, *lbMenuLabels;
	CCLayerColor *lyColor;
	data_package *data;
	sockaddr_list *list_ip;
	bool canDisplayDialog;
	int accept_index, deny_index;
	const char *cli_name;
};

#endif /* SCENESERVER_H_ */
