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
#include "Scenes/SceneArrangement.h"

CCArray *SceneArrangement::ships = NULL;

void SceneArrangement::onExit(void)
{
	this->unschedule(schedule_selector(SceneArrangement::update));
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CC_SAFE_RELEASE(mdShips);
	this->removeAllChildrenWithCleanup(true);
}

// functions
CCScene* SceneArrangement::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneArrangement *layer = SceneArrangement::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneArrangement::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}
	// init params
	createBackground();
	initParams();

	// set touchable
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	// update game
	this->schedule(schedule_selector(SceneArrangement::update));

	return true;
}

void SceneArrangement::initParams(void)
{
	// play background music
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("raw/ocean.mp3", true);
	// initialize map
	map = new Map(39.6f, 360.72f, 32);

	// initialize params
	isTouch = false;
	canDisplayDialog = true;
	canListen = true;
	numShip1 = 2;
	numShip2 = 2;
	numShip3 = 2;
	numShip4 = 2;

	// lb text
	lbText = CCLabelTTF::create("Đối phương đã sẵn sàng!", "fonts/OctemberScriptVi.ttf", 28);
	lbText->setColor(ccRED);
	lbText->setPosition(ccp(400, 432));
	lbText->setScale(0);
	lbText->runAction(CCRepeatForever::create(CCFadeOut::create(1)));
	this->addChild(lbText, 1);

	// initialize models
	Ship *modelShip1 = Ship::create(ccp(600, 346-20), Ship::TYPE_1);
	this->addChild(modelShip1, 1);

	Ship *modelShip2 = Ship::create(ccp(584-16, 280-20), Ship::TYPE_2);
	this->addChild(modelShip2, 1);

	Ship *modelShip3 = Ship::create(ccp(568-35, 210-20), Ship::TYPE_3);
	this->addChild(modelShip3, 1);

	Ship *modelShip4 = Ship::create(ccp(552-52, 143-20), Ship::TYPE_4);
	this->addChild(modelShip4, 1);

	/*create text for number of models*/
	char str[100] = {0};
	sprintf(str, "%s  %d", "x", numShip1);
	lbMdShip1 = CCTextFieldTTF::create(str, "fonts/OctemberScriptVi.ttf", 32);
	lbMdShip1->setPosition(ccp(700, 346));
	lbMdShip1->setColor(ccBLACK);
	this->addChild(lbMdShip1, 0);

	sprintf(str, "%s  %d", "x", numShip2);
	lbMdShip2 = CCTextFieldTTF::create(str, "fonts/OctemberScriptVi.ttf", 32);
	lbMdShip2->setPosition(ccp(700, 280));
	lbMdShip2->setColor(ccBLACK);
	this->addChild(lbMdShip2, 0);

	sprintf(str, "%s  %d", "x", numShip3);
	lbMdShip3 = CCTextFieldTTF::create(str, "fonts/OctemberScriptVi.ttf", 32);
	lbMdShip3->setPosition(ccp(700, 210));
	lbMdShip3->setColor(ccBLACK);
	this->addChild(lbMdShip3, 0);

	sprintf(str, "%s  %d", "x", numShip4);
	lbMdShip4 = CCTextFieldTTF::create(str, "fonts/OctemberScriptVi.ttf", 32);
	lbMdShip4->setPosition(ccp(700, 143));
	lbMdShip4->setColor(ccBLACK);
	this->addChild(lbMdShip4, 0);

	// initialize ships
	ships = CCArray::create();
	ships->retain();

	mdShips = CCArray::create(modelShip1, modelShip2, modelShip3, modelShip4, NULL);
	mdShips->retain();

}

