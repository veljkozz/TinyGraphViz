#pragma once
#include <iostream>
#include <SFML/System/Vector2.hpp>

using namespace std;

struct Edge {
	Vector2i nodes;
    float weight;

	Edge(int node1, int node2, float weight = 1) : nodes({ node1, node2 }), weight(weight) {};
	Edge(Vector2i nodes, float weight = 1) : nodes(nodes), weight(weight) {};

    // Overloading [] operator
    int operator[](size_t index) const {
        if (index == 0) 
            return nodes.x;
        else if (index == 1) 
            return nodes.y;
        return -1;
    }

    friend std::ostream& operator<<(std::ostream& os, const Edge& obj) {
        os << "(" << obj[0] << ',' << obj[1] << ')';
        return os;
    }
};