#ifndef Util_h
#define Util_h

#include <stdio.h>
#include <exception>
#include <string>
#include <unordered_set>
#include <iostream>

namespace Util {
    
    enum NodeType : short { HEAD = 0, BODY = 1, FOOD = 2, UNINIT = 3 };
    
    struct Node {
        int x;
        int y;
        NodeType type;
        Node() : x(0), y(0), type(NodeType::UNINIT) {};
        Node(std::size_t x, std::size_t y, NodeType type) : x(x), y(y), type(type) {}
    };
    
    inline bool operator==(const Node &lhs, const Node &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    
    struct NodePtrHash {
        std::size_t operator()(const Node* node) const {
            return nullptr == node ? 0 : node->x + node->y;
        }
    };
    struct NodePtrEqual {
        bool operator()(const Node *lhs, const Node *rhs) const {
            return (nullptr == lhs && nullptr == rhs)
            || (nullptr != lhs && nullptr != rhs && *lhs == *rhs);
        }
    };
    using Hashset = std::unordered_set<Node*, NodePtrHash, NodePtrEqual>;
    
    enum Direction : short { LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3, UNDEFINED = 4 };
    
    // unordered_set comparator
    class CompareNodes {
        bool operator()(Node *lhs, Node *rhs) {
            return lhs->x == rhs->x && lhs->y == rhs->y;
        }
    };
    
    class Exception : public std::exception {
    public:
        Exception(const std::string & msg) : msg(msg) {}
        Exception(std::string      && msg) : msg(std::move(msg)) {}
        std::string msg;
    };
    
    inline std::ostream& operator<<(std::ostream& out, const Node& node)
    {
        out << "(" << node.x << ", " << node.y << ", ";
        switch (node.type) {
            case FOOD:
                out << "Food)";
                break;
            case HEAD:
                out << "Head)";
                break;
            case BODY:
                out << "Body)";
                break;
            default:
                out << "UnknownType)";
                break;
        }
        return out;
    }
}

#endif /* Util_h */