void SceneArrangement::createBackground(void)
{
	// create background
	CCSprite *spBgGround = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background.jpg"));
	spBgGround->setPosition(ccp(400, 240));
	this->addChild(spBgGround, 0);

	CCSprite *spBgPaper = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_paper.png"));
	spBgPaper->setPosition(ccp(400, 240));
	this->addChild(spBgPaper, 0);

	CCSprite *spGrid = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/grid.png"));
	spGrid->setPosition(ccp(200, 200));
	this->addChild(spGrid, 0);

	// create buttons
	CCMenuItemImage *btBack = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/back.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/back_press.png"), this, menu_selector(SceneArrangement::backToServer));
	btBack->setScale(0.8f);
	btBack->setPosition(ccp(100, 435));

	CCMenuItemImage *btTurn = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/turn.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/turn_press.png"), this, menu_selector(SceneArrangement::turnShip));
	btTurn->setScale(0.7f);
	btTurn->setPosition(ccp(420+8, 50));

	CCMenuItemImage *btPlay = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/play.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/play_press.png"), this, menu_selector(SceneArrangement::goToGame));
	btPlay->setScale(0.8f);
	btPlay->setPosition(ccp(710-32, 50));

	menu = CCMenu::create(btBack, btPlay, btTurn, NULL);
	menu->setPosition(CCPointZero);
	this->addChild(menu, 0);
}


void SceneArrangement::update(float deltaTime)
{
	// check connect to server
	if (SceneServer::client->getErrServer())
	{
		if (canDisplayDialog)
		{
			// show dialog
			alertServer("Không thể kết nối đến Server!");
			canDisplayDialog = false;
		}
	}
	if (canListen) data = SceneServer::client->listen();
	if (canListen && data != NULL)
	{
		switch (data->data_type)
		{
		case START_BATTLE:
			// change scene
			CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneGame::scene(), false));
			canListen = false;
			break;

		case READY_PARTNER_NOTI:
			// one person ready
			lbText->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1)));
			break;

		case PARTNER_DISCONNECTED_NOTI:
			alertServer("Đối phương đã thoát!");
			break;

		default:
			break;
		}
	}
}

bool SceneArrangement::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
	if (isTouch) return false;
	isTouch = true;
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);

	/*choose model*/
	for (unsigned int i = 0; i < mdShips->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(mdShips->objectAtIndex(i));
		if (ship->boundingBox().containsPoint(location))
		{
			/*create new ship if number of ships > 0*/
			switch (ship->getType())
			{
			select_ship = NULL;
			case Ship::TYPE_1:
				if (numShip1 > 0) createShip(ship);
				break;

			case Ship::TYPE_2:
				if (numShip2 > 0) createShip(ship);
				break;

			case Ship::TYPE_3:
				if (numShip3 > 0) createShip(ship);
				break;

			case Ship::TYPE_4:
				if (numShip4 > 0) createShip(ship);
				break;

			default:
				break;
			}
		}
	}

	/*choose ship*/
	for (unsigned int i = 0; i < ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(ships->objectAtIndex(i));
		ship->setTouch(false);
		if (ship->boundingBox().containsPoint(location))
		{
			ship->setTouch(true);
		}
	}

	return true;
}

void SceneArrangement::ccTouchMoved(CCTouch * pTouch, CCEvent * pEvent)
{
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);

	if (select_ship)
	{
		if (!select_ship->getisSet())
		{
			select_ship->setPosition(ccp(location.x-select_ship->getSize().width/2, location.y-select_ship->getSize().height/2));
			select_ship->setColor(ccBLUE);
			/*check outside*/
			if (checkOutSide(select_ship))
			{
				select_ship->setColor(ccRED);
			}
		}
	}

	/*move ship*/
	for (unsigned int i = 0; i < ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(ships->objectAtIndex(i));
		if (ship->getTouch())
		{
			/*check outside*/
			ship->setoldCoorI(ship->getcoorI());
			ship->setoldCoorJ(ship->getcoorJ());
			if (!ship->getRotated()) ship->setPosition(ccp(location.x-ship->getSize().width/2, location.y-ship->getSize().height/2));
			else ship->setPosition(ccp(location.x, location.y-ship->getSize().width/2));
		}
	}
}

