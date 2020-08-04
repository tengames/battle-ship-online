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
#include "SceneGame.h"

void SceneGame::onExit(void)
{
	this->unschedule(schedule_selector(SceneGame::update));
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CC_SAFE_RELEASE(mines);
	this->removeAllChildrenWithCleanup(true);
}

// functions
CCScene* SceneGame::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneGame *layer = SceneGame::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneGame::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}
	// init params
	initParams();
	createBackground(); // layer 0
	showShips();
	// set touchable
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	// update game
	this->schedule(schedule_selector(SceneGame::update));

	return true;
}

void SceneGame::initParams(void)
{
	isTouch = false;
	isFinish = false;
	canShoot = false;
	canListen = true;
	canDisplayDialog = true;
	shootPoint.x = -1;
	shootPoint.y = -1;
	shootLocation.x = -1;
	shootLocation.y = -1;
	count_lost = 0;
	x_ser = -1;
	y_ser = -1;
	mine_server = NULL;
	// initialize map
	map = new Map(39.6f, 360.72f, 32);
	map_battle = new Map(436.8f, 360.72f+16, 32);
	// initialize mines
	mines = CCArray::create();
	mines->retain();
	// create view finder
	view_finder = ObjectSprite::create(CCPointZero);
	view_finder->setRegion("view_finder");
	view_finder->setScale(0);
	this->addChild(view_finder, 3);
	// play background music
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("raw/xmusic.mp3", true);
}

void SceneGame::createBackground(void)
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

	CCSprite *spGridO = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/grid.png"));
	spGridO->setPosition(ccp(600, 200));
	this->addChild(spGridO, 0);

	// create label
	lbTurn = CCLabelTTF::create("Tới lượt đối thủ!", "fonts/OctemberScriptVi.ttf", 28);
	lbTurn->runAction(CCRepeatForever::create(CCFadeOut::create(1)));
	lbTurn->setPosition(ccp(600, 440));
	lbTurn->setColor(ccRED);
	this->addChild(lbTurn, 0);

	// create buttons
	CCMenuItemImage *btBack = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/back.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/back_press.png"), this, menu_selector(SceneGame::backToServer));
	btBack->setScale(0.8f);
	btBack->setPosition(ccp(100, 435));

	menu = CCMenu::create(btBack, NULL);
	menu->setPosition(CCPointZero);
	this->addChild(menu, 0);
}

void SceneGame::showShips(void)
{
	for (unsigned int i = 0; i < SceneArrangement::ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(SceneArrangement::ships->objectAtIndex(i));
		Ship *temp_ship = Ship::create(ship->getPosition(), ship->getType());
		if (ship->getRotated()) temp_ship->setRotation(-90);
		this->addChild(temp_ship, 0);
	}
	// TODO:
	/*Parsing array, check shoot*/
/*	for (unsigned int i = 0; i < SceneArrangement::ships->count(); i++)
	{
		cout << "Ship: " << i << endl;
		Ship *ship = dynamic_cast<Ship*>(SceneArrangement::ships->objectAtIndex(i));
		for (unsigned int j = 0; j < ship->getHpCoors()->count(); j++)
		{
			HpCoor *hpCoor = dynamic_cast<HpCoor*>(ship->getHpCoors()->objectAtIndex(j));
			cout << "Coor: (" << hpCoor->getI() << ", " << hpCoor->getJ() << ") " << endl;
		}
		cout << "========================" << endl;
	}*/
}

bool SceneGame::checkHit(int x, int y)
{
	/*Parsing array, check shoot*/
	for (unsigned int i = 0; i < SceneArrangement::ships->count(); i++)
	{
		Ship *ship = dynamic_cast<Ship*>(SceneArrangement::ships->objectAtIndex(i));
		for (unsigned int j = 0; j < ship->getHpCoors()->count(); j++)
		{
			HpCoor *hpCoor = dynamic_cast<HpCoor*>(ship->getHpCoors()->objectAtIndex(j));
			if (hpCoor->getI() == x && hpCoor->getJ() == y)
			{
				// set lost
				count_lost++;
				// TODO:
//				cout << "Count: " << count_lost << endl;
				return true;
			}
		}
	}
	return false;
}

bool SceneGame::checkInSide(CCPoint location)
{
	if (location.x >= 434+16 && location.x <= 780 && location.y >= 18 && location.y <= 360)
		return true;
	return false;
}

bool SceneGame::checkExistMine(int i, int j)
{
	for (unsigned int x = 0; x < mines->count(); x++)
	{
		Mine *mine = dynamic_cast<Mine*>(mines->objectAtIndex(x));
		if (mine->getI() == i && mine->getJ() == j)
			return true;
	}
	return false;
}

