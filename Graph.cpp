#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include <fstream>
#include <regex>
#include <tuple>
#include <chrono>
#include <thread>

#include "Graph.hpp"
#include "Util.hpp"
#include <SFML/Graphics/Text.hpp>
#include "Line.hpp"

# define PI 3.14159265358979323846

struct lesser_first
{
	inline bool operator() (const Edge& e1, const Edge& e2)
	{
		return (e1[0] < e2[0]);
	}
};

Graph::Graph(std::vector<Node>& nodes, std::vector<Edge>& edges) : nodes(nodes), edges(edges), algorithm(None)
{
	adjList = vector<list<int>>(nodes.size());
	for (int i = 0; i < nodes.size(); ++i) {
		this->nodes[i].id = i;
	}

	for (Edge& e : this->edges) {
		if (e[1] < e[0]){
			int tmp = e.nodes.x;
			e.nodes.x = e.nodes.y;
			e.nodes.y = tmp;
		}
	}
	// Sort edges 
	sort(this->edges.begin(), this->edges.end(), lesser_first());

	for (const Edge& e : this->edges) {
		if (e[0] >= nodes.size() || e[1] >= nodes.size()) {
			cout << "Invalid edge: " << e << endl;
			exit(0xDEAD);
		}
		adjList[e[0]].push_back(e[1]);
		adjList[e[1]].push_back(e[0]);
	}

	// Maximum node degree in the graph
	maxDegree = 0;
	for (int i = 0; i < adjList.size(); i++)
		maxDegree = max(maxDegree, (int)adjList[i].size());

	// Maximum edge weight in the graph
	maxWeight = 0.f;
	for (auto e: this->edges)
		maxWeight = max(maxWeight, (float)e.weight);
}

void Graph::FruchtermanReingold(FruchtermanParams params) {
	this->L = params.L;
	this->cooling = params.cooling;
	this->temp = DEFAULT_TEMP;
	this->algorithm = Algorithm::FructhermanReingold;
	this->width = params.W;
	this->height = params.H;
	this->done = false;
}

bool Graph::Update()
{
	if (!done){
		switch (algorithm) {
		case Algorithm::FructhermanReingold:
			iter++;
			done = fructhermanReingoldStep();
			break;
		default:
			throw std::invalid_argument("Algorithm not configured or not supported");
		};
	}

	return !done;
}

void Graph::Reset() {
	done = false;
	temp = DEFAULT_TEMP;
}

bool Graph::fructhermanReingoldStep()
{
	bool equilibrium = true;
	vector<Vector2f> forces(nodes.size(), { 0, 0 });
	
	// Iterate through each node pair and calculate repulsive forces
	for (int i = 0; i < nodes.size(); ++i) {
		for (int j = i + 1; j < nodes.size(); ++j) {
			Vector2f rep = repulsive(nodes[i].pos, nodes[j].pos, L);

			if (DEBUGGING)
				cout << "Repulsive force " << i << "<->" << j << ": (" << rep.x << "," << rep.y << ")" << endl;

			forces[i] += rep;
			forces[j] -= rep;
		}
	}

	// Iterate through all edges and calculate attractive forces
	for (auto e : edges) {
		int i = e.nodes.x;
		int j = e.nodes.y;
		Vector2f attr = attractive(nodes[i].pos, nodes[j].pos, L);
		
		if (DEBUGGING)
			cout << "Attractive force " << i << "<->" << j << ": (" << attr.x << "," << attr.y << ")" << endl;

		forces[i] += attr;
		forces[j] -= attr;
	}

	// Apply forces
	for (int i = 0; i < nodes.size(); ++i) {
		// Scale forces to number of nodes
		forces[i] /= float(nodes.size());
		
		// add attractive force, pulling node to the center the further its away
		Vector2f center = { width / 2, height / 2 };
		Vector2f centerPull = attractive(nodes[i].pos, center, CANVAS_HEIGHT) / float(nodes.size());
		forces[i] += centerPull * Gravity;

		int signX = (forces[i].x > 0.f) - (forces[i].x < 0.f);
		int signY = (forces[i].y > 0.f) - (forces[i].y < 0.f);

		// Use temperature to limit displacement
		forces[i].x = min(abs(forces[i].x), temp) * signX;
		forces[i].y = min(abs(forces[i].y), temp) * signY;

		if ((abs(forces[i].x) > treshold) || (abs(forces[i].y) > treshold)) {
			equilibrium = false;
		}
		nodes[i].pos += forces[i];

		// Limit the node to be inside the window
		nodes[i].pos.x = max(nodes[i].pos.x, float(nodes[i].shape.getRadius()));
		nodes[i].pos.x = min(nodes[i].pos.x, float(width) - 2 * nodes[i].shape.getRadius());

		nodes[i].pos.y = max(nodes[i].pos.y, float(nodes[i].shape.getRadius()));
		nodes[i].pos.y = min(nodes[i].pos.y, float(height) - 2 * nodes[i].shape.getRadius());
		 
		nodes[i].shape.setPosition(nodes[i].pos);
	}

	temp *= cooling;

	if (DEBUGGING)
		cout << "Temp: " << temp << endl;

	return equilibrium;
}

const float thickness = 1.5f;

