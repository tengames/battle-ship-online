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
#include "ObjectSprite.h"

ObjectSprite::~ObjectSprite()
{
	this->removeAllChildrenWithCleanup(true);
}

ObjectSprite::ObjectSprite(CCPoint position)
{
	this->isStopAni = false;
	this->isLoop = false;
	this->setPosition(position);
}

ObjectSprite *ObjectSprite::create(CCPoint position)
{
	ObjectSprite *objectSprite = new ObjectSprite(position);
	if (objectSprite)
	{
		objectSprite->autorelease();
		return objectSprite;
	}
	CC_SAFE_DELETE(objectSprite);
	return NULL;
}

ObjectSprite::ObjectSprite(CCPoint position, CCSize size)
{
	this->isStopAni = false;
	this->isLoop = false;
	this->setPosition(position);
	this->setContentSize(size);
}

ObjectSprite *ObjectSprite::create(CCPoint position, CCSize size)
{
	ObjectSprite *objectSprite = new ObjectSprite(position, size);
	if (objectSprite)
	{
		objectSprite->autorelease();
		return objectSprite;
	}
	CC_SAFE_DELETE(objectSprite);
	return NULL;
}

ObjectSprite::ObjectSprite(const char *pList, CCPoint position)
{
	char str1[100] = {0};
	char str2[100] = {0};

	this->isStopAni = false;
	this->isLoop = false;
	this->setPosition(position);
	sprintf(str1, "%s.plist", pList);
	sprintf(str2, "%s.png", pList);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(str1, str2);

}

ObjectSprite *ObjectSprite::create(const char *pList, CCPoint position)
{
	ObjectSprite *objectSprite = new ObjectSprite(pList, position);
	if (objectSprite)
	{
		objectSprite->autorelease();
		return objectSprite;
	}
	CC_SAFE_DELETE(objectSprite);
	return NULL;
}

ObjectSprite::ObjectSprite(const char *pList, CCTexture2D *texture, CCPoint position)
{
	char str[100] = {0};

	this->isStopAni = false;
	this->isLoop = false;
	this->setPosition(position);
	sprintf(str, "%s.plist", pList);
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(str, texture);
}

ObjectSprite *ObjectSprite::create(const char *pList, CCTexture2D *texture, CCPoint position)
{
	ObjectSprite *objectSprite = new ObjectSprite(pList, texture, position);
	if (objectSprite)
	{
		objectSprite->autorelease();
		return objectSprite;
	}
	CC_SAFE_DELETE(objectSprite);
	return NULL;
}

void ObjectSprite::setRegion(const char *aniName)
{
	char str[100] = {0};
	sprintf(str, "%s.png", aniName);
	frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str);
	this->initWithSpriteFrame(frame);
}

CCSpriteFrame *ObjectSprite::getRegionFrame(void)
{
	if (frame) return this->frame;
	return NULL;
}

void ObjectSprite::startAnimation(const char *aniName, int startFrame, int stopFrame, float delay, bool isLoop, bool canReverse)
{
	this->isLoop = isLoop;
	this->isStopAni = false;
	int numberOfFrames = stopFrame - startFrame + 1;
	aniFrames = CCArray::createWithCapacity(numberOfFrames);
	aniFrames->retain();

	char str[100] = {0};
	for (int i = startFrame; i <= stopFrame; i++)
	{
		sprintf(str, "%s%d.png", aniName, i);
		CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str);
		aniFrames->addObject(frame);
	}

	this->initWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(str));
	animation = CCAnimation::createWithSpriteFrames(aniFrames, delay);
	animation->retain();
	animate = CCAnimate::create(animation);
	animate->retain();

	if (isLoop)
	{
		acForever = CCRepeatForever::create(animate);
		this->runAction(acForever);
	}
	else
	{
		this->runAction(animate);
	}
}

void ObjectSprite::stopAnimation(void)
{
	if (isLoop) this->stopAction(acForever);
}

bool ObjectSprite::isAnimationDone(void)
{
	if (animate) return animate->isDone();
	return false;
}

void ObjectSprite::remove(CCSprite *father)
{
	father->removeFromParentAndCleanup(false);
}