void SceneGame::createMine(CCPoint location)
{
	if (checkInSide(location))
	{
		int i = map_battle->convertToMap(location.x, location.y).x;
		int j = map_battle->convertToMap(location.x, location.y).y;
		int posX = map_battle->convertToWorld(i, j, true).x;
		int posY = map_battle->convertToWorld(i, j, true).y;
		// check in mines
		if (!checkExistMine(i, j))
		{
			// send coordinate shoot
			SceneServer::client->sendShotDataPackage(SceneServer::client->getSock(), i, j);
			shootPoint.x = i;
			shootPoint.y = j;
			shootLocation.x = posX;
			shootLocation.y = posY;
			// lock shoot
			canShoot = false;
			// TODO:
//			cout << "(I, J) : (" << i << ", " << j << ")"<< endl;
		}
	}
}

bool SceneGame::checkFinish(void)
{
	if (count_lost >= 20) return true;
	return false;
}

void SceneGame::alertFinish(bool isWin)
{
	// disalbe touch in screen
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	// disable touch in menu
	menu->setTouchEnabled(false);
	// summon color
	CCLayerColor *lyColor = CCLayerColor::create(ccc4(0, 0, 0, 128));
	this->addChild(lyColor, 1);
	// summon dialog
	CCSprite *spDialog = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->textureForKey("drawable/background_dialog.png"));
	spDialog->setPosition(ccp(400, 240));
	spDialog->setScale(0);
	CCActionInterval *acDialog = CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1));
	spDialog->runAction(acDialog);
	this->addChild(spDialog, 3);

	// text for dialog
	const char *str;
	if (isWin)
	{
		str = "Đã tiêu diệt toàn bộ tàu địch!";
	}
	else
	{
		str = "Toàn bộ tàu đã bị tiêu diệt!";
	}
	CCLabelTTF *lbAlert = CCLabelTTF::create(str, "fonts/OctemberScriptVi.ttf", 30);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 198));
	spDialog->addChild(lbAlert);
	// disconnect
	canListen = false;
	SceneServer::client->disconnect();
}

void SceneGame::goToWin(CCObject *pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, SceneFinal::scene(true), ccWHITE));
}

void SceneGame::goToLoose(CCObject *pSender)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, SceneFinal::scene(false), ccWHITE));
}

bool SceneGame::ccTouchBegan(CCTouch * pTouch, CCEvent * pEvent)
{
	if (isTouch) return false;
	isTouch = true;
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	/*process*/
	if (canShoot)
	{
		if (checkInSide(location))
		{
			view_finder->setPosition(location);
			// run effect
			view_finder->runAction(CCEaseElasticInOut::create(CCScaleTo::create(0.1f, 1)));
		}
	}
	return true;
}

void SceneGame::ccTouchMoved(CCTouch * pTouch, CCEvent * pEvent)
{
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	/*process*/
	if (canShoot) view_finder->setPosition(location);
}

void SceneGame::ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent)
{
	isTouch = false;
	CCPoint location = pTouch->getLocationInView();
	location = CCDirector::sharedDirector()->convertToGL(location);
	/*process*/

	// make new mine
	if (canShoot)
	{
		createMine(location);
		// for view_finder
		if (checkInSide(location))
		{
			view_finder->setPosition(location);
			view_finder->runAction(CCEaseElasticIn::create(CCScaleTo::create(0.5f, 0)));
		}
		else
		{
			view_finder->setScale(0);
		}
	}
}

void SceneGame::alertServer(const char *str)
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
	this->addChild(spDialog, 3);
	// text for dialog
	CCLabelTTF *lbAlert = CCLabelTTF::create(str, "fonts/OctemberScriptVi.ttf", 28);
	lbAlert->setColor(ccBLUE);
	lbAlert->setPosition(ccp(326, 220));
	spDialog->addChild(lbAlert);

	CCMenuItemImage *btYes = CCMenuItemImage::create(CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes.png"), CCTextureCache::sharedTextureCache()->textureForKey("drawable/yes_press.png"), this, menu_selector(SceneGame::backToServer));
	btYes->setScale(0.8f);
	btYes->setPosition(ccp(326, 120));

	CCMenu *menu = CCMenu::create(btYes, NULL);
	menu->setPosition(CCPointZero);
	spDialog->addChild(menu, 0);
}

void SceneGame::backToServer(CCObject *pSender)
{
	// play button click sound
	SimpleAudioEngine::sharedEngine()->playEffect("raw/buttonclicked.mp3");
	// close socket
	canListen = false;
	SceneServer::client->disconnect();
	// change scene
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneServer::scene(), true));
}


