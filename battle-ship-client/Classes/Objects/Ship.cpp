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
#include "Objects/Ship.h"

Ship::Ship(CCPoint position, int type) : ObjectSprite(position)
{
	hpCoors = CCArray::create();
	hpCoors->retain();
	this->type = type;
	isRotated = false;
	setScale(0.8f);
	setcoorI(0);
	setcoorJ(0);
	setoldCoorI(0);
	setoldCoorJ(0);
	setTouch(true);
	setisSet(false);
	setDie(false);
	switch (type)
	{
	case TYPE_1:
		hp = 1;
		this->setRegion("ship1");
		this->setFlipX(true);
		break;

	case TYPE_2:
		hp = 2;
		this->setRegion("ship2_hoz");
		break;

	case TYPE_3:
		hp = 3;
		this->setRegion("ship3_hoz");
		this->setFlipX(true);
		break;

	case TYPE_4:
		hp = 4;
		this->setRegion("ship4_hoz");
		break;
	}
	this->setAnchorPoint(CCPointZero);
}

Ship *Ship::create(CCPoint position, int type)
{
	Ship *ship = new Ship(position, type);
	if (ship)
	{
		ship->autorelease();
		return ship;
	}
	CC_SAFE_DELETE(ship);
	return NULL;
}

CCArray *Ship::getHpCoors()
{
	return this->hpCoors;
}

void Ship::setHpFinish(void)
{
	// set new
	if (this->getRotated())
	{
		/*rotate ship*/
		HpCoor *hpCoor = new HpCoor(getcoorI()-1, getcoorJ()-hp+1);
		hpCoors->addObject(hpCoor);
		for (int i = getcoorJ()-hp+2; i < this->getcoorJ()+1; i++)
		{
			HpCoor *hpCoor = new HpCoor(getcoorI()-1, i);
			hpCoors->addObject(hpCoor);
		}
	}
	else
	{
		/*normal state - hoziron*/
		HpCoor *hpCoor = new HpCoor(getcoorI(), getcoorJ());
		hpCoors->addObject(hpCoor);
		for (int i = getcoorI()+1; i <= hp+this->getcoorI(); i++)
		{
			HpCoor *hpCoor = new HpCoor(i, getcoorJ());
			hpCoors->addObject(hpCoor);
		}
		hpCoors->removeLastObject(true);
	}
}

void Ship::turn(bool cw)
{
	float rotate = this->getRotation();
	if (cw)
	{
		if (rotate == 0)
		{
			rotate = -90;
		}
		else if (rotate == -90)
		{
			rotate = 0;
		}
	}
	else
	{
		rotate = 0;
	}
	this->setRotation(rotate);
	this->isRotated = !this->isRotated;
}

bool Ship::getRotated(void)
{
	return this->isRotated;
}

CCSize Ship::getSize(void)
{
	return (this->getRegionFrame()->getOriginalSize());
}

short Ship::getType(void)
{
	return this->type;
}