void SceneArrangement::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
{
	isTouch = false;

	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);

	/*set select_ship*/
	if (select_ship)
	{
		/*check outside*/
		if (checkOutSide(select_ship))
		{
			/*check if select_ship is set*/
			if (select_ship->getisSet())
			{
				// increase ship
				increaseShips(select_ship);
				// remove itself
				select_ship->remove(select_ship);
				// remove top of ship
				ships->removeLastObject(true);
			}
			// remove ship
			this->removeChild(select_ship);
		}
		else
		{
			/*create new ship*/
			select_ship->stopAllActions();
			if (!select_ship->getisSet())
			{
				CCFiniteTimeAction *acChoice = CCSequence::create(
						CCFadeIn::create(0.2f),
						CCEaseBackOut::create(CCScaleTo::create(0.25f, 1.2f)),
						CCEaseBackIn::create(CCScaleTo::create(0.25f, 0.8f)),
						NULL);
				select_ship->runAction(acChoice);
				// decrease ship
				select_ship->setisSet(true);
				decreaseShips();

				int i = (int)map->convertToMap(select_ship->getPositionX(), select_ship->getPositionY()).x;
				int j = (int)map->convertToMap(select_ship->getPositionX(), select_ship->getPositionY()).y;
				select_ship->setcoorI(i);
				select_ship->setcoorJ(j);
				CCPoint position;
				position.x = map->convertToWorld(i, j).x;
				position.y = map->convertToWorld(i, j).y;
				select_ship->setPosition(position);
			}
			ships->addObject(select_ship);
		}
	}

	/*move ship*/
	for (unsigned int i = 0; i < ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(ships->objectAtIndex(i));
		if (ship->getTouch())
		{
			/*set position follow dragg*/
			if (!ship->getRotated()) ship->setPosition(ccp(location.x-ship->getSize().width/2, location.y-ship->getSize().height/2));
			else ship->setPosition(ccp(location.x, location.y-ship->getSize().width/2));
			/*check outside*/
			if (checkOutSide(ship))
			{
				CCPoint position;
				position.x = map->convertToWorld(ship->getoldCoorI(), ship->getoldCoorJ(), ship->getRotated()).x;
				position.y = map->convertToWorld(ship->getoldCoorI(), ship->getoldCoorJ(), ship->getRotated()).y;

				ship->setPosition(position);
			}
			else
			{
				int i = (int)map->convertToMap(ship->getPositionX(), ship->getPositionY()).x;
				int j = (int)map->convertToMap(ship->getPositionX(), ship->getPositionY()).y;
				ship->setcoorI(i);
				ship->setcoorJ(j);
				// for display
				CCPoint position;
				position.x = map->convertToWorld(i, j, ship->getRotated()).x;
				position.y = map->convertToWorld(i, j, ship->getRotated()).y;

				ship->setPosition(position);
			}
		}
	}
}

void SceneArrangement::createShip(Ship *ship)
{
	select_ship = Ship::create(ship->getPosition(), ship->getType());
	select_ship->setScale(0);
	CCActionInterval *acChoice = CCEaseBackOut::create(CCScaleTo::create(0.5f, 1));
	CCActionInterval *acFade = CCRepeatForever::create(CCFadeOut::create(1));
	select_ship->runAction(acChoice);
	select_ship->runAction(acFade);
	this->addChild(select_ship, 1);
}

void SceneArrangement::increaseShips(Ship *select_ship)
{
	char str[100] = {0};
	switch (select_ship->getType())
	{
	case Ship::TYPE_1:
		numShip1++;
		sprintf(str, "%s  %d", "x", numShip1);
		lbMdShip1->setString(str);
		break;

	case Ship::TYPE_2:
		numShip2++;
		sprintf(str, "%s  %d", "x", numShip2);
		lbMdShip2->setString(str);
		break;

	case Ship::TYPE_3:
		numShip3++;
		sprintf(str, "%s  %d", "x", numShip3);
		lbMdShip3->setString(str);
		break;

	case Ship::TYPE_4:
		numShip4++;
		sprintf(str, "%s  %d", "x", numShip4);
		lbMdShip4->setString(str);
		break;
	}

}