void SceneGame::update(float deltaTime)
{
	if (checkFinish() && !isFinish)
	{
		// disable touch
		menu->setTouchEnabled(false);
		CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
		// process
		// send message
		SceneServer::client->sendLoseNotificationPackage(SceneServer::client->getSock());
		alertFinish(false);
		CCFiniteTimeAction *acFinish = CCSequence::create(CCDelayTime::create(2), CCCallFuncN::create(this, callfuncN_selector(SceneGame::goToLoose)), NULL);
		this->runAction(acFinish);
		isFinish = true;
	}

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
	if (data && canListen)
	{
		switch (data->data_type)
		{
		case LOSE_NOTI:
			// disable touch
			menu->setTouchEnabled(false);
			CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
			// process
			alertFinish(true);
			acFinish = CCSequence::create(CCDelayTime::create(2), CCCallFuncN::create(this, callfuncN_selector(SceneGame::goToWin)), NULL);
			this->runAction(acFinish);
			break;

		case PARTNER_DISCONNECTED_NOTI:
			alertServer("Đối phương đã thoát!");
			break;

		case FIRST_TURN:
			canShoot = true;
			lbTurn->setString("Tới lượt bạn!");
			lbTurn->setColor(ccBLUE);
			break;

		case SECOND_TURN:
			canShoot = false;
			break;

		case SHOT_DATA:
			x_ser = data->common_data.shot_data.x;
			y_ser = data->common_data.shot_data.y;
			// TODO:
//			cout << "Enemy fire: (" << x_ser << ", " << y_ser << ")" << endl;
			if (checkHit(x_ser, y_ser))
			{
				// text
				lbTurn->setString("Tới lượt đối thủ!");
				lbTurn->setColor(ccRED);
				// set flag can not shoot
				canShoot = false;
				// show explosion
				int posX = map->convertToWorld(x_ser, y_ser).x;
				int posY = map->convertToWorld(x_ser, y_ser).y;
				mine_server = Mine::create(ccp(posX+18, posY+16), x_ser, y_ser);
				mine_server->setAnchorPoint(CCPointZero);
				/*make explosion*/
				SimpleAudioEngine::sharedEngine()->playEffect("raw/explosion.mp3");
				mine_server->startAnimation("explosion_", 1, 8, 0.1f, false, false);
				this->addChild(mine_server, 2);
				SceneServer::client->sendIsHitNotificationPackage(SceneServer::client->getSock(), 1);
			}
			else
			{
				// my turn
				canShoot = true;
				// text
				lbTurn->setString("Tới lượt bạn!");
				lbTurn->setColor(ccBLUE);
				// show mine
				int posX = map->convertToWorld(x_ser, y_ser).x;
				int posY = map->convertToWorld(x_ser, y_ser).y;
				mine_server = Mine::create(ccp(posX+18, posY+16), x_ser, y_ser);
				mine_server->setAnchorPoint(CCPointZero);
				/*make explosion*/
				SimpleAudioEngine::sharedEngine()->playEffect("raw/mimo.ogg");
				mine_server->startAnimation("mine_", 1, 7, 0.1f, false, false);
				this->addChild(mine_server, 2);
				// send message
				SceneServer::client->sendIsHitNotificationPackage(SceneServer::client->getSock(), 0);
			}
			break;

		case IS_HIT_NOTI:
			switch (data->common_data.isHit)
			{
			// fire in the hole
			case 1:
				mine_server = Mine::create(ccp(shootLocation.x+16, shootLocation.y), shootPoint.x, shootPoint.y);
				mine_server->setAnchorPoint(CCPointZero);
				/*make explosion*/
				SimpleAudioEngine::sharedEngine()->playEffect("raw/explosion.mp3");
				mine_server->startAnimation("explosion_", 1, 8, 0.1f, false, false);
				this->addChild(mine_server, 2);
				mines->addObject(mine_server);
				// can shoot
				canShoot = true;
				// text
				lbTurn->setString("Tới lượt bạn!");
				lbTurn->setColor(ccBLUE);
				break;

			case 0:
				mine_server = Mine::create(ccp(shootLocation.x+16, shootLocation.y), shootPoint.x, shootPoint.y);
				mine_server->setAnchorPoint(CCPointZero);
				/*create mine*/
				SimpleAudioEngine::sharedEngine()->playEffect("raw/mimo.ogg");
				mine_server->startAnimation("mine_", 1, 7, 0.2f, false, false);
				this->addChild(mine_server, 2);
				mines->addObject(mine_server);
				// can shoot
				canShoot = false;
				// text
				lbTurn->setString("Tới lượt đối thủ!");
				lbTurn->setColor(ccRED);
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}
