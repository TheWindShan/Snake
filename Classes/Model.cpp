#include "Model.h"
#include <iostream>
#include <random>

namespace Snake {
    Model::GameStatus Model::update(Util::Direction command) {
        
        resetNodesChanged();
        resetNodesAdded();
        resetNodesRemoved();
        
        updateDirection(command);
        
        changeSnakeHeadToBody();
        
        Util::Node *newHead = generateSnakeHead();
        if (!isInBoard(newHead)) {
            return GameStatus::LOSE;
        }
        
        bool valid = false;
        if (*newHead == food) {
            // size of snake increases, no need to remove the snake tail
            // push the head into snake,
            // then check if the snake is all over the board
            valid = pushNodePtr(newHead);
            if (boardSize == snake.size()) {
                return GameStatus::WIN;
            }
            ++foodEaten;
            updateFood();
        }
        else {
            // move the tail before moving thehead
            // otherwise a bug occurs: when head, tail are adjacent,
            // and the next step the head will move to the tail position,
            // it will cause a problem, since that place is still held by
            // the tail.
            // Move Tail First!!
            moveSnakeTail();
            valid = pushNodePtr(newHead);
        }
        
        return valid ? GameStatus::NORMAL : GameStatus::LOSE;
    }
    
    void Model::updateDirection(Util::Direction command) {
        // if command == undefined, use the privious direction
        if (Util::Direction::UNDEFINED == command) {
            return;
        }
        if (static_cast<short>(command) % 2 == static_cast<short>(direction) % 2) {
            // UP <-> DOWN, LEFT <-> RIGHT
            // junk info, discard
            return;
        }
        direction = command;
    }
    
    Util::Node* Model::generateSnakeHead() {
        // origin head has already been one part of the body
        Util::Node *oriHead = getSnakeHead();
        // make a new head
        Util::Node *newHead = new Util::Node(0, 0, Util::NodeType::HEAD);
        
        switch (direction) {
            case Util::LEFT:
                newHead->x = oriHead->x - 1;
                newHead->y = oriHead->y;
                break;
            case Util::RIGHT:
                newHead->x = oriHead->x + 1;
                newHead->y = oriHead->y;
                break;
            case Util::UP:
                newHead->x = oriHead->x;
                newHead->y = oriHead->y + 1;
                break;
            case Util::DOWN:
                newHead->x = oriHead->x;
                newHead->y = oriHead->y - 1;
                break;
            case Util::UNDEFINED:
                // should never go into here :)
            default:
                delete newHead;
                throw Util::Exception("Command not readable.");
                break;
        }
        
        return newHead;
    }
    
    void Model::changeSnakeHeadToBody() noexcept {
        Util::Node *node = getSnakeHead();
        node->type = Util::NodeType::BODY;
        nodesChanged.push_back(node);
    }
    
    void Model::moveSnakeTail() {
        // snake tail is head of the queue
        popNodePtr();
    }
    
    Util::Node* Model::getSnakeHead() {
        return snake.back();
    }
    
    void Model::updateFood() {
        while (!putFood()) {}
        nodesAdded.push_back(&food);
    }
    
    bool Model::putFood() {
        static std::default_random_engine generator;
        std::uniform_int_distribution<std::size_t> distributionH(0, height - 1);
        std::uniform_int_distribution<std::size_t> distributionW(0, width - 1);
        food.x = distributionW(generator);
        food.y = distributionH(generator);
        std::cerr << "Put food at (" << food.x << ", " << food.y << ")";
        std::cerr << " valid ? " << (occupied.end() == occupied.find(&food)) <<
        std::endl;
        return occupied.end() == occupied.find(&food);
    }
    
    Model::NodesVector Model::getNodesAdded() {
        Model::NodesVector copyOfNodes;
        copyOfNodes.reserve(nodesAdded.size());
        for (Util::Node *node : nodesAdded) {
            copyOfNodes.push_back(*node);
        }
        return copyOfNodes;
    }
    Model::NodesVector Model::getNodesRemoved() {
        Model::NodesVector copyOfNodes;
        copyOfNodes.reserve(nodesRemoved.size());
        for (Util::Node *node : nodesRemoved) {
            copyOfNodes.push_back(*node);
        }
        return copyOfNodes;
    }
    
    Model::NodesVector Model::getNodesChanged() {
        NodesVector copy;
        for (Util::Node *node : nodesChanged) {
            copy.push_back(*node);
        }
        return copy;
    }
    
    bool Model::pushNodePtr(Util::Node *node) {
        auto ret = occupied.insert(node);
        if (ret.second) {
            // success
            snake.push(node);
            if (*node == food) {
                // if the position of the node(usually head)
                // is the same as the position of food
                // change the food node
                nodesChanged.push_back(node);
            } else {
                // else adding a new node
                nodesAdded.push_back(node);
            }
        }
        else {
            auto collision = *(ret.first);
            collision->type = Util::NodeType::HEAD;
            nodesChanged.push_back(collision);
            // fail, destroy the node
            delete node;
        }
        return ret.second;
    }
    
    void Model::popNodePtr() noexcept {
        Util::Node *node = snake.front();
        snake.pop();
        occupied.erase(node);
        nodesRemoved.push_back(node);
    }
    
    void Model::resetNodesAdded() noexcept {
        nodesAdded.clear();
    }
    
    void Model::resetNodesRemoved() noexcept {
        for (Util::Node *node : nodesRemoved) {
            if (*node == food) {
                continue;
            }
            delete node;
        }
        nodesRemoved.clear();
    }
    
    void Model::resetNodesChanged() noexcept {
        nodesChanged.clear();
    }
    
    bool Model::isInBoard(Util::Node * node) noexcept {
        return 0 <= node->x && width > node->x && 0 <= node->y && height > node->y;
    }
    
} // end namespace Snake