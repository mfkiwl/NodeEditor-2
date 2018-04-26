#pragma once

#include <SFML\Graphics\RenderWindow.hpp>

#include "imgui\imgui.h"
#include "imgui\imgui-SFML.h"

#include <memory>
#include <map>
#include <vector>

class NodeTemplate;
class NodeData;
class Camera;

class NodeEditor
{
private:
	NodeEditor();
	~NodeEditor();

	static NodeEditor * instance;

public:
	static NodeEditor & get();
	static void create();

	ImFont * nodeHighResFont;
	ImFont * uiFont;
	ImFont * defaultFont;

private:
	sf::RenderWindow * sfmlWindow = nullptr;

	bool nodeListOpen = true;
	void drawWindowNodeList();
	void drawWindowNodeViewer();


	Camera * nodeViewerCamera;
	//NodeTemplate * nodeTemplate;
	//NodeData * nodeData;

	std::map<int, std::shared_ptr<NodeTemplate>> nodeTemplates;
	std::map<int, std::shared_ptr<NodeData>> nodeDatas;

	float scrollDelta = 0.0f;

public:

	void start();
	void loadFonts();
	void load();


	const NodeData & getNodeData(int _id) const;
	const NodeTemplate & getNodeTemplate(int _id) const;
};

