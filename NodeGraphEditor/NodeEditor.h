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

	void mouseDown(const sf::Vector2f & _pos);
	void mouseUpdate(const sf::Vector2f & _pos);
	void mouseUp(const sf::Vector2f & _pos);
	void resetDrag();


	Camera * nodeViewerCamera;
	//NodeTemplate * nodeTemplate;
	//NodeData * nodeData;

	std::map<int, std::shared_ptr<NodeTemplate>> nodeTemplates;
	std::map<int, std::shared_ptr<NodeData>> nodeDatas;

	float scrollDelta = 0.0f;


/* TODO: put collision code in functions like this*/
	bool doesScreenPositionCollideWithNode(int & _node, sf::Vector2f & _initialOffset, const sf::Vector2f & _position);
	bool doesScreenPositionCollideWithJoint(int & _node, int & _property, const sf::Vector2f & _position, bool _jointIsInput);


	enum DragType
	{
		NONE,
		NODEDATA,
		INPUTJOINT,
		OUTPUTJOINT
	} dragType;
	int nodeDataDragID = -1;
	int propertyIDDrag = -1;
	sf::Vector2f dragInitialOffset{ 0, 0 };


public:

	void start();
	void loadFonts();
	void load();


	const NodeData & getNodeData(int _id) const;
	const NodeTemplate & getNodeTemplate(int _id) const;
};

