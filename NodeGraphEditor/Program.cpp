#include "Program.h"

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\System\Clock.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Graphics\CircleShape.hpp>

#include <algorithm>
#include <fstream>
#include <Windows.h>
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

void Program::loadData()
{
	using namespace nlohmann;
	
	json dataJson;

	std::ifstream dataFile("data.json");

	dataFile >> dataJson;

	dataFile.close();

	//LOAD PROPERTY COUNT
	propertyTypeCount = dataJson.at("propertyTypeCount").get<int>();

	//LOAD CONNECTION MATRIX
	connectionMatrix.resize(propertyTypeCount);


	auto & connectionMatrixJson = dataJson.at("connectionMatrix");
	for (int i = 0; i < propertyTypeCount; ++i)
	{
		connectionMatrix[i].resize(propertyTypeCount);

		auto & connectionSubMatrixJson = connectionMatrixJson[i];

		for (int j = 0; j < propertyTypeCount; ++j)
		{
			connectionMatrix[i][j] = connectionSubMatrixJson[j].get<bool>();
		}
	}

	//LOAD NODE TEMPLATES
	auto & nodeTemplatesJson = dataJson.at("nodeTemplates");

	for (auto & nodeTemplateJson : nodeTemplatesJson)
	{
		std::shared_ptr<NodeTemplate> deserialisedNodeTemplate = std::make_shared<NodeTemplate>(NodeTemplate::deserialise(nodeTemplateJson));

		nodeTemplates[deserialisedNodeTemplate->id] = deserialisedNodeTemplate;
	}
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
	loadData();

	loadFonts();


	nodeViewerCamera = new Camera();
}

void Program::closeCurrent()
{
	nodeViewerCamera = new Camera();
	nodeDatas.clear();
	nextNodeID = 0;
}

void Program::open()
{
	std::string filename;
	if (!chooseFile("Open Node Editor File", true, filename))
	{
		return;
	}

	using namespace nlohmann;

	json inputJson;

	std::ifstream openFile(filename);

	openFile >> inputJson;

	openFile.close();


	//LOAD NEXT NODE ID
	nextNodeID = inputJson.at("nextNodeID").get<int>();


	//LOAD NODE DATAS

	auto & nodeDatasJson = inputJson.at("nodeDatas");

	for (auto itr = nodeDatasJson.begin(); itr != nodeDatasJson.end(); ++itr)
	{
		const json nodeDataJson = *itr;

		std::shared_ptr<NodeData> deserialisedNodeData = std::make_shared<NodeData>(NodeData::deserialise(nodeDataJson));

		nodeDatas[deserialisedNodeData->id] = deserialisedNodeData;
	}
}

void Program::save()
{
	std::string filename;
	if (!chooseFile("Save Node Editor File", false, filename))
	{
		return;
	}

	using namespace nlohmann;


	json outputJson;


	outputJson["nextNodeID"] = nextNodeID;

	json nodeDatasJson;

	for (const auto & _nodeData : nodeDatas)
	{
		const auto & nodeData = _nodeData.second;

		json nodeSerialised = nodeData->serialise();

		nodeDatasJson.push_back(nodeSerialised);
	}

	outputJson["nodeDatas"] = nodeDatasJson;



	std::ofstream saveFile(filename);

	saveFile << outputJson;

	saveFile.close();

}

