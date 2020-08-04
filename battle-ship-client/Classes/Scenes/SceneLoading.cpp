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
#include "SceneLoading.h"

void SceneLoading::onExit(void)
{
		this->removeAllChildrenWithCleanup(true);
}

CCScene* SceneLoading::scene(void)
{
	CCScene *scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(!scene);

		SceneLoading *layer = SceneLoading::create();
		CC_BREAK_IF(!layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

bool SceneLoading::init(void)
{
	if (!CCLayer::init())
	{
		return false;
	}
	// init params
	createBackground();
	initParams();
	loadSound();
	addAssets();

	return true;
}

void SceneLoading::initParams(void)
{
	// initialize
	numberOfAssets = 31;
	numberOfLoadedAssets = 0;
}

void SceneLoading::loadSound(void)
{
    // preload background music and effect
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("raw/xmusic.mp3");
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("raw/menumusic.mp3");
    SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("raw/ocean.mp3");

    SimpleAudioEngine::sharedEngine()->preloadEffect("raw/buttonclicked.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("raw/explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("raw/loosesound.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("raw/winsound.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("raw/mimo.ogg");
}

void SceneLoading::createBackground(void)
{
	// create background
	CCSprite *spBgLoading = CCSprite::create("drawable/background.jpg");
	spBgLoading->setPosition(ccp(400, 240));
	this->addChild(spBgLoading, 0);
}

void SceneLoading::addAssets(void)
{
	addTextures((char*) "drawable/", (char*) ".png",
			(char*) "back_press", (char*) "back", (char*) "background_dialog",
			(char*) "background_game_over", (char*) "background_menu",
			(char*) "background_paper", (char*) "background_winner",
			(char*) "cancel_press", (char*) "cancel", (char*) "exit_press",
			(char*) "exit", (char*) "grid", (char*) "gui",
			(char*) "next_press", (char*) "next", (char*) "no_press", (char*) "no",
			(char*) "play_press", (char*) "play", (char*) "setting_press", (char*) "setting",
			(char*) "sound_off", (char*) "sound_on", (char*) "start_press", (char*) "start",
			(char*) "yes_press", (char*) "yes", (char*) "turn_press", (char*) "turn", (char*) "reset", (char*) "reset_press", NULL);
}

void SceneLoading::removeAssets(void)
{
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/xmusic.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/menumusic.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/ocean.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/buttonclicked.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/explosion.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/loosesound.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/winsound.mp3");
    SimpleAudioEngine::sharedEngine()->unloadEffect("raw/mimo.ogg");

	removeTextures((char*) "drawable/", (char*) ".jpg", (char*) "background", NULL);
	removeTextures((char*) "drawable/", (char*) ".png",
			(char*) "back_press", (char*) "back", (char*) "background_dialog",
			(char*) "background_game_over", (char*) "background_menu",
			(char*) "background_paper", (char*) "background_winner",
			(char*) "cancel_press", (char*) "cancel", (char*) "exit_press",
			(char*) "exit", (char*) "grid", (char*) "gui",
			(char*) "next_press", (char*) "next", (char*) "no_press", (char*) "no",
			(char*) "play_press", (char*) "play", (char*) "setting_press", (char*) "setting",
			(char*) "sound_off", (char*) "sound_on", (char*) "start_press", (char*) "start",
			(char*) "yes_press", (char*) "yes", (char*) "turn_press", (char*) "turn", (char*) "reset", (char*) "reset_press", NULL);
}

void SceneLoading::addTextures(char *link, char *format, char *files, ...)
{
	va_list arguments;
	const char *links, *formats, *file;
	char *result;
	int size_needed;

	links = link;
	formats = format;
	file = files;

	va_start(arguments, files);

	do
	{
		size_needed = snprintf(NULL, 0, "%s%s%s", links, file, formats);
		result = (char*)malloc(size_needed + 1);
		snprintf(result, size_needed + 1, "%s%s%s", links, file, formats);

		// loading texture
		CCTextureCache::sharedTextureCache()->addImageAsync(result, this, callfuncO_selector(SceneLoading::loadingCallBack));

		// next file
		file = va_arg(arguments, char*);

	} while(file != NULL);

	va_end(arguments);
}

void SceneLoading::removeTextures(char *link, char *format, char *files, ...)
{
	va_list arguments;
	const char *links, *formats, *file;
	char *result;
	int size_needed;

	links = link;
	formats = format;
	file = files;

	va_start(arguments, files);

	do
	{
		size_needed = snprintf(NULL, 0, "%s%s%s", links, file, formats);
		result = (char*)malloc(size_needed + 1);
		snprintf(result, size_needed + 1, "%s%s%s", links, file, formats);

		// loading texture
		if (CCTextureCache::sharedTextureCache()->textureForKey(result) != NULL) CCTextureCache::sharedTextureCache()->removeTextureForKey(result);
		else cout << result << " not found!" << endl;
		// next file
		file = va_arg(arguments, char*);

	} while(file != NULL);

	va_end(arguments);
}

void SceneLoading::addPlists(char *link, char *pList, ...)
{
	va_list arguments;
	const char *file, *links;
	char *plist_file, *texture_file;
	int size_needed;

	links = link;
	file = pList;

	va_start(arguments, pList);

	do
	{
		size_needed = snprintf(NULL, 0, "%s%s%s", links, file, ".plist");
		plist_file = (char*)malloc(size_needed + 1);
		snprintf(plist_file, size_needed + 1, "%s%s%s", links, file, ".plist");

		size_needed = snprintf(NULL, 0, "%s%s%s", links, file, ".png");
		texture_file = (char*)malloc(size_needed + 1);
		snprintf(texture_file, size_needed + 1, "%s%s%s", links, file, ".png");

		// add plist file
		CCTexture2D *texture = CCTextureCache::sharedTextureCache()->textureForKey(texture_file);
		if (texture)
		{
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plist_file, texture_file);
		}
		else cout << texture_file << " not found!" << endl;

		// next file
		file = va_arg(arguments, char*);

	} while(file != NULL);

	va_end(arguments);
}

void SceneLoading::loadingCallBack(cocos2d::CCObject *obj)
{
	// increase loaded assets
	++numberOfLoadedAssets;

	// for assign variables
	if (numberOfLoadedAssets == numberOfAssets)
	{
		loadDone();
	}
}

void SceneLoading::loadDone(void)
{
	// add plist
	addPlists((char*) "drawable/", (char*) "gui", NULL);
	// change scene
	CCDirector::sharedDirector()->replaceScene(CCTransitionPageTurn::create(1, SceneMenu::scene(), true));
}
