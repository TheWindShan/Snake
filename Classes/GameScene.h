//
//  GameScene.h
//  Snake
//
//  Created by 星空下的传说 on 16/4/23.
//
//

#ifndef GameScene_h
#define GameScene_h

#include "cocos2d.h"
#include "Util.h"


namespace Snake {
    class Model;
}
/*
namespace Util {
    class Node;
}
*/
class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    void menuBackToMain(cocos2d::Ref* pSender);
    
    CREATE_FUNC(GameScene);
    
private:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    static constexpr std::size_t RATIO = 10; // real_size / model_size
#else
    static constexpr std::size_t RATIO = 20; // real_size / model_size
#endif
    std::size_t height;
    std::size_t width;
    float speed;
    Util::Direction userCommand;
    
    cocos2d::Point touchBeginPoint;
    cocos2d::Point touchEndPoint;
    
    cocos2d::Label* gameover;
    cocos2d::Size visiblesize;
    cocos2d::Menu* menu;
    
    Snake::Model* modelPtr;
    
    // main function
    void update(float dt);
    void gameOver();
    
    // keyboard controller
    void createKeyboardListener();
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    
    // touch controller
    void createTouchListener();
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void parseTouchCommand();
    
    int createAndPutSprite(const Util::Node& node);
    int removeSprite(const Util::Node& node);
    int updateSprite(const Util::Node& node);
};

#endif /* GameScene_h */
