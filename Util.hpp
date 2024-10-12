#pragma once

#include <vector>
#include <iostream>
#include <random>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <chrono>
#include <SFML/System/Vector2.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

using namespace sf;
using namespace std;

// Define DBG macro for logging in debug mode
#ifdef DEBUG
#define DBG(msg) std::cout << msg << std::endl
#else
#define DBG(msg)
#endif

struct FruchtermanParams {
	// ideal spring length
	float L;
	// cooling factor
	float cooling;
	// width/height
	float W; float H;
};

extern struct FruchtermanParams params;

constexpr bool DEBUGGING = false;
const float WINDOW_WIDTH = 1800.f;
const float WINDOW_HEIGHT = 950.f;
const float BOTTOM_BAR = 0.f;
const float MENU_BAR = 28.f;
const float LEFT_MENU = 200.f;
const Vector2f CANVAS_OFFSET = { LEFT_MENU, MENU_BAR };
const float CANVAS_WIDTH = WINDOW_WIDTH - CANVAS_OFFSET.x;
const float CANVAS_HEIGHT = WINDOW_HEIGHT - BOTTOM_BAR - CANVAS_OFFSET.y;
const auto WINDOW_BG_COLOR = Color({ 120, 120, 120 });
const auto CANVAS_BG_COLOR = Color({ 169, 169, 169 });
extern float FRAMERATE;
extern bool RUNNING;
extern bool DONE;
extern std::chrono::time_point<std::chrono::high_resolution_clock> timeStart;

// Print vector
template<typename T>
inline void print_vector(const std::vector<T>& v) {
	if (v.empty()) {
		cout << "[]" << endl;
		return;
	}
	cout << "[";
	cout << v[0];
	for (int i = 1; i < v.size(); ++i) {
		cout << ", " << v.at(i);
	}
	cout << "]" << endl;
}

inline float Euclidian(Vector2f p1, Vector2f p2);

// Calculate repulsive force between two points
// p1, p2 - points
// l - ideal spring length for edges
// f_rep = l^2 / |p1-p2|
Vector2f repulsive(Vector2f p1, Vector2f p2, float l);

// Calculate attractive force between two points
// p1, p2 - points
// l - ideal spring length for edges
// f_attr = |p1-p2|^2 / l
Vector2f attractive(Vector2f p1, Vector2f p2, float l);

// Function to generate a random number between -A and A
inline float generateRandomNumber(float A, float B) {
	// Create a random number generator engine
	std::random_device rd;
	std::mt19937 gen(rd());

	// Create a uniform distribution in the range [-A, A]
	std::uniform_real_distribution<float> dis(A, B);

	// Generate and return a random number
	return dis(gen);
}

// trim from start (in place)
inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

// Calculate Fruchterman-Reingold parameters
// k = C * sqrt(area/num_vertices))
FruchtermanParams calcFruchtParams(const int num_nodes, float C = 0.7);