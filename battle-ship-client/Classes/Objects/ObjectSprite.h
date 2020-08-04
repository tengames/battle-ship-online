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
#ifndef OBJECTSPRITE_H_
#define OBJECTSPRITE_H_

#include "cocos2d.h"

USING_NS_CC;

class ObjectSprite : public CCSprite {
public:
	~ObjectSprite();
	ObjectSprite(CCPoint position);
	static ObjectSprite *create(CCPoint position);
	ObjectSprite(CCPoint position, CCSize size);
	static ObjectSprite *create(CCPoint position, CCSize size);
	ObjectSprite(const char *pList, CCPoint position);
	static ObjectSprite *create(const char *pList, CCPoint position);
	ObjectSprite(const char *pList, CCTexture2D *texture, CCPoint position);
	static ObjectSprite *create(const char *pList, CCTexture2D *texture, CCPoint position);

	void startAnimation(const char *aniName, int startFrame, int stopFrame, float delay, bool isLoop, bool canReverse);
	void stopAnimation(void);
	void setRegion(const char *aniName);
	CCSpriteFrame *getRegionFrame(void);
	void remove(CCSprite *father);
	bool isAnimationDone(void);

private:
	CCArray *aniFrames;
	CCAnimation *animation;
	CCAnimate *animate;
	CCActionInterval *acForever;
	CCSpriteFrame *frame;

	bool isStopAni, isLoop;
};

#endif /* OBJECTSPRITE_H_ */
