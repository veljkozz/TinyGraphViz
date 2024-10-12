
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>

#include "Util.hpp"
#include "Node.hpp"
#include "Edge.hpp"
#include "Graph.hpp"
#include "Gui.hpp"

using namespace sf;
using namespace std;

Graph G;
// Current algorithm parameters
FruchtermanParams params;

// Example graphs
const string ZACHARY_GML = "graphs/Zachary.gml";
const string LES_MISERABLES_GML = "graphs/LesMiserables.gml";
const string K5 = "graphs/k5.gml";

bool KeyPressed = false;
bool RUNNING = false;
bool DONE = false;
std::chrono::time_point<std::chrono::high_resolution_clock> timeStart;

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "GraphVisualizer", Style::Titlebar | Style::Close);

    // Position the window in the center of the screen
    auto desktop = sf::VideoMode::getDesktopMode();
    window.setPosition({ (int)(desktop.width / 2 - window.getSize().x / 2), (int)(desktop.height / 2 - window.getSize().y / 2 - 60) });

    window.setFramerateLimit(FRAMERATE);
    auto currFramerate = FRAMERATE;

    Font font;
    if (!font.loadFromFile("CourierPrime-Regular.ttf"))
    {
        exit(0xDEAD);
    }

    // Create GUI object
    tgui::Gui gui(window);

    // Create TGUI canvas for graph drawing
    auto canvas = tgui::CanvasSFML::create({ CANVAS_WIDTH, CANVAS_HEIGHT });
    canvas->setPosition(tgui::Layout2d(CANVAS_OFFSET));
    gui.add(canvas, "canvas");

    Graph G = Graph::fromGML(ZACHARY_GML);
    DBG(G);

    G.RandomCircularLayout(Vector2f(CANVAS_WIDTH / 2.f, CANVAS_HEIGHT / 2.f), CANVAS_HEIGHT * 0.40);
    params = calcFruchtParams(G.Nodes().size());
    G.FruchtermanReingold(params);

    GUI::initWidgets(gui, G);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                cout << "Pressed exit" << endl;
                window.close();
            }

            gui.handleEvent(event);
        }

        // Update
        if (!DEBUGGING && RUNNING) {
            if (!G.Update()) {
                // Simulation has ended
                RUNNING = false;
                GUI::updateWidgetsDone(gui);
                auto timeEnd = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(timeEnd - timeStart);
                cout << "Equillibrium reached in " << duration.count() << " milliseconds" << endl;
            }
        }

        if (currFramerate != FRAMERATE) {
            window.setFramerateLimit(FRAMERATE);
            currFramerate = FRAMERATE;
        }

        // Draw
        window.clear(WINDOW_BG_COLOR);

        canvas->clear(CANVAS_BG_COLOR);
        G.draw(canvas, font);
        canvas->display();

        gui.draw();

        window.display();
    }

	return 0;
}