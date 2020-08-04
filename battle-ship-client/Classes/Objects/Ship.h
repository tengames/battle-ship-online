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
#ifndef SHIP_H_
#define SHIP_H_

#include "ObjectSprite.h"
#include "HpCoor.h"

class Ship : public ObjectSprite {
public:
	static const char TYPE_1 = 1;
	static const char TYPE_2 = 2;
	static const char TYPE_3 = 3;
	static const char TYPE_4 = 4;

	Ship(CCPoint position, int type);
	static Ship *create(CCPoint position, int type);
	CCSize getSize(void);
	CCArray *getHpCoors();
	short getType(void);
	void turn(bool cw);
	bool getRotated(void);
	void setHpFinish(void);

	CC_SYNTHESIZE(bool, isTouch, Touch);
	CC_SYNTHESIZE(bool, isSet, isSet);
	CC_SYNTHESIZE(bool, isDie, Die);
	CC_SYNTHESIZE(short, coorI, coorI);
	CC_SYNTHESIZE(short, coorJ, coorJ);
	CC_SYNTHESIZE(short, oldCoorI, oldCoorI);
	CC_SYNTHESIZE(short, oldCoorJ, oldCoorJ);

private:
	CCArray *hpCoors;
	short type, hp;
	bool isRotated;
};

#endif /* SHIP_H_ */
