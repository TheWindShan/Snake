//
//  GameScene.cpp
//  Snake
//
//  Created by 星空下的传说 on 16/4/23.
//
//

#include "GameScene.h"
#include "Model.h"
#include "Util.h"
#include "HelloWorldScene.h"

USING_NS_CC;

namespace {
    constexpr int SPRITE_LAYER = 0;
    // get texture file name based on the node type
    std::string getTextureName(const Util::Node &node);
    
    // based on the position, calculate the hash value
    // 1000 * x + y, assumption, y < 1000
    inline std::size_t calcHash(int x, int y);
}

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    height = visibleSize.height / RATIO;
    width = visibleSize.width / RATIO;
    modelPtr = new Snake::Model(width, height);
    //std::move(new Snake::Model(width, height)));
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Snake::Model::NodesVector nodesToAdd = modelPtr->getNodesAdded();
    
    for (auto node : nodesToAdd) {
        createAndPutSprite(node);
    }
    
    speed = 120;
    schedule(schedule_selector(GameScene::update), 60/speed);
    
    createKeyboardListener();
    createTouchListener();
    
    return true;
}

void GameScene::update(float dt){
    Snake::Model::GameStatus ret = modelPtr->update(userCommand);
    if (Snake::Model::GameStatus::NORMAL != ret) {
        gameOver();
    }
    Snake::Model::NodesVector nodesToRemove = modelPtr->getNodesRemoved();
    for (auto& node : nodesToRemove) {
        removeSprite(node);
    }
    Snake::Model::NodesVector nodesToChange = modelPtr->getNodesChanged();
    for (auto& node : nodesToChange) {
        updateSprite(node);
    }
    Snake::Model::NodesVector nodesToAdd = modelPtr->getNodesAdded();
    for (auto& node : nodesToAdd) {
        createAndPutSprite(node);
    }
}


int GameScene::createAndPutSprite(const Util::Node& node) {
    auto sprite = Sprite::create(getTextureName(node));
    sprite->setPosition(Vec2(node.x * RATIO, node.y * RATIO));
    sprite->setAnchorPoint(Vec2(0, 0));
    this->addChild(sprite, SPRITE_LAYER, static_cast<int>(calcHash(node.x, node.y)));
    return 0;
}

int GameScene::updateSprite(const Util::Node& node) {
    auto child = this->getChildByTag(static_cast<int>(calcHash(node.x, node.y)));
    auto sprite = static_cast<Sprite*>(child);
    sprite->setTexture(getTextureName(node));
    return 0;
}

int GameScene::removeSprite(const Util::Node& node) {
    this->removeChildByTag(static_cast<int>(calcHash(node.x, node.y)));
    return 0;
}

void GameScene::gameOver() {
    unschedule(schedule_selector(GameScene::update));
    
    _eventDispatcher->removeEventListenersForType(EventListener::Type::TOUCH_ONE_BY_ONE);
    _eventDispatcher->removeEventListenersForType(EventListener::Type::KEYBOARD);
    
    auto label = Label::create("GAME OVER", "Arial", 50);
    Size visibleSize = Director::getInstance()->getVisibleSize();
    label->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
    this->addChild(label, 1);
    label->setScale(0);
    label->runAction(ScaleTo::create(3, 1.5));
    label->setOpacity(0);
    label->runAction(FadeTo::create(1.5, 255));
    this->runAction(Sequence::create(DelayTime::create(5),CallFunc::create([]{Director::getInstance()->replaceScene(HelloWorld::createScene());}), NULL));
    
}

namespace {
    std::string getTextureName(const Util::Node &node) {
        switch (node.type) {
            case Util::NodeType::FOOD:
                return std::string("Food.png");
            case Util::NodeType::BODY:
                return std::string("Snake.png");
            case Util::NodeType::HEAD:
                return std::string("SnakeHead.png");
            default:
                // should never go here
                break;
        }
        return std::string("Error.png");
    }
    std::size_t calcHash(int x, int y) {
        return 1000 * x + y;
    }
}

/**************************** Keyboard Listener *************************************/
 
void GameScene::createKeyboardListener() {
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            userCommand = Util::Direction::LEFT;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            userCommand = Util::Direction::RIGHT;
            break;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            userCommand = Util::Direction::UP;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            userCommand = Util::Direction::DOWN;
            break;
        default:
            break;
    }
}

/****************************** Touch Listener *************************************/

void GameScene::createTouchListener() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
}

bool GameScene::onTouchBegan(Touch* touch, Event* event) {
    touchBeginPoint = touch->getLocation();
    return true;
}

void GameScene::onTouchEnded( Touch* touch, Event* event ) {
    touchEndPoint = touch->getLocation();
    parseTouchCommand();
}

void GameScene::parseTouchCommand() {
    auto startX = static_cast<int>(touchBeginPoint.x);
    auto startY = static_cast<int>(touchBeginPoint.y);
    auto endX   = static_cast<int>(touchEndPoint.x);
    auto endY   = static_cast<int>(touchEndPoint.y);
    
    if (abs(endY - startY) > abs(endX - startX)) {
        userCommand = endY > startY ? Util::Direction::UP : Util::Direction::DOWN;
    } else {
        userCommand = endX > startX ? Util::Direction::RIGHT : Util::Direction::LEFT;
    }
}
