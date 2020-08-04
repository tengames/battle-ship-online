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
#ifndef SCENELOADING_H_
#define SCENELOADING_H_

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "SceneMenu.h"

using namespace std;
using namespace CocosDenshion;
USING_NS_CC;

class SceneLoading : public CCLayer
{
public:
	virtual bool init(void);
	virtual void onExit(void);
	static CCScene *scene(void);
	CREATE_FUNC(SceneLoading);
	static void removeAssets(void);

private:
	static void removeTextures(char *link, char *format, char *files, ...);
	void initParams(void);
	void loadSound(void);
	int countAssets(string link);
	void addAssets(void);
	void addPlists(char *link, char *pList, ...);
    void addTextures(char *link, char *format, char *files, ...);
    void loadingCallBack(CCObject *obj);
    void loadDone(void);
	void createBackground(void);

    int numberOfAssets;
    int numberOfLoadedAssets;
};

#endif /* SCENELOADING_H_ */
