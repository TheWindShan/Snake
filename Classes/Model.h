#ifndef Engine_h
#define Engine_h

#include <stdio.h>
#include <queue>
#include <vector>
#include "Util.h"
#include <iostream>

namespace Snake {
    
    /* Model of the game */
    class Model {
    public:
        
        enum GameStatus { NORMAL = 0, WIN = 1, LOSE = 2, UNDEFINED = 3 };
        using NodesVector = std::vector<Util::Node>;
        
        /**********************************************************************
         * Constructor, arguments for size of the game area                   *
         * Note: size should be greater than 2*2, otherwise throws exceptions *
         **********************************************************************/
        Model(std::size_t width = 2, std::size_t height = 2);
        
        ~Model();
        
        /* takes one argument of the user command,
         * should be one of UP, DOWN, LEFT, RIGHT, UNDEFINED
         * if command = UNDEFINED, then the snake would go as
         * last direction return GameStatus */
        GameStatus update(Util::Direction command);
        
        // getter of direction
        inline Util::Direction getDirection() const { return direction; }
        // getter of the foodEaten
        inline std::size_t getFoodEaten() const { return foodEaten; }
        
        // after calling update(), should call these
        // two functionsto get lists of nodes changed
        // Note, the nodes returned are copies of the original one
        NodesVector getNodesAdded();
        NodesVector getNodesRemoved();
        NodesVector getNodesChanged();
        
        // disable
        Model(const Model&) = delete;
        Model operator=(const Model&) = delete;
        Model(Model&&) = delete;
        Model operator=(Model&&) = delete;
    private:
        using NodePtrsVector = std::vector<Util::Node*>;
        // board size
        std::size_t height;
        std::size_t width;
        std::size_t boardSize;
        // #food eaten
        std::size_t foodEaten;
        /* snake body
         * head of the queue is the tail of the snake
         * tail of the queue is the head of the snake */
        std::queue<Util::Node*> snake;
        // food position
        Util::Node food;
        // hashset, std::unordered_set<Node*, Hash, Equal>
        Util::Hashset occupied;
        // direction the snake heading to
        Util::Direction direction;
        // nodes changed
        NodePtrsVector nodesAdded;
        NodePtrsVector nodesRemoved;
        NodePtrsVector nodesChanged;
        
        /************************ Utility Functions *********************/
        
        // based on the command received from controller
        // change the direction of the snake
        // keep going if command = undefined
        void updateDirection(Util::Direction command);
        // check the capacity before calling this function
        // i.e check if there is enough room to put a food
        void updateFood();
        // generate a new head based on the direction
        // also mark the old head as a part of body
        // either return a node* which may not be "valid"
        // or throw an exception with no memory allocated
        Util::Node* generateSnakeHead();
        void changeSnakeHeadToBody() noexcept;
        void moveSnakeTail();
        // put food at arbitary place,
        // if success return true, otherwise false
        bool putFood();
        
        Util::Node* getSnakeHead();
        // put a node* into hashset and the tail of the queue
        // if success then put it into nodesAdded vector.
        // return false if that place(x,y) has already been occupied
        bool pushNodePtr(Util::Node *node);
        // pop the head of queue (the tail of the snake)
        // remove it from the hashset
        // put it in the nodesRemoved
        void popNodePtr() noexcept;
        // reset the nodesAdded vector, should be called before each frame
        void resetNodesAdded() noexcept;
        // reset the nodesRemoved, will delete all the nodes in that vector
        // call before each frame
        void resetNodesRemoved() noexcept;
        void resetNodesChanged() noexcept;
        
        bool isInBoard(Util::Node *node) noexcept;
    };
    
    inline Model::Model(std::size_t width, std::size_t height)
    : height(height), width(width), boardSize(height * width), foodEaten(0) {
        // check the board size, should be greater than 2*2
        if (height < 2 || width < 2) {
            throw Util::Exception("Board is too small, 2*2 at least");
        }
        
        // usually, only two nodes would be added each frame
        nodesAdded.reserve(3);
        
        // put snake
        Util::Node *head = new Util::Node(width / 2, height / 2, Util::NodeType::HEAD);
        Util::Node *body = new Util::Node(width / 2 - 1, height / 2, Util::NodeType::BODY);
        
        std::cerr << head->x << "," << head->y << std::endl;
        std::cerr << body->x << "," << body->y << std::endl;
        // no need to check return code because the nodes are calculated, cannot be dup
        pushNodePtr(body);
        pushNodePtr(head);
        // put food
        food.type = Util::NodeType::FOOD;
        updateFood();
        
        direction = Util::Direction::RIGHT;
    }
    
    inline Model::~Model() {
        while (!snake.empty()) {
            popNodePtr();
        }
        // nodes poped up to nodeRemoved vector, delete from there
        resetNodesRemoved();
    }
    
} // end namespace Snake

#endif /* Engine_hpp */