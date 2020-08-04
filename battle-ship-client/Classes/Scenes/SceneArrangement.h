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
#ifndef SCENEARRANGEMENT_H_
#define SCENEARRANGEMENT_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SceneServer.h"
#include "SceneGame.h"
#include "../Objects/Ship.h"
#include "../Objects/Map.h"
#include "../Objects/HpCoor.h"
#include "../Client/Pack.h"

using namespace std;
using namespace CocosDenshion;
USING_NS_CC;

class SceneArrangement : public CCLayer {
public:
	virtual void onExit(void);
	virtual bool init(void);
	static CCScene *scene(void);
	CREATE_FUNC(SceneArrangement);
	static CCArray *ships;

private:
	virtual bool ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent);
	virtual void ccTouchMoved(CCTouch * pTouch, CCEvent * pEvent);
	virtual void ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent);
	void initParams(void);
	void createBackground(void);
	void update(float deltaTime);
	void createShip(Ship *ship);
	void increaseShips(Ship *select_ship);
	void decreaseShips(void);
	void turnShip(CCObject *pSender);
	void showDialog(void);
	void hideDialog(CCObject *pSender);
	void readyDialog(void);
	void backToServer(CCObject *pSender);
	void goToGame(CCObject *pSender);
	bool checkOutSide(Ship *select_ship);
	void alertServer(const char *str);

	CCArray *mdShips;
	CCLabelTTF *lbMdShip1, *lbMdShip2, *lbMdShip3, *lbMdShip4, *lbText;
	CCSprite *spDialog;
	CCLayerColor *lyColor;
	Ship *select_ship;
	CCMenu *menu;
	Map *map;
	data_package *data;
	short numShip1, numShip2, numShip3, numShip4;
	bool isTouch, canDisplayDialog, canListen;
};

#endif /* SCENEARRANGEMENT_H_ */
