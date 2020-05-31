
#ifndef node_h
#define node_h

#include <Core/Transform.h>

class Node {
public:
    Node();
    ~Node() = default;
    
public:
    // Parent node
    Node* parent;
    
    // Child nodes
    std::vector<Node*> children;
    
    // Name
    std::string name;
    
    // Transformation
    matrix4 transform;
    
    // FInal transform
    matrix4 finalTransform;
};

#endif
