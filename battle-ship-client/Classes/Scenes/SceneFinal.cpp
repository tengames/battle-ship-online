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
#include "Scenes/SceneFinal.h"

bool SceneFinal::isWin = false;

void SceneFinal::onExit(void)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	this->removeAllChildrenWithCleanup(true);
}

// functions
CCScene* SceneFinal::scene(bool isWin)
{
	SceneFinal::isWin = isWin;
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneFinal *layer = SceneFinal::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneFinal::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}

	createBackground();
	// set touchable
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);

	return true;
}

void SceneFinal::createBackground(void)
{
	// init param
	canReturn = true;

	// create background
	CCSprite *spBgGround = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background.jpg"));
	spBgGround->setPosition(ccp(400, 240));
	this->addChild(spBgGround, 0);

	CCSprite *spBgPaper = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_paper.png"));
	spBgPaper->setPosition(ccp(400, 240));
	this->addChild(spBgPaper, 0);

	if (SceneFinal::isWin)
	{
		// play background music
		SimpleAudioEngine::sharedEngine()->playEffect("raw/winsound.mp3");
		/*winner*/
		CCSprite *spWin = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_winner.png"));
		spWin->setPosition(ccp(400, 235));
		this->addChild(spWin, 0);
	}
	else
	{
		// play background music
		SimpleAudioEngine::sharedEngine()->playEffect("raw/loosesound.mp3");
		/*loose*/
		CCSprite *spLoose = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_game_over.png"));
		spLoose->setPosition(ccp(400, 235));
		this->addChild(spLoose, 0);
	}
}

bool SceneFinal::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
	if (canReturn)
	{
		// return server
		CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneServer::scene(), true));
		canReturn = false;
	}
	return true;
}
