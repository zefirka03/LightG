#include "quadtree.h"

QuadNode::QuadNode() {}

QuadNode::QuadNode(int _deep, int _pool_position) : 
    deep(_deep), pool_position(_pool_position) {}

void QuadNode::reset() {
    bounds = boundingBox();
    is_devided = false;
    childs_size = 0;
    child_first = -1;
    child_last = -1;
    deep = 0;
    pool_position = 0;
}