void SceneArrangement::decreaseShips(void)
{
	char str[100] = {0};
	switch (select_ship->getType())
	{
	case Ship::TYPE_1:
		numShip1--;
		sprintf(str, "%s  %d", "x", numShip1);
		lbMdShip1->setString(str);
		break;

	case Ship::TYPE_2:
		numShip2--;
		sprintf(str, "%s  %d", "x", numShip2);
		lbMdShip2->setString(str);
		break;

	case Ship::TYPE_3:
		numShip3--;
		sprintf(str, "%s  %d", "x", numShip3);
		lbMdShip3->setString(str);
		break;

	case Ship::TYPE_4:
		numShip4--;
		sprintf(str, "%s  %d", "x", numShip4);
		lbMdShip4->setString(str);
		break;
	}
}

void SceneArrangement::turnShip(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	/*turn ship*/
	for (unsigned int i = 0; i < ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(ships->objectAtIndex(i));
		if (ship->getTouch())
		{
			ship->turn(true);
			if (checkOutSide(ship))
			{
				ship->turn(false);
			}
		}
	}
}

bool SceneArrangement::checkOutSide(Ship *select_ship)
{
	/*check out side desk*/
	if (select_ship)
	{
		if (!select_ship->getRotated())
		{
			if (select_ship->getPositionX() < 25
					|| select_ship->getPositionX()+select_ship->getSize().width > 420
					|| select_ship->getPositionY() < 15
					|| select_ship->getPositionY()+select_ship->getSize().height > 380)
			{
				return true;
			}
		}
		else
		{
			// TODO:
			if (select_ship->getPositionX() < 60
					|| select_ship->getPositionY()+select_ship->getSize().height > 268
					|| select_ship->getPositionY() < 15
					|| select_ship->getPositionX()+select_ship->getSize().width > 550)
			{
				return true;
			}
		}
	}
	return false;
}

void SceneArrangement::showDialog(void)
{
	// disalbe touch in screen
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	// disable touch in menu
	menu->setTouchEnabled(false);
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
	CCLabelTTF *lbAlert = CCLabelTTF::create("Bạn phải xếp hết các tàu trước!", "fonts/OctemberScriptVi.ttf", 28);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneArrangement::hideDialog));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326, 120));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneArrangement::hideDialog(CCObject *pSender)
{
	// enable touch to scene and menu
	menu->setTouchEnabled(true);
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	// hide dialog
	this->removeChild(lyColor);
	this->removeChild(spDialog);
}

void SceneArrangement::readyDialog(void)
{
	// disalbe touch in screen
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	// disable touch in menu
	menu->setTouchEnabled(false);
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
	CCLabelTTF *lbAlert = CCLabelTTF::create("SẴN SÀNG!", "fonts/OctemberScriptVi.ttf", 50);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 180));
	spDialog->addChild(lbAlert);
}

void SceneArrangement::alertServer(const char *str)
{
	// disable touch in menu
	menu->setTouchEnabled(false);
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
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
	CCLabelTTF *lbAlert = CCLabelTTF::create(str, "fonts/OctemberScriptVi.ttf", 28);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneArrangement::backToServer));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326, 120));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneArrangement::backToServer(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// close socket
	SceneServer::client->disconnect();
	// safe remove array
	CC_SAFE_RELEASE(ships);
	// change scene
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneServer::scene(), true));
}

void SceneArrangement::goToGame(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	/*check number of ships*/
	if (numShip1 > 0 || numShip2 > 0 || numShip3 > 0 || numShip4 > 0)
	{
		showDialog();
	}
	else
	{
		/*can play*/
		// set hp for each ship
		for (unsigned int i = 0; i < ships->count(); i++)
		{
			Ship *ship = dynamic_cast<Ship*>(ships->objectAtIndex(i));
			ship->setHpFinish();
		}
		// send ready state
		readyDialog();
		SceneServer::client->sendReadyConfirm(SceneServer::client->getSock());
	}
}
