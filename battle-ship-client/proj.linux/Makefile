EXECUTABLE = battleship

SOURCES = main.cpp \
	../Classes/AppDelegate.cpp \
	../Classes/Scenes/SceneLoading.cpp \
	../Classes/Scenes/SceneMenu.cpp \
	../Classes/Scenes/SceneGame.cpp \
	../Classes/Scenes/SceneServer.cpp \
	../Classes/Scenes/SceneArrangement.cpp \
	../Classes/Scenes/SceneFinal.cpp \
	../Classes/Scenes/SceneAbout.cpp \
	../Classes/Client/Client.cpp \
	../Classes/Objects/ObjectSprite.cpp \
	../Classes/Objects/Mine.cpp \
	../Classes/Objects/Ship.cpp \
	../Classes/Objects/Map.cpp \
	../Classes/Objects/HpCoor.cpp \

SHAREDLIBS += -lcocos2d -lcocosdenshion
COCOS_LIBS = $(LIB_DIR)/libcocos2d.so $(LIB_DIR)/libcocosdenshion.so

####### Direct link
COCOS_ROOT = /home/woodyx/android/cocos2d-x-2.1.4/cocos2d-x-2.1.4
include $(COCOS_ROOT)/cocos2dx/proj.linux/cocos2dx.mk

INCLUDES +=	 -I../ \
	-I../../ \
	-I../Classes \
	-I$(COCOS_ROOT)/CocosDenshion/include \
	-I$(COCOS_ROOT)/extensions/ \
	-I$(COCOS_ROOT)/external/ \
	-I$(COCOS_ROOT)/external/chipmunk/include/chipmunk

####### Use extension libs	
STATICLIBS += \
	$(STATICLIBS_DIR)/libcurl.a \
	$(LIB_DIR)/libextension.a \
	#$(LIB_DIR)/libbox2d.a \
	#$(LIB_DIR)/libchipmunk.a

####### Build rules
$(TARGET): $(OBJECTS) $(STATICLIBS) $(COCOS_LIBS) $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_LINK)$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(SHAREDLIBS) $(STATICLIBS) $(LIBS)

####### Compile
$(OBJ_DIR)/%.o: ../%.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/%.o: %.c $(CORE_MAKEFILE_LIST)
	@mkdir -p $(@D)
	$(LOG_CC)$(CC) $(CCFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

