#pragma once

#include <SFML/System/Vector2.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

class Graph;

class GUI {
public:

	// Setup widgets when creating the application
	static void initWidgets(tgui::Gui& gui, Graph& G);


	// Update the widgets when a simulation is done/started/paused/reset
	static void updateWidgetsDone(tgui::Gui& gui);
	static void updateWidgetsStart(tgui::Gui& gui);
	static void updateWidgetsPause(tgui::Gui& gui);
	static void updateWidgetsReset(tgui::Gui& gui);
	~GUI() = delete;

private:
	static void addLeftPanel(tgui::Gui&, Graph&);
};
