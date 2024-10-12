#include <iostream>
#include <vector>
#include <chrono>

#include "Gui.hpp"
#include "Util.hpp"
#include "Graph.hpp"

void addMenu(tgui::Gui& gui, Graph& G);
void openFileDialog(tgui::Gui& gui, Graph& G);
// Setup a control button (play/pause etc) in the left panel 
void setupControlButton(tgui::BitmapButton::Ptr& btn);


void GUI::initWidgets(tgui::Gui& gui, Graph& G)
{
	auto window = gui.getWindow();
	auto windowSize = window->getSize();

	addLeftPanel(gui, G);
	addMenu(gui, G);

}

static void openFileDialog(tgui::Gui& gui, Graph& G) {
	auto prev = gui.get<tgui::FileDialog>("openFile");
	if (prev != nullptr) {
		gui.remove(prev);
	}

	auto openFileDialog = tgui::FileDialog::create("Open file", "Open");
	openFileDialog->setFileTypeFilters({ {"GML", {"*.gml"}}, {"All files", {}} }, 0);
	openFileDialog->onFileSelect([&gui, &G](const vector<tgui::Filesystem::Path>& paths) {
		if (paths.size() > 1) {
			cout << "Can't load multiple files" << endl;
		}
		else if (paths.size() == 0) {
			cout << "No file loaded" << endl;
		}
		else {
			auto& path = paths[0];
			if (path.getFilename().ends_with(".gml")) {
				G = Graph::fromGML(path.asString().toStdString());
				G.RandomCircularLayout(Vector2f(CANVAS_WIDTH / 2.f, CANVAS_HEIGHT / 2.f), CANVAS_HEIGHT * 0.40);
				auto nodeSizer = gui.get<tgui::RangeSlider>("nodeSizer");
				float nodeMin = nodeSizer->getSelectionStart();
				float nodeMax = nodeSizer->getSelectionEnd();
				G.setNodeDimensions(nodeMin, nodeMax);
				G.FruchtermanReingold(params);
				DBG(G);
				GUI::updateWidgetsPause(gui);
			}
		}
		});

	gui.add(openFileDialog, "openFile");
}

static void saveFileDialog(tgui::Gui& gui) {
	auto prev = gui.get<tgui::FileDialog>("saveFile");
	if (prev != nullptr) {
		gui.remove(prev);
	}

	auto saveFileDialog = tgui::FileDialog::create("Save file", "Save");
	saveFileDialog->setFileMustExist(false);
	saveFileDialog->setPath(tgui::Filesystem::getCurrentWorkingDirectory() / "imgs");
	saveFileDialog->setFilename("screenshot.png");

	saveFileDialog->onFileSelect([&gui](const tgui::String& filePath) {
			auto canvas = gui.get<tgui::CanvasSFML>("canvas");
			RenderTexture& renderTexture = canvas->getRenderTexture();
			auto img = renderTexture.getTexture().copyToImage();
			img.saveToFile(filePath.toStdString() + ".png");
		});

	gui.add(saveFileDialog, "saveFile");

}

static void addMenu(tgui::Gui& gui, Graph& G) {
	auto menu = tgui::MenuBar::create();
	menu->setTextSize(14);
	menu->setHeight(28.f);
	menu->addMenu("File");
	menu->addMenuItem("Load");
	menu->addMenu("Help");
	menu->addMenuItem("About");

	menu->connectMenuItem({ "File", "Load" }, [&gui, &G] {
		openFileDialog(gui, G);
		});

	gui.add(menu);
}

static void setupControlButton(tgui::BitmapButton::Ptr& btn) {
	btn->setSize({ LEFT_MENU / 4, LEFT_MENU / 4 });
	btn->setImageScaling(1.f);
	btn->setFocusable(false);
	btn->setMouseCursor(tgui::Cursor::Type::Hand);
	auto render = btn->getRenderer();
	render->setBackgroundColor(Color::Transparent);
	render->setBackgroundColorDown(Color::Transparent);
	render->setBackgroundColorHover(Color::Transparent);
	render->setBorderColor(Color::Transparent);
	render->setBorderColorDown(Color::Transparent);
	render->setBorderColorHover(Color::Transparent);
}




