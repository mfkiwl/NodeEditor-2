#include "Program.h"

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\System\Clock.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Graphics\CircleShape.hpp>

//TODO: REMOVE:
#include <iostream>

#include <algorithm>
#include <fstream>
#include "json.hpp"

#include "NodeTemplate.h"
#include "NodeData.h"
#include "Camera.h"
#include "Util.h"

Program * Program::instance = nullptr;

Program & Program::get()
{
	return *instance;
}

void Program::create()
{
	instance = new Program();
}

void Program::loadFonts()
{
	auto io = ImGui::GetIO();

	defaultFont = io.Fonts->AddFontDefault();
	uiFont = io.Fonts->AddFontFromFileTTF("inconsolata\\Inconsolata-Regular.ttf", 13);
	nodeHighResFont = io.Fonts->AddFontFromFileTTF("inconsolata\\Inconsolata-Regular.ttf", 64);

	ImGui::SFML::UpdateFontTexture();
}

void Program::load()
{
	loadFonts();


	nodeViewerCamera = new Camera();


	nodeTemplates[5] = std::make_shared<NodeTemplate>(5,
		"Crazy Operation", 
		std::vector<Property>{
			Property("Input 1", 0),
			Property("Input Thing 2", 1)
		}, 
		std::vector<Property>{
			Property("Ouput 1", 0),
			Property("Output Thing 2", 1),
			Property("An Output 3", 2)
		});
	nodeTemplates[9] = std::make_shared<NodeTemplate>(9,
		"Add",
		std::vector<Property>{
			Property("Input 1", 0),
			Property("Input 2", 1)
		},
		std::vector<Property>{
			Property("Ouput 1", 2)
		});


	nodeDatas[3] = std::make_shared<NodeData>(3, 5, sf::Vector2f{ 100, 100 });
	nodeDatas[4] = std::make_shared<NodeData>(4, 5, sf::Vector2f{ 500, 100 });
	nodeDatas[5] = std::make_shared<NodeData>(5, 9, sf::Vector2f{ 100, 500 });
	nodeDatas[6] = std::make_shared<NodeData>(6, 9, sf::Vector2f{ 500, 500 });


/*
	nodeDatas[3]->nodeConnections.push_back(NodeConnection{ 3, 1, 6, 0 });
	nodeDatas[5]->nodeConnections.push_back(NodeConnection{ 5, 0, 6, 1 });*/

}

void Program::closeCurrent()
{
	nodeViewerCamera = new Camera();
	nodeDatas.clear();
}

void Program::open()
{
	using namespace nlohmann;

	json inputJson;

	std::ifstream openFile("output.json");

	openFile >> inputJson;

	openFile.close();


	for (auto itr = inputJson.begin(); itr != inputJson.end(); ++itr)
	{
		const json nodeDataJson = *itr;

		std::shared_ptr<NodeData> deserialisedNodeData = std::make_shared<NodeData>(NodeData::deserialise(nodeDataJson));

		nodeDatas[deserialisedNodeData->id] = deserialisedNodeData;
	}
}

void Program::save()
{
	using namespace nlohmann;


	json outputJson;

	for (const auto & _nodeData : nodeDatas)
	{
		const auto & nodeData = _nodeData.second;

		json nodeSerialised = nodeData->serialise();

		outputJson.push_back(nodeSerialised);
	}



	std::ofstream saveFile("output.json");

	saveFile << outputJson;

	saveFile.close();

}

