LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

LOCAL_SRC_FILES := hellocpp/main.cpp \
			        ../../Classes/AppDelegate.cpp \
			        ../../Classes/Scenes/SceneLoading.cpp \
			        ../../Classes/Scenes/SceneMenu.cpp \
			        ../../Classes/Scenes/SceneGame.cpp \
			        ../../Classes/Scenes/SceneServer.cpp \
			        ../../Classes/Scenes/SceneArrangement.cpp \
			        ../../Classes/Scenes/SceneFinal.cpp \
			        ../../Classes/Scenes/SceneAbout.cpp \
			        ../../Classes/Client/Client.cpp \
			        ../../Classes/Objects/ObjectSprite.cpp \
			        ../../Classes/Objects/Mine.cpp \
			        ../../Classes/Objects/Ship.cpp \
			        ../../Classes/Objects/Map.cpp \
			        ../../Classes/Objects/HpCoor.cpp \

                   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes                   

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)