void  GUI::updateWidgetsDone(tgui::Gui& gui)
{
	gui.get<tgui::BitmapButton>("playBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("pauseBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("nextBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("resetBtn")->setVisible(true);
}

void  GUI::updateWidgetsStart(tgui::Gui& gui) {
	gui.get<tgui::BitmapButton>("playBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("nextBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("pauseBtn")->setVisible(true);
}

void  GUI::updateWidgetsPause(tgui::Gui& gui) {
	gui.get<tgui::BitmapButton>("playBtn")->setVisible(true);
	gui.get<tgui::BitmapButton>("nextBtn")->setVisible(true);
	gui.get<tgui::BitmapButton>("pauseBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("resetBtn")->setVisible(false);
}

void  GUI::updateWidgetsReset(tgui::Gui& gui) {
	gui.get<tgui::BitmapButton>("resetBtn")->setVisible(false);
	gui.get<tgui::BitmapButton>("playBtn")->setVisible(true);
	gui.get<tgui::BitmapButton>("nextBtn")->setVisible(true);
}

void GUI::addLeftPanel(tgui::Gui& gui, Graph& G) {
	auto algoSelectLabel = tgui::Label::create("Select algorithm:");
	algoSelectLabel->setPosition(LEFT_MENU / 8.f, 80.f);
	algoSelectLabel->setTextSize(14);
	algoSelectLabel->getRenderer()->setTextColor(Color::White);
	auto algoSelect = tgui::ComboBox::create();
	auto algoPos = tgui::Layout2d(LEFT_MENU / 8.f, 100.f);
	algoSelect->setTextSize(12);
	algoSelect->setPosition(algoPos);
	algoSelect->addItem("FruchtermanReingold");
	algoSelect->setSelectedItemByIndex(0);

	algoSelect->onItemSelect([](const tgui::String& item) {
		// Algorithm selected
		});

	auto playPos = tgui::Layout2d(LEFT_MENU / 2.f - LEFT_MENU / 8.f, 150.f);
	auto nextPos = tgui::Layout2d(LEFT_MENU / 2.f + LEFT_MENU / 8.f, 150.f);

	auto playBtn = tgui::BitmapButton::create();
	playBtn->setImage("icons/play.png");
	playBtn->setPosition(playPos);
	setupControlButton(playBtn);

	auto prevBtn = tgui::BitmapButton::create();
	prevBtn->setImage("icons/previous.png");
	prevBtn->setPosition(LEFT_MENU / 2, 200.f);
	setupControlButton(prevBtn);

	auto nextBtn = tgui::BitmapButton::create();
	nextBtn->setImage("icons/next.png");
	nextBtn->setPosition(nextPos);
	setupControlButton(nextBtn);

	auto pauseBtn = tgui::BitmapButton::create();
	pauseBtn->setImage("icons/pause.png");
	pauseBtn->setPosition(playPos);
	pauseBtn->setVisible(false);
	setupControlButton(pauseBtn);

	auto resetBtn = tgui::BitmapButton::create();
	resetBtn->setImage("icons/reset.png");
	resetBtn->setPosition(playPos);
	resetBtn->setVisible(false);
	setupControlButton(resetBtn);

	playBtn->onPress([&gui]() {
		RUNNING = true;
		updateWidgetsStart(gui);
		timeStart = std::chrono::high_resolution_clock::now();
		});

	nextBtn->onPress([&G]() {
		G.Update();
		});

	pauseBtn->onPress([&G, &gui]() {
		RUNNING = false;
		updateWidgetsPause(gui);
		});

	resetBtn->onPress([&gui, &G]() {
		G.RandomCircularLayout(Vector2f(CANVAS_WIDTH / 2.f, CANVAS_HEIGHT / 2.f), CANVAS_HEIGHT * 0.40);
		G.Reset();
		updateWidgetsReset(gui);
		});

	resetBtn->setVisible(false);

	auto slider = tgui::Slider::create();
	slider->setMinimum(1);
	slider->setMaximum(100);
	slider->setStep(10);
	slider->setValue(100);
	slider->setSize({ LEFT_MENU*3/4.f, 10.f });
	slider->setPosition(LEFT_MENU/8, playPos.y + 100.f);

	auto sliderLabel = tgui::Label::create("Speed");
	sliderLabel->setTextSize(14);
	sliderLabel->getRenderer()->setTextColor(Color::White);
	sliderLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	sliderLabel->setPosition({ LEFT_MENU/2-24.f, slider->getPosition().y - 25.f });

	slider->onValueChange([](float value) {
		cout << "Framerate changed to " << value << endl;
		if (value == 100) {
			FRAMERATE = 0;
		}
		else {
			FRAMERATE = value;
		}
		});

	auto kSlider = tgui::Slider::create();
	kSlider->setMinimum(0);
	kSlider->setMaximum(2);
	kSlider->setStep(0.05);
	kSlider->setValue(1);
	kSlider->setSize({ LEFT_MENU * 3 / 4.f, 10.f });
	kSlider->setPosition(LEFT_MENU / 8, slider->getPosition().y + 100.f);

	auto KsliderLabel = tgui::Label::create("algorithm parameter C");
	KsliderLabel->setTextSize(14);
	KsliderLabel->getRenderer()->setTextColor(Color::White);
	KsliderLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	KsliderLabel->setPosition({ LEFT_MENU/2 - 80.f, kSlider->getPosition().y - 25.f });

	kSlider->onValueChange([&G](float value) {
		params = calcFruchtParams(G.Nodes().size(), value);
		G.FruchtermanReingold(params);
	});

	auto nodeSizerLabel = tgui::Label::create("Node sizes (min, max)");
	nodeSizerLabel->setTextSize(14);
	nodeSizerLabel->getRenderer()->setTextColor(Color::White);
	nodeSizerLabel->setPosition({ LEFT_MENU / 8,  kSlider->getPosition().y + 80.f });

	auto nodeSizer = tgui::RangeSlider::create(1.f, 30.f);
	nodeSizer->setSize({ LEFT_MENU * 3 / 4.f, 15.f });
	nodeSizer->setPosition({ LEFT_MENU / 8,  nodeSizerLabel->getPosition().y + 30.f });
	nodeSizer->setStep(0.2f);
	nodeSizer->setSelectionStart(8.f); // Position for first knob
	nodeSizer->setSelectionEnd(10.f); // Position for second knob

	nodeSizer->onRangeChange([&G](float start, float end) {
		G.setNodeDimensions(start, end);
	});

	auto showLabelsCheck = tgui::CheckBox::create("Show labels");
	showLabelsCheck->setChecked(false);
	showLabelsCheck->setTextSize(14);
	showLabelsCheck->getRenderer()->setTextColor(Color::White);
	showLabelsCheck->setTextClickable(false);
	showLabelsCheck->setPosition({ LEFT_MENU / 4,  nodeSizer->getPosition().y + 50.f });

	showLabelsCheck->onChange([&G](bool checked) {
		G.setShowLabels(checked);
	});

	auto saveBtn = tgui::BitmapButton::create();
	saveBtn->setImage("icons/save.png");
	saveBtn->setPosition({ LEFT_MENU / 2.f - LEFT_MENU / 8.f, showLabelsCheck->getPosition().y + 80.f});
	setupControlButton(saveBtn);

	saveBtn->onPress([&gui]() {
		cout << "Saving screenshot . . ." << endl;
		saveFileDialog(gui);
	});

	gui.add(algoSelect, "algoSelect");
	gui.add(algoSelectLabel, "algoSelectLabel");
	gui.add(resetBtn, "resetBtn");
	gui.add(playBtn, "playBtn");
	gui.add(pauseBtn, "pauseBtn");
	gui.add(nextBtn, "nextBtn");
	gui.add(slider, "speedSlider");
	gui.add(sliderLabel, "sliderLabel");
	gui.add(kSlider, "kSlider");
	gui.add(KsliderLabel, "kSliderLabel");
	gui.add(nodeSizerLabel, "nodeSizerLabel");
	gui.add(nodeSizer, "nodeSizer");
	gui.add(showLabelsCheck, "showLabels");
	gui.add(saveBtn, "saveBtn");
}