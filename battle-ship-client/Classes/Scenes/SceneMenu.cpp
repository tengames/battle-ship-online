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
#include "Scenes/SceneMenu.h"

CCTextFieldTTF *SceneMenu::tfIp = NULL;
CCTextFieldTTF *SceneMenu::tfName = NULL;

void SceneMenu::onExit(void)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	this->removeAllChildrenWithCleanup(true);
}

CCScene* SceneMenu::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneMenu *layer = SceneMenu::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneMenu::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}
	// init params
	initParams();
	createBackground();
	// set touchable
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	// update game
	this->schedule(schedule_selector(SceneMenu::update));

	return true;
}

void SceneMenu::initParams(void)
{
	isTouch = false;
	// play background music
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("raw/ocean.mp3", true);
}

void SceneMenu::createBackground()
{
	// create background
	CCSprite *spBgGround = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background.jpg"));
	spBgGround->setPosition(ccp(400, 240));
	this->addChild(spBgGround, 0);
	CCSprite *spBgPaper = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_paper.png"));
	spBgPaper->setPosition(ccp(400, 240));
	this->addChild(spBgPaper, 0);
	CCSprite *spBgMenu = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_menu.png"));
	spBgMenu->setPosition(ccp(400, 240));
	this->addChild(spBgMenu, 0);

	// add buttons
	CCMenuItemImage *btStart = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/start.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/start_press.png"), this, menu_selector(SceneMenu::requireIpDialog));
	btStart->setScale(0.8f);
	btStart->setPosition(ccp(400, 160));

	CCMenuItemImage *btSetting = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/setting.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/setting_press.png"), this, menu_selector(SceneMenu::goSceneAbout));
	btSetting->setScale(0.8f);
	btSetting->setPosition(ccp(60, 435));

	CCMenuItemImage *btExit = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/exit.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/exit_press.png"), this, menu_selector(SceneMenu::confirmExit));
	btExit->setScale(0.8f);
	btExit->setPosition(ccp(740, 435));

	menu = CCMenu::create(btStart, btSetting, btExit, NULL);
	menu->setPosition(CCPointZero);
	this->addChild(menu, 0);
}

void SceneMenu::requireIpDialog(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// disable touch in menu
	menu->setTouchEnabled(false);
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.2f));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	/*process text*/
	CCLabelTTF *lbText = CCLabelTTF::create("Mời nhập ip Server", "fonts/OctemberScriptVi.ttf", 20);
	lbText->setPosition(ccp(spDialog->getContentSize().width/2, 260));
	lbText->setColor(ccBLUE);
	spDialog->addChild(lbText, 0);

	CCLabelTTF *lbName = CCLabelTTF::create("Mời nhập Nick Name", "fonts/OctemberScriptVi.ttf", 20);
	lbName->setPosition(ccp(spDialog->getContentSize().width/2, 200-8));
	lbName->setColor(ccBLUE);
	spDialog->addChild(lbName, 0);


	tfIp = CCTextFieldTTF::textFieldWithPlaceHolder("Server Ip", "fonts/UVF DK Crayon Crumble.ttf", 30);
	tfIp->retain();
	tfIp->setPosition(ccp(spDialog->getContentSize().width/2, 230-8));
	tfIp->setColor(ccBLACK);
	spDialog->addChild(tfIp, 0);

	tfName = CCTextFieldTTF::textFieldWithPlaceHolder("Nick Name", "fonts/UVF DK Crayon Crumble.ttf", 30);
	tfName->retain();
	tfName->setPosition(ccp(spDialog->getContentSize().width/2, 200-48));
	tfName->setColor(ccBLACK);
	spDialog->addChild(tfName, 0);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneMenu::goSceneGame));
	btYes->setScale(0.5f);
	btYes->setPosition(ccp(spDialog->getContentSize().width/2, 100-15));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneMenu::confirmExit(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// disable touch in menu
	menu->setTouchEnabled(false);
	// summon color
	lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor);
	// summon dialog
	spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 1);

	ObjectSprite *osText = ObjectSprite::create(ccp(340, 220));
	osText->setRegion("confirm_exit");
	spDialog->addChild(osText, 0);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneMenu::exitGame));
	btYes->setPosition(ccp(190, 120));
	CCMenuItemImage *btNo = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/no.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/no_press.png"), this, menu_selector(SceneMenu::cancelExit));
	btNo->setPosition(ccp(480, 120));

	CCMenu *menu = CCMenu::create(btYes, btNo, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneMenu::cancelExit(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// enable touch in menu
	menu->setTouchEnabled(true);
	// remove dialog
	this->removeChild(lyColor);
	this->removeChild(spDialog);
}

void SceneMenu::exitGame(CCObject* pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// free memory
	SceneLoading::removeAssets();
	// exit
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void SceneMenu::goSceneGame(CCObject* pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneServer::scene(), false));
}

void SceneMenu::goSceneAbout(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneAbout::scene(), false));
}

 // CCLayer
 bool SceneMenu::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
 {
	if (isTouch) return false;
	isTouch = true;
	return true;

 }

 void SceneMenu::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
 {
	isTouch = false;
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);

	// turn on ime
	if (location.x >= 280 && location.x <= 520 && location.y >= 280 && location.y <= 330)
	{
		tfIp->attachWithIME();
	}

	if (location.x >= 280 && location.x <= 520 && location.y >= 190 && location.y <= 245)
	{
		tfName->attachWithIME();
	}
 }
