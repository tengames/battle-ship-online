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
#include "Scenes/SceneAbout.h"

void SceneAbout::onExit(void)
{
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	this->removeAllChildrenWithCleanup(true);
}

// functions
CCScene* SceneAbout::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneAbout *layer = SceneAbout::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneAbout::init(void)
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

void SceneAbout::createBackground(void)
{
	// init param
	canReturn = true;

	// play background music
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("raw/ocean.mp3", true);

	// create background
	CCSprite *spBgGround = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background.jpg"));
	spBgGround->setPosition(ccp(400, 240));
	this->addChild(spBgGround, 0);

	CCSprite *spBgPaper = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_paper.png"));
	spBgPaper->setPosition(ccp(400, 240));
	this->addChild(spBgPaper, 0);

	// create text
	const char *str ="kong<tengames.inc@gmail.com>";
	CCLabelTTF *lbAbout = CCLabelTTF::create(str, "fonts/UVF DK Crayon Crumble.ttf", 32);
	lbAbout->setPosition(ccp(400, 240));
	lbAbout->setColor(ccBLUE);
	this->addChild(lbAbout);

}

bool SceneAbout::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
	return true;
}

void SceneAbout::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
{
	if (canReturn)
	{
		// return menu
		CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneMenu::scene(), true));
		canReturn = false;
	}
}
