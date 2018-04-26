#include "NodeEditor.h"

#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\System\Clock.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Graphics\CircleShape.hpp>

//TODO: REMOVE:
#include <iostream>

#include "NodeTemplate.h"
#include "NodeData.h"
#include "Camera.h"

NodeEditor * NodeEditor::instance = nullptr;

NodeEditor & NodeEditor::get()
{
	return *instance;
}

void NodeEditor::create()
{
	instance = new NodeEditor();
}

void NodeEditor::loadFonts()
{
	auto io = ImGui::GetIO();

	defaultFont = io.Fonts->AddFontDefault();
	uiFont = io.Fonts->AddFontFromFileTTF("inconsolata\\Inconsolata-Regular.ttf", 13);
	nodeHighResFont = io.Fonts->AddFontFromFileTTF("inconsolata\\Inconsolata-Regular.ttf", 64);

	ImGui::SFML::UpdateFontTexture();
}

void NodeEditor::load()
{
	loadFonts();


	nodeViewerCamera = new Camera();


	nodeTemplates[5] = std::make_shared<NodeTemplate>(5,
		"Crazy Operation", 
		std::vector<Property>{
			Property("Input 1"),
			Property("Input Thing 2")
		}, 
		std::vector<Property>{
			Property("Ouput 1"),
			Property("Output Thing 2"),
			Property("An Output 3")
		});
	nodeTemplates[9] = std::make_shared<NodeTemplate>(9,
		"Add",
		std::vector<Property>{
			Property("Input 1"),
			Property("Input 2")
		},
		std::vector<Property>{
			Property("Ouput 1")
		});


	nodeDatas[3] = std::make_shared<NodeData>(3, 5, sf::Vector2f{ 100, 100 });
	nodeDatas[4] = std::make_shared<NodeData>(4, 5, sf::Vector2f{ 500, 100 });
	nodeDatas[5] = std::make_shared<NodeData>(5, 9, sf::Vector2f{ 100, 500 });
	nodeDatas[6] = std::make_shared<NodeData>(6, 9, sf::Vector2f{ 500, 500 });



	nodeDatas[3]->nodeConnections.push_back(NodeConnection{ 3, 1, 6, 0 });
	nodeDatas[5]->nodeConnections.push_back(NodeConnection{ 5, 0, 6, 1 });

}

void NodeEditor::drawWindowNodeList()
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

void NodeEditor::drawWindowNodeViewer()
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

				nodeTemplate.draw(drawList, *nodeViewerCamera, nodeData);
			}
		}

		ImGui::PopFont();
	}
	ImGui::End();
}

NodeEditor::NodeEditor()
{
}


NodeEditor::~NodeEditor()
{
}

void NodeEditor::start()
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

		}


		//SETUP
		ImGui::SFML::Update(window, deltaClock.restart());


		//Draw windows
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

const NodeData & NodeEditor::getNodeData(int _id) const
{
	const NodeData & nodeData = *nodeDatas.at(_id);
	return nodeData;
}

const NodeTemplate & NodeEditor::getNodeTemplate(int _id) const
{
	const NodeTemplate & nodeTemplate = *nodeTemplates.at(_id);
	return nodeTemplate;
}