void Program::drawMainMenu()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				closeCurrent();
			}
			if (ImGui::MenuItem("Open"))
			{
				closeCurrent();
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

		if (nodeDataSelection != -1 && ImGui::BeginMenu("Node"))
		{
			if (ImGui::MenuItem("Delete \t DEL"))
			{
				deleteSelected();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void Program::drawWindowNodeList()
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ sfmlWindow->getSize().x / 4.f, (float)sfmlWindow->getSize().y });

	ImGui::Begin("Node List", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::PushFont(uiFont);

	ImGui::Text("Click on a node to instantiate it:");
	ImGui::Spacing();
	//ImGui::Button("Example node 1");
	//ImGui::Button("Example node 2");
	////ImGui::TreeNode("Category 1");
	////ImGui::TreePop();
	////ImGui::TreeNode("Category 2");
	////ImGui::TreePop();

	/*if (ImGui::TreeNode("Category 1"))
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
	}*/

	for (auto & nodeTemplate : nodeTemplates)
	{
		if (ImGui::Button(nodeTemplate.second->name.c_str()))
		{
			nodeDatas[nextNodeID] = std::make_shared<NodeData>(nextNodeID, nodeTemplate.second->id, nodePlacementPosition);
			++nextNodeID;
		}
	}

	sf::Vector2f pos = ImGui::GetWindowPos();
	sf::Vector2f size = ImGui::GetWindowSize();
	sf::Vector2f max = (sf::Vector2f)pos + (sf::Vector2f)size;





	ImGui::PopFont();
	ImGui::End();
}

void Program::drawWindowNodeViewer()
{
	ImGui::SetNextWindowPos({ sfmlWindow->getSize().x / 4.f, 0 });
	ImGui::SetNextWindowSize({ 3.f * sfmlWindow->getSize().x / 4.f, (float)sfmlWindow->getSize().y });

	if (ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse))
	{
		drawMainMenu();

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


		//Find node placement posiiton for node list
		nodePlacementPosition = nodeViewerCamera->getTransform().getInverse().transformPoint(windowSize / 2.0f);

		
		//Actually draw
		auto drawList = ImGui::GetWindowDrawList();

		////Draw grid
		//{
		//	sf::Vector2f yStart = nodeViewerCamera->getTransform().transformPoint({ 0, -100000 });
		//	sf::Vector2f yEnd = nodeViewerCamera->getTransform().transformPoint({ 0, 100000 });
		//	drawList->AddLine(yStart, yEnd, ImColor::HSV(0.5f, 1, 1));


		//	sf::Vector2f xStart = nodeViewerCamera->getTransform().transformPoint({ -100000, 0 });
		//	sf::Vector2f xEnd = nodeViewerCamera->getTransform().transformPoint({ 100000, 0 });
		//	drawList->AddLine(xStart, xEnd, ImColor::HSV(0.5f, 1, 1));
		//}

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
				auto jointRegion = nodeTemplate.getInputJointRegion(i, *nodeViewerCamera, nodeData);
				if (jointRegion.contains(_position))
				{
					_node = nodeDataPair.first;
					_property = i;

					return true;
				}
			}
		}
		else
		{
			for (int i = 0; i < nodeTemplate.getOutputsCount(); ++i)
			{
				auto jointRegion = nodeTemplate.getOutputJointRegion(i, *nodeViewerCamera, nodeData);
				if (jointRegion.contains(_position))
				{
					_node = nodeDataPair.first;
					_property = i;

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
	case DragType::NONE:
	{
		int hoverNodeID = -1;
		int hoverPropertyID = -1;
		sf::Vector2f hoverNodeInitialOffset;
		if (doesScreenPositionCollideWithJoint(hoverNodeID, hoverPropertyID, _pos, false))
		{
			showTooltip("New Connection");
		}
		else if (doesScreenPositionCollideWithJoint(hoverNodeID, hoverPropertyID, _pos, true))
		{
			//Edit connection feedback from end of it
			auto itr = nodeDatas[hoverNodeID]->inputConnections.find(hoverPropertyID);
			if (itr != nodeDatas[hoverNodeID]->inputConnections.end())
			{
				showTooltip("Edit Connection");
			}
		}
		else if (doesScreenPositionCollideWithNode(hoverNodeID, hoverNodeInitialOffset, _pos))
		{
			if (nodeDataSelection == hoverNodeID)
			{
				showTooltip("Drag Node");
			}
			else
			{
				showTooltip("Select/Drag Node");
			}
		}
	}
		break;

	case DragType::NODEDATA:
	{
		if (nodeDataDragID != -1)
		{
			nodeDatas[nodeDataDragID]->position = worldPos;
		}
	}
		break;

	case DragType::CONNECTION:
	{
		int newNodeID = -1;
		int newPropertyID = -1;
		if (doesScreenPositionCollideWithJoint(newNodeID, newPropertyID, _pos, true))
		{
			if (isValidConnection(nodeDataDragID, propertyIDDrag, newNodeID, newPropertyID))
			{
				//Show feedback
				showTooltip("Complete Connection");
			}
			else
			{
				showTooltip("Invalid Connection", false);
			}

		}
	}
		break;
	}
}

Program::Program()
{
}


Program::~Program()
{
}

bool Program::isValidConnection(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex)
{
	//Can't connect node to itself
	if (_startNode == _endNode)
	{
		return false;
	}

	//Can't connect different types
	auto startTemplate = nodeTemplates[nodeDatas[_startNode]->nodeTemplateID];
	auto endTemplate = nodeTemplates[nodeDatas[_endNode]->nodeTemplateID];
	if (!connectionMatrix[startTemplate->getOutputProperty(_startPropertyIndex).type][endTemplate->getInputProperty(_endPropertyIndex).type])
	{
		return false;
	}

	return true;
}

bool Program::tryConnectNodes(int _startNode, int _startPropertyIndex, int _endNode, int _endPropertyIndex)
{
	if (!isValidConnection(_startNode, _startPropertyIndex, _endNode, _endPropertyIndex))
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

void Program::disconnectInputGivenOutput(NodeConnection & _outputConnection)
{
	//This is output so disconnect input
	auto endNode = nodeDatas[_outputConnection.endNodeID];
	endNode->inputConnections.erase(_outputConnection.endNodeInputPropertyIndex);
}

void Program::disconnectOutputGivenInput(NodeConnection & _inputConnection)
{
	//This is input so disconnect output
	auto startNode = nodeDatas[_inputConnection.startNodeID];

	//Find the right output connection
	auto & outputConnections = startNode->outputConnections;
	auto itr = std::find_if(outputConnections.begin(), outputConnections.end(), [_inputConnection](std::pair<int, NodeConnection> _connection) {
		return _connection.second.endNodeID == _inputConnection.endNodeID &&
			_connection.second.endNodeInputPropertyIndex == _inputConnection.endNodeInputPropertyIndex;
	});


	outputConnections.erase(itr);
}

void Program::deleteAllConnections(int _nodeID)
{
	auto & nodeData = nodeDatas[_nodeID];

	//Outputs
	for (auto itr = nodeData->outputConnections.begin(); itr != nodeData->outputConnections.end(); ++itr)
	{
		disconnectInputGivenOutput(itr->second);
		//nodeData->outputConnections.erase(itr->first);
	}

	//Inputs
	for (auto itr = nodeData->inputConnections.begin(); itr != nodeData->inputConnections.end(); ++itr)
	{
		disconnectOutputGivenInput(itr->second);
		//nodeData->inputConnections.erase(itr->first);
	}
}

void Program::deleteSelected()
{
	deleteAllConnections(nodeDataSelection);
	nodeDatas.erase(nodeDataSelection);
	nodeDataSelection = -1;
}

void Program::showTooltip(const std::string & _text, bool _good)
{
	ImGui::SetNextWindowPos(sf::Vector2f(ImGui::GetMousePos()) + sf::Vector2f{ 5, 5 });
	if (ImGui::Begin("Tooltip", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::SetWindowSize(sf::Vector2f{ ImGui::CalcTextSize(_text.c_str()) } + sf::Vector2f{ 20, 5 });
		ImGui::TextColored(_good ? ImColor::HSV(0.5f, 1, 1) : ImColor::HSV(0, 1, 1), _text.c_str());
	}
	ImGui::End();
}

bool Program::chooseFile(const std::string & _title, bool _isOpen, std::string & _filename) const
{
	char buffer[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = "Node Editor Files\0*.json\0\0";
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = _title.c_str();
	//ofn.nMaxFileTitle = sizeof(_title.c_str());


	if (_isOpen)
	{
		if (GetOpenFileName(&ofn))
		{
			_filename = buffer;
			return true;
		}
	}
	else
	{
		if (GetSaveFileName(&ofn))
		{
			_filename = buffer;
			return true;
		}
	}

	DWORD err = CommDlgExtendedError();

	return false;
}

void Program::start()
{
	sf::RenderWindow & window = *(sfmlWindow = new sf::RenderWindow(sf::VideoMode(1280, 768), "Node Graph Editor"));
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window, false);

	load();
	

	sf::Clock deltaClock;
	while (window.isOpen()) {
		//SETUP
		ImGui::SFML::Update(window, deltaClock.restart());

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
					deleteSelected();
				}
			}
		}

		mouseUpdate(ImGui::GetMousePos());




		//Draw windows
		//drawMainMenu();
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