void Program::drawMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New");
			if (ImGui::MenuItem("Open"))
			{
				open();
			}
			if (ImGui::MenuItem("Save"))
			{
				save();
			}
			if (ImGui::MenuItem("Close"))
			{
				closeCurrent();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Settings");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void Program::drawWindowNodeList()
{
	////ImGui::SetNextWindowPos({ 0, 0 });
	////ImGui::SetNextWindowSize({ 300, 200 });
	ImGui::Begin("Node List");
	ImGui::PushFont(uiFont);
	//ImGui::Button("Example node 1");
	//ImGui::Button("Example node 2");
	////ImGui::TreeNode("Category 1");
	////ImGui::TreePop();
	////ImGui::TreeNode("Category 2");
	////ImGui::TreePop();

	if (ImGui::TreeNode("Category 1"))
	{
		ImGui::Button("Example node 3");
		ImGui::Button("Example node 4");
		ImGui::Button("Example node 5");

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Category 2"))
	{
		ImGui::Button("Example node 6");
		ImGui::Button("Example node 7");
		ImGui::Button("Example node 8");
		ImGui::Button("Example node 9");
		ImGui::Button("Example node 10");

		ImGui::TreePop();
	}

	sf::Vector2f pos = ImGui::GetWindowPos();
	sf::Vector2f size = ImGui::GetWindowSize();
	sf::Vector2f max = (sf::Vector2f)pos + (sf::Vector2f)size;





	ImGui::PopFont();
	ImGui::End();
}

void Program::drawWindowNodeViewer()
{
	if (ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove))
	{
		ImGui::PushFont(nodeHighResFont);

		//Get position variables
		sf::Vector2f windowPos = ImGui::GetWindowPos();
		sf::Vector2f windowSize = ImGui::GetWindowSize();

		//Update camera
		nodeViewerCamera->setWindowPosition(windowPos);

		//Respond to input
		if (ImGui::IsMouseHoveringWindow())
		{
			if (ImGui::IsMouseDragging(1))
			{
				nodeViewerCamera->getPosition() += sf::Vector2f(ImGui::GetMouseDragDelta(1)) / nodeViewerCamera->getScale();
				ImGui::ResetMouseDragDelta(1);
			}

			//Update Scale
			{
				float scaleMul = (scrollDelta / 10.0f) + 1.0f;
				scaleMul = scaleMul > 0.5f ? scaleMul : 0.5f;
				scaleMul = scaleMul < 1.5f ? scaleMul : 1.5f;

				float oldScale = nodeViewerCamera->getScale();

				sf::Vector2f delFromScale = windowSize * (1.f - scaleMul) / (2.f * nodeViewerCamera->getScale() * scaleMul);

				nodeViewerCamera->getScale() *= scaleMul;

				nodeViewerCamera->getPosition() += delFromScale;
			}
		}

		//Update camera
		nodeViewerCamera->updateTransform();

		
		//Actually draw
		auto drawList = ImGui::GetWindowDrawList();

		//Draw grid
		{
			sf::Vector2f yStart = nodeViewerCamera->getTransform().transformPoint({ 0, -1000 });
			sf::Vector2f yEnd = nodeViewerCamera->getTransform().transformPoint({ 0, 1000 });
			drawList->AddLine(yStart, yEnd, ImColor::HSV(0.5f, 1, 1));


			sf::Vector2f xStart = nodeViewerCamera->getTransform().transformPoint({ -1000, 0 });
			sf::Vector2f xEnd = nodeViewerCamera->getTransform().transformPoint({ 1000, 0 });
			drawList->AddLine(xStart, xEnd, ImColor::HSV(0.5f, 1, 1));
		}

		//Draw nodes
		{
			for (const std::pair<int, std::shared_ptr<NodeData>> & nodeDataPair : nodeDatas)
			{
				const NodeData & nodeData = *nodeDataPair.second;
				NodeTemplate & nodeTemplate = *nodeTemplates[nodeData.nodeTemplateID];

				nodeTemplate.draw(drawList, *nodeViewerCamera, nodeData, nodeDataPair.first == nodeDataSelection);
			}
		}

		ImGui::PopFont();



		//Draw temporary connections to mouse position
		if (dragType == DragType::CONNECTION)
		{
			const NodeData & nodeData = *nodeDatas[nodeDataDragID];
			NodeTemplate & nodeTemplate = *nodeTemplates[nodeData.nodeTemplateID];

			auto startPos = nodeTemplate.getOutputJointCircleRegion(propertyIDDrag, *nodeViewerCamera, nodeData).first;
			drawConnectionRaw(drawList, *nodeViewerCamera, startPos, ImGui::GetMousePos(), nodeTemplate.getOutputProperty(propertyIDDrag).type);
		}
	}
	ImGui::End();
}

void Program::mouseDown(const sf::Vector2f & _pos)
{
	int inputTempNodeDataDragID = -1;
	int inputTempPropertyDragID = -1;


	if (doesScreenPositionCollideWithNode(nodeDataDragID, dragInitialOffset, _pos))
	{
		dragType = DragType::NODEDATA;
		nodeDataSelection = nodeDataDragID;
	}
	else if (doesScreenPositionCollideWithJoint(inputTempNodeDataDragID, inputTempPropertyDragID, _pos, true))
	{
		//If there is a connection ending at this property then delete and start dragging this connection
		
		auto & inputConnections = nodeDatas[inputTempNodeDataDragID]->inputConnections;
		auto itr = inputConnections.find(inputTempPropertyDragID);
		if (itr != inputConnections.end())
		{
			//Found a connection
			//Delete it and start dragging

			nodeDataDragID = itr->second.startNodeID;
			propertyIDDrag = itr->second.startNodeOutputPropertyIndex;

			tryDisconnectNodes(itr->second.startNodeID, itr->second.startNodeOutputPropertyIndex, itr->second.endNodeID, itr->second.endNodeInputPropertyIndex);

			dragType = DragType::CONNECTION;
		}
		

/*
		std::vector<NodeConnection> & connections = nodeDatas[inputTempNodeDataDragID]->nodeConnections;
		
		bool foundConnection = false;
		for (NodeConnection & connection : connections)
		{
			if (connection.)
		}*/

		//dragType = DragType::CONNECTION;
	}
	else if (doesScreenPositionCollideWithJoint(nodeDataDragID, propertyIDDrag, _pos, false))
	{
		dragType = DragType::CONNECTION;
	}
}

void Program::mouseUp(const sf::Vector2f & _pos)
{
	//Check for attaching joints output->input
	if (dragType == DragType::CONNECTION)
	{
		int newNodeID = -1;
		int newPropertyID = -1;
		if (doesScreenPositionCollideWithJoint(newNodeID, newPropertyID, _pos, true))
		{
			tryConnectNodes(nodeDataDragID, propertyIDDrag, newNodeID, newPropertyID);


			//nodeDatas[nodeDataDragID]->outputConnections[propertyIDDrag] = NodeConnection{ nodeDataDragID, propertyIDDrag, newNodeID, newPropertyID };
			//nodeDatas[nodeDataDragID]->outputConnections.push_back(NodeConnection{ nodeDataDragID, propertyIDDrag, newNodeID, newPropertyID });

		}
	}

	resetDrag();
}

void Program::resetDrag()
{

	dragType = DragType::NONE;
	nodeDataDragID = -1;
	propertyIDDrag = -1;
	dragInitialOffset = { 0, 0 };
}

bool Program::doesScreenPositionCollideWithNode(int & _node, sf::Vector2f & _initialOffset, const sf::Vector2f & _position)
{
	for (const std::pair<int, std::shared_ptr<NodeData>> & nodeDataPair : nodeDatas)
	{
		const NodeData & nodeData = *nodeDataPair.second;
		NodeTemplate & nodeTemplate = *nodeTemplates[nodeData.nodeTemplateID];

		sf::FloatRect region = nodeTemplate.getHeaderRegion(*nodeViewerCamera, nodeData);

		if (region.contains(_position))
		{
			//Set node ID
			_node = nodeDataPair.first;

			//Set initial offset position
			_initialOffset = _position - sf::Vector2f{ region.left, region.top };
			return true;
		}
	}

	return false;
}

bool Program::doesScreenPositionCollideWithJoint(int & _node, int & _property, const sf::Vector2f & _position, bool _jointIsInput)
{
	for (const std::pair<int, std::shared_ptr<NodeData>> & nodeDataPair : nodeDatas)
	{
		const NodeData & nodeData = *nodeDataPair.second;
		NodeTemplate & nodeTemplate = *nodeTemplates[nodeData.nodeTemplateID];

		if (_jointIsInput)
		{
			for (int i = 0; i < nodeTemplate.getInputsCount(); ++i)
			{
				//auto jointRegion = nodeTemplate.getInputJointCircleRegion(i, *nodeViewerCamera, nodeData);
				//if (circleContainsPoint(jointRegion, _position))


				auto jointRegion = nodeTemplate.getInputJointRegion(i, *nodeViewerCamera, nodeData);
				if (jointRegion.contains(_position))
				{
					_node = nodeDataPair.first;
					_property = i;

					std::cout << "Mouse down on node: " << nodeDataPair.first << ", input property: " << i << std::endl;

					return true;
				}
			}
		}
		else
		{
			for (int i = 0; i < nodeTemplate.getOutputsCount(); ++i)
			{
				//auto jointRegion = nodeTemplate.getOutputJointCircleRegion(i, *nodeViewerCamera, nodeData);
				//if (circleContainsPoint(jointRegion, _position))


				auto jointRegion = nodeTemplate.getOutputJointRegion(i, *nodeViewerCamera, nodeData);
				if (jointRegion.contains(_position))
				{
					_node = nodeDataPair.first;
					_property = i;

					std::cout << "Mouse down on node: " << nodeDataPair.first << ", output property: " << i << std::endl;

					return true;
				}
			}
		}
		
		
	}

	return false;
}

void Program::mouseUpdate(const sf::Vector2f & _pos)
{
	sf::Vector2f worldPos = nodeViewerCamera->getTransform().getInverse().transformPoint(_pos - dragInitialOffset);

	switch (dragType)
	{
	case DragType::NODEDATA:
		if (nodeDataDragID != -1)
		{
			nodeDatas[nodeDataDragID]->position = worldPos;
		}
		break;

	case DragType::CONNECTION:

		break;
	}
}

Program::Program()
{
}


Program::~Program()
{
}

bool Program::tryConnectNodes(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex)
{
	//Can't connect node to itself
	if (_startNode == _endNode)
	{
		return false;
	}

	//Can't connect different types
	auto startTemplate = nodeTemplates[nodeDatas[_startNode]->nodeTemplateID];
	auto endTemplate = nodeTemplates[nodeDatas[_endNode]->nodeTemplateID];
	if (startTemplate->getOutputProperty(_startPropertyIndex).type != endTemplate->getInputProperty(_endPropertyIndex).type)
	{
		return false;
	}

	//See if something is already connected to the end node input property
	auto itr = nodeDatas[_endNode]->inputConnections.find(_endPropertyIndex);
	if (itr != nodeDatas[_endNode]->inputConnections.end())
	{
		//Disconnect first
		auto connection = itr->second;
		tryDisconnectNodes(connection.startNodeID, connection.startNodeOutputPropertyIndex, connection.endNodeID, connection.endNodeInputPropertyIndex);
	}

	nodeDatas[_startNode]->outputConnections.emplace(std::pair<int, NodeConnection>{_startPropertyIndex, NodeConnection{ _startNode, _startPropertyIndex, _endNode, _endPropertyIndex }});
	nodeDatas[_endNode]->inputConnections[_endPropertyIndex] = NodeConnection{ _startNode, _startPropertyIndex, _endNode, _endPropertyIndex };


	return true;
}

void Program::tryDisconnectNodes(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex)
{
	//Find the right output connection
	auto & outputConnections = nodeDatas[_startNode]->outputConnections;
	auto itr = std::find_if(outputConnections.begin(), outputConnections.end(), [_endNode, _endPropertyIndex](std::pair<int, NodeConnection> _connection) {
		return _connection.second.endNodeID == _endNode &&
			_connection.second.endNodeInputPropertyIndex == _endPropertyIndex;
	});

	if (itr != outputConnections.end())
	{
		outputConnections.erase(itr);
	}

	nodeDatas[_endNode]->inputConnections.erase(_endPropertyIndex);
}

void Program::deleteAllConnections(int _nodeID)
{
	auto & nodeData = nodeDatas[_nodeID];
	//Outputs first
	for (auto itr = nodeData->outputConnections.begin(); itr != nodeData->outputConnections.end(); ++itr)
	{

	}
}

void Program::start()
{
	sf::RenderWindow & window = *(sfmlWindow = new sf::RenderWindow(sf::VideoMode(1280, 768), "Node Graph Editor"));
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window, false);

	load();
	

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::Resized) {
				window.setSize({ event.size.width, event.size.height });
			}
			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				scrollDelta = event.mouseWheelScroll.delta;
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == 0)
				{
					mouseDown(sf::Vector2f{ (float)event.mouseButton.x, (float)event.mouseButton.y });
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == 0)
				{
					mouseUp(sf::Vector2f{ (float)event.mouseButton.x, (float)event.mouseButton.y });
				}
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Delete)
				{
					//TODO: also delete node connections
					nodeDatas.erase(nodeDataSelection);
					nodeDataSelection = -1;
				}
			}
		}

		mouseUpdate(ImGui::GetMousePos());


		//SETUP
		ImGui::SFML::Update(window, deltaClock.restart());


		//Draw windows
		drawMainMenu();
		drawWindowNodeList();
		drawWindowNodeViewer();
		


		//RENDER
		window.resetGLStates();
		window.clear();

		//DRAW IMGUI
		ImGui::SFML::Render(window);





		window.display();

		//Reset
		scrollDelta = 0.0f;
	}

	ImGui::SFML::Shutdown();
}

const NodeData & Program::getNodeData(int _id) const
{
	const NodeData & nodeData = *nodeDatas.at(_id);
	return nodeData;
}

const NodeTemplate & Program::getNodeTemplate(int _id) const
{
	const NodeTemplate & nodeTemplate = *nodeTemplates.at(_id);
	return nodeTemplate;
}