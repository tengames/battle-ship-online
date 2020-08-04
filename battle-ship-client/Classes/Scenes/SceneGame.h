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
#ifndef SCENEGAME_H_
#define SCENEGAME_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SceneServer.h"
#include "SceneFinal.h"
#include "../Objects/ObjectSprite.h"
#include "../Objects/Mine.h"
#include "../Objects/Map.h"
#include "../Objects/Ship.h"
#include "../Client/Pack.h"

using namespace std;
using namespace CocosDenshion;
USING_NS_CC;

class SceneGame : public CCLayer {
public:
	virtual void onExit(void);
	virtual bool init(void);
	static CCScene *scene(void);
	CREATE_FUNC(SceneGame);

private:
	virtual bool ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent);
	virtual void ccTouchMoved(CCTouch * pTouch, CCEvent * pEvent);
	virtual void ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent);

	void initParams(void);
	void createBackground(void);
	void showShips(void);
	void createMine(CCPoint location);
	bool checkInSide(CCPoint location);
	bool checkExistMine(int i, int j);
	bool checkFinish(void);
	void alertFinish(bool isWin);
	void update(float deltaTime);
	void backToServer(CCObject *pSender);
	void goToWin(CCObject *pSender);
	void goToLoose(CCObject *pSender);
	void alertServer(const char *str);
	bool checkHit(int i, int j);

	CCArray *mines;
	CCMenu *menu;
	CCLabelTTF *lbTurn;
	CCSprite *spDialog;
	CCLayerColor *lyColor;
	CCFiniteTimeAction *acFinish;
	CCPoint shootPoint, shootLocation;
	Map *map, *map_battle;
	Mine *mine_server;
	ObjectSprite *view_finder;
	data_package *data;
	int x_ser, y_ser, count_lost;
	bool isTouch, isFinish, canShoot, canDisplayDialog, canListen;
};

#endif /* SCENEGAME_H_ */