void Graph::draw(tgui::CanvasSFML::Ptr &target, sf::Font font)
{
	for (const Edge& e : edges) {
		auto node1 = nodes[e[0]].shape;
		auto node2 = nodes[e[1]].shape;
		target->draw(sfLine(node1.getPosition() + node1.getOrigin(), node2.getPosition() + node2.getOrigin(), thickness));
	}

	for (int i = 0; i < nodes.size(); ++i) {
		// scale node size based on degree
		float scaledR = nodeMin + (nodeMax - nodeMin) * (adjList[i].size() / (float)maxDegree);
		nodes[i].shape.setOrigin(scaledR / 2, scaledR / 2);
		nodes[i].shape.setRadius(scaledR);
		target->draw(nodes[i].shape);

		if (showLabels) {
			sf::Text label{ nodes[i].label, font};
			label.setFillColor(sf::Color::Red);
			label.setCharacterSize(18);
			FloatRect numRect = label.getGlobalBounds();
			Vector2f numRectCenter(numRect.width / 2.0f + numRect.left, numRect.height / 2.0f + numRect.top);
			label.setOrigin(numRectCenter);
			label.setPosition(nodes[i].pos.x, nodes[i].pos.y);
			target->draw(label);
		}

	}
}

void Graph::setNodeDimensions(float nodeMin, float nodeMax)
{
	this->nodeMin = nodeMin;
	this->nodeMax = nodeMax;
}

void Graph::setShowLabels(bool showLabels)
{
	this->showLabels = showLabels;
}

void Graph::RandomLayout(Vector2f pos, float L) {
	for (Node& n : nodes) {
		n.pos = { pos.x + generateRandomNumber(-L, L), pos.y + generateRandomNumber(-L, L) };
		n.shape.setPosition(n.pos);
	}

};

void Graph::RandomCircularLayout(Vector2f pos, float R) {
	for (Node& n : nodes) {
		float random = generateRandomNumber(0, 1);
		float angle = random * 2 * PI;
		n.pos = { pos.x + R * cos(angle), pos.y + R * sin(angle) };
		n.shape.setPosition(n.pos);
	}

};

void Graph::add_node(Node n)
{
	nodes.push_back(n);
	adjList.resize(adjList.size() + 1);
}

void Graph::add_edge(Edge e)
{
	edges.push_back(e);
	adjList[e[0]].push_back(e[1]);
	adjList[e[1]].push_back(e[0]);
}

const std::vector<Node>& Graph::Nodes() const
{
	return nodes;
}

const vector<Edge>& Graph::Edges() const
{
	return edges;
}

std::ostream& operator<<(std::ostream& os, const Graph& obj) {
	os << "Graph" << endl << "nodes: ";
	print_vector(obj.nodes);

	cout << "Edges:";
	print_vector(obj.edges);

	return os;
}


Graph Graph::fromGML(string file)
{
	ifstream src(file);
	string str;
	vector<Node> nodes;
	vector<Edge> edges;
	while (getline(src, str)) {
		if (str.find("graph") != string::npos) {
			break;
		}
	}

	int minus = 0;
	bool first = true;
	bool hasWeights = true;
	while (getline(src, str)) {
		auto trimmed = trim_copy(str);
		if (trimmed.find("node") != string::npos) {
			// Parse node
			while (str.find("id") == string::npos)
				getline(src, str);

			// 'id <id>'
			trim(str);
			int id = stoi(str.substr(3)) - minus;
			if (first) {
				if (id == 0) {
					minus = 0;
				}else {
					minus = 1;
					id -= 1;
				}
				first = false;
			}

			do
				getline(src, str);
			while (trim_copy(str) == "");

			string label = to_string(id);
			if (str.find("label") != string::npos) {
				// parse 'label <l>'
				trim(str);
				// trim the quotes
				if (str[str.length() - 1] == '"') {
					str.pop_back();
					label = str.substr(7);
				}
				else label = str.substr(6);
			}

			nodes.emplace_back(Node::from_id(id, label));
			// parse rest of node
			while (str.find(']') == string::npos && getline(src, str));
		}
		else if(trimmed.find("edge") != string::npos) {
			// Parse edge
			getline(src, str); // skip '['

			// 'source <id>'
			while (str.find("source") == string::npos)
				getline(src, str);
			trim(str);

			// 'target <id>'
			int source_id = stoi(str.substr(7)) - minus;
			while (str.find("target") == string::npos)
				getline(src, str);
			trim(str);
			int target_id = stoi(str.substr(7)) - minus;

			// 'value <num>'
			float w = 1;
			if (hasWeights) {
				while (str.find("value") == string::npos) {
					getline(src, str);
					if (str.find("]") != string::npos) {
						hasWeights = false;
						break;
					}
				}
				if (hasWeights) {
					trim(str);
					w = stof(str.substr(6));
				}
			}

			edges.emplace_back(Edge(source_id, target_id, w));
			// parse rest of edge
			while (str.find(']') == string::npos && getline(src, str));
		}
	}

	if (DEBUGGING) {
		cout << nodes.size() << " Nodes:" << endl;
		print_vector(nodes);
		cout << edges.size() << " Edges:" << endl;
		print_vector(edges);
	}

	return Graph(nodes, edges);
}