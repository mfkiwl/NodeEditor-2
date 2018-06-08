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
class NodeConnection;

class Program
{
private:
	Program();
	~Program();

	static Program * instance;

public:
	///
	/// Returns the singleton instance of the NodeEditor program
	///
	static Program & get();

	///
	/// Creates the singleton instance of the NodeEditor program
	///
	static void create();

	///
	/// The high resolution font for rendering
	///
	ImFont * nodeHighResFont;

	///
	/// The UI font for rendering
	///
	ImFont * uiFont;

	///
	/// The default ImGui font
	///
	ImFont * defaultFont;

	///
	/// The number of property data types
	///
	int propertyTypeCount = 0;

private:
	sf::RenderWindow * sfmlWindow = nullptr;


	std::vector<std::vector<bool>> connectionMatrix;


	void loadData();
	void loadFonts();
	void load();

	void closeCurrent();
	void open();
	void save();


	bool nodeListOpen = true;
	void drawMainMenu();
	void drawWindowNodeList();
	void drawWindowNodeViewer();

	void mouseDown(const sf::Vector2f & _pos);
	void mouseUpdate(const sf::Vector2f & _pos);
	void mouseUp(const sf::Vector2f & _pos);
	void resetDrag();


	Camera * nodeViewerCamera;

	std::map<int, std::shared_ptr<NodeTemplate>> nodeTemplates;
	std::map<int, std::shared_ptr<NodeData>> nodeDatas;

	float scrollDelta = 0.0f;


	bool doesScreenPositionCollideWithNode(int & _node, sf::Vector2f & _initialOffset, const sf::Vector2f & _position);
	bool doesScreenPositionCollideWithJoint(int & _node, int & _property, const sf::Vector2f & _position, bool _jointIsInput);


	enum DragType
	{
		NONE,
		NODEDATA,
		CONNECTION
	} dragType;
	int nodeDataDragID = -1;
	int propertyIDDrag = -1;
	sf::Vector2f dragInitialOffset{ 0, 0 };

	sf::Vector2f nodePlacementPosition{ 0, 0 };
	int nextNodeID = 0;

	bool isValidConnection(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex);
	bool tryConnectNodes(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex);
	void tryDisconnectNodes(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex);
	void disconnectInputGivenOutput(NodeConnection & _outputConnection);
	void disconnectOutputGivenInput(NodeConnection & _inputConnection);

	void deleteAllConnections(int _nodeID);

	void deleteSelected();

	int nodeDataSelection = -1;


	void showTooltip(const std::string & _text, bool _good = true);

	bool chooseFile(const std::string & _title, bool _isOpen, std::string & _filename) const;

public:

	///
	/// Starts the NodeEditor program
	///
	void start();


	///
	/// Returns a node
	/// _id - the ID of the node
	///
	const NodeData & getNodeData(int _id) const;

	///
	/// Returns a node template
	/// _id - the ID of the node template
	///
	const NodeTemplate & getNodeTemplate(int _id) const;
};

