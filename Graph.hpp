#pragma once

#include <vector>
#include <list>

#include "Node.hpp"
#include "Edge.hpp"
#include <SFML/Graphics/Font.hpp>

using namespace std;

const float DEFAULT_TEMP = CANVAS_HEIGHT / 8;

class Graph 
{
    enum Algorithm { None, Eades, FructhermanReingold, KamadaKawai, YifanHu };
private:
    vector<list<int>> adjList;
    vector<Node> nodes;
    vector<Edge> edges;

    // Force directed drawing params
    Algorithm algorithm;
    float width, height;
    float temp;
    float cooling;
    float L;
    float G = 5.f; // gravity
    bool done = false;
    bool running = false;
    int maxIterations = 100000;
    float treshold = 0.1f;
    float Gravity = 1.f;
    int iter = 0; // num of iterations

    // Parameters used in drawing
    int maxDegree;
    float maxWeight;
    float nodeMin = DEFAULT_RADIUS, nodeMax = DEFAULT_RADIUS;
    bool showLabels = false;
public:
    Graph() = default;
    Graph(vector<Node>& nodes, vector<Edge>& edges);
    friend std::ostream& operator<<(std::ostream& os, const Graph& obj);

    // Generic graph operations
    void add_node(Node n);
    void add_edge(Edge e);
    const vector<Node>& Nodes() const;
    const vector<Edge>& Edges() const;

    // Place nodes randomly in a rectangle area defined by pos and L
    void RandomLayout(Vector2f pos, float L);
    // Place nodes randomly on circle line defined by pos and R
    void RandomCircularLayout(Vector2f pos, float R);

    /* Force directed graph drawing operations
    * 
    * Algorithm is selected and configured by calling respective method (e.g. FructhermanReingold())
    * Then repeatedly call Update() for single step or Run() to run the whole algorithm 
    */

    // Fruchterman-Reingold algorithm
    // l - ideal spring length
    // cool - cooling rate
    void FruchtermanReingold(FruchtermanParams);
    // Run 1 iteraton, return !done
    bool Update();
    // Reset the 'done' flag
    void Reset();

    // Parse contents of GML file and create a Graph
    static Graph fromGML(string file);

    /* Drawing the graph
    *
    * Functions for configuring and modyfing how the graph will be drawn
    */
    // Draw the graph
    void draw(tgui::CanvasSFML::Ptr& target, sf::Font font);
    // Set drawing parameters
    void setNodeDimensions(float nodeMin, float nodeMax);
    // Set whether to show labels
    void setShowLabels(bool showLabels);
private:
    // Implementation of force-directed drawing algorithms, returns true if equilibrum is reached
    bool fructhermanReingoldStep();
};


