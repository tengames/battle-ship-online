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
#ifndef SCENEMENU_H_
#define SCENEMENU_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SceneLoading.h"
#include "SceneServer.h"
#include "SceneAbout.h"
#include "../Objects/ObjectSprite.h"

using namespace std;
using namespace CocosDenshion;
USING_NS_CC;

class SceneMenu : public CCLayer, public CCIMEDelegate, public CCTextFieldDelegate {
public:
	virtual bool init(void);
	virtual void onExit(void);
	static CCScene *scene(void);
	CREATE_FUNC(SceneMenu);
	static CCTextFieldTTF *tfIp, *tfName;

private:
    // CCLayer
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	void initParams(void);
	void createBackground(void);
	void requireIpDialog(CCObject *pSender);
	void confirmExit(CCObject *pSender);
	void cancelExit(CCObject *pSender);
	void exitGame(CCObject* pSender);
	void goSceneGame(CCObject* pSender);
	void goSceneAbout(CCObject *pSender);

	CCLayerColor *lyColor;
	CCSprite *spDialog;
	CCMenu *menu;
	bool isTouch;
};

#endif /* SCENEMENU_H_ */
