#include "NodeTemplate.h"

#include "NodeData.h"
#include "NodeEditor.h"
#include "Util.h"

#define START_NODE_FONT_SCALE 18.f / 64.f
#define NODE_PADDING 5.f
#define INPUTS_FROM_OUTPUTS_PADDING NODE_PADDING * 3.f
#define PROPERTY_PADDING_FROM_SIDES NODE_PADDING * 3.f

NodeTemplate::NodeTemplate(int _id, const std::string & _name, const std::vector<Property> & _inputs, const std::vector<Property> & _outputs) : 
	id(_id),
	name(_name), 
	inputs(_inputs), 
	outputs(_outputs)
{
}

NodeTemplate::~NodeTemplate()
{
}

void NodeTemplate::draw(ImDrawList * _drawList, Camera & _camera, const NodeData & _data)
{
	recalculateSize();

	ImGuiStyle & style = ImGui::GetStyle();

	auto rectStart = _camera.getTransform().transformPoint(_data.position);
	auto rectEnd = _camera.getTransform().transformPoint(_data.position + size);
	auto titleEnd = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ size.x, lineHeight + NODE_PADDING * 2.f });

	float yOffsetBeforeContent = lineHeight + NODE_PADDING * 3.f;

	auto textStart = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ NODE_PADDING, NODE_PADDING });

	float scale = _camera.getScale();

	//Main BG
	_drawList->AddRectFilled(rectStart, rectEnd, ImColor(style.Colors[ImGuiCol_TitleBg]), 5.0f * scale);

	//Title BG
	_drawList->AddRectFilled(rectStart, titleEnd, ImColor(style.Colors[ImGuiCol_TitleBgActive]), 5.0f * scale);

	//Main Border
	_drawList->AddRect(rectStart, rectEnd, ImColor(style.Colors[ImGuiCol_Border]), 5.0f * scale);

	//Title Border
	_drawList->AddRect(rectStart, titleEnd, ImColor(style.Colors[ImGuiCol_Border]), 5.0f * scale);

	//Title Text
	_drawList->AddText(ImGui::GetFont(), getNodeFontSize(_camera), textStart, ImColor(style.Colors[ImGuiCol_Text]), name.c_str());


	drawProperties(_drawList, _camera, _data, yOffsetBeforeContent);
	drawNodeConnections(_drawList, _camera, _data);
	
}

void NodeTemplate::drawProperties(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, float _yOffsetBeforeContent) const
{
	ImGuiStyle & style = ImGui::GetStyle();
	float scale = _camera.getScale();

	//Draw properties
	//Inputs
	for (int i = 0; i < inputs.size(); ++i)
	{
		const Property & p = inputs[i];

		auto propertyStart = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ PROPERTY_PADDING_FROM_SIDES, _yOffsetBeforeContent + i * (lineHeight + NODE_PADDING) });

		//Property label
		_drawList->AddText(ImGui::GetFont(), getNodeFontSize(_camera), propertyStart, ImColor(style.Colors[ImGuiCol_Text]), p.name.c_str());

		//Property joint
		_drawList->AddCircleFilled(getInputJointRegion(i, _camera, _data).first, NODE_PADDING * scale, ImColor(style.Colors[ImGuiCol_Text]));
	}

	//Outputs
	for (int i = 0; i < outputs.size(); ++i)
	{
		const Property & p = outputs[i];

		auto propertyStart = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ size.x - p.textUnscaledWidth - PROPERTY_PADDING_FROM_SIDES, _yOffsetBeforeContent + i * (lineHeight + NODE_PADDING) });

		//Property label
		_drawList->AddText(ImGui::GetFont(), getNodeFontSize(_camera), propertyStart, ImColor(style.Colors[ImGuiCol_Text]), p.name.c_str());

		//Property joint
		_drawList->AddCircleFilled(getOutputJointRegion(i, _camera, _data).first, NODE_PADDING * scale, ImColor(style.Colors[ImGuiCol_Text]));
	}
}

void NodeTemplate::drawNodeConnections(ImDrawList * _drawList, Camera & _camera, const NodeData & _data) const
{
	for (const NodeConnection & connection : _data.nodeConnections)
	{
		const NodeData & otherNodeData = NodeEditor::get().getNodeData(connection.otherNodeID);
		const NodeTemplate & otherNodeTemplate = NodeEditor::get().getNodeTemplate(otherNodeData.nodeTemplateID);

		sf::Vector2f startPos = getOutputJointRegion(connection.thisNodeOutputPropertyIndex, _camera, _data).first;
		sf::Vector2f endPos = otherNodeTemplate.getInputJointRegion(connection.otherNodeInputPropertyIndex, _camera, otherNodeData).first;

		/*auto c1 = startPos + 0.3f * (endPos - startPos);
		c1.y = startPos.y;

		auto c2 = startPos + 0.3f * (endPos - startPos);
		c2.y = endPos.y;

		_drawList->AddBezierCurve(startPos, c1, c2, endPos, IM_COL32_WHITE, 1.f);*/

		drawConnectionRaw(_drawList, _camera, startPos, endPos, 0);
	}
}

void NodeTemplate::recalculateSize()
{
	auto titleBounds = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * START_NODE_FONT_SCALE, FLT_MAX, 0.0f, name.c_str());
	lineHeight = titleBounds.y;


	//Calculate size of properties
	//Inputs
	float largestWidthInputs = 0;
	for (Property & p : inputs)
	{
		auto thisPropertyBounds = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * START_NODE_FONT_SCALE, FLT_MAX, 0.0f, p.name.c_str());
		if (thisPropertyBounds.x > largestWidthInputs)
		{
			largestWidthInputs = thisPropertyBounds.x;
		}

		p.textUnscaledWidth = thisPropertyBounds.x;
	}

	//Outputs
	float largestWidthOutputs = 0;
	for (Property & p : outputs)
	{
		auto thisPropertyBounds = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * START_NODE_FONT_SCALE, FLT_MAX, 0.0f, p.name.c_str());
		if (thisPropertyBounds.x > largestWidthOutputs)
		{
			largestWidthOutputs = thisPropertyBounds.x;
		}

		p.textUnscaledWidth = thisPropertyBounds.x;
	}


	//Choose largest width out of (title or inputs + outputs + padding)
	size.x = fmaxf(titleBounds.x + NODE_PADDING * 2.f, largestWidthInputs + largestWidthOutputs + PROPERTY_PADDING_FROM_SIDES * 2.f + INPUTS_FROM_OUTPUTS_PADDING);

	//		Title         TOP+BOTTOM		   BELOW TITLE TEXT            PER PROPERTY
	size.y = lineHeight + NODE_PADDING * 2.f + NODE_PADDING		+ (inputs.size() > outputs.size() ? inputs.size() : outputs.size()) * (lineHeight + NODE_PADDING);
}

sf::FloatRect NodeTemplate::getHeaderRegion(const Camera & _camera, const NodeData & _data) const
{
	auto rectStart = _camera.getTransform().transformPoint(_data.position);
	auto titleEnd = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ size.x, lineHeight + NODE_PADDING * 2.f });
	return sf::FloatRect{ rectStart, titleEnd - rectStart };
}

std::pair<sf::Vector2f, float> NodeTemplate::getInputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto contentStartLocal = _data.position + sf::Vector2f{ NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
	auto propertyStartLocal = contentStartLocal + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
	auto propertyStart = _camera.getTransform().transformPoint(propertyStartLocal + sf::Vector2f{ 0, lineHeight / 2.f });

	return { propertyStart, NODE_PADDING * _camera.getScale() };
}

std::pair<sf::Vector2f, float> NodeTemplate::getOutputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto contentStartLocalRight = _data.position + sf::Vector2f{ size.x - NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
	auto propertyStartLocalRight = contentStartLocalRight + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
	auto propertyStartRight = _camera.getTransform().transformPoint(propertyStartLocalRight + sf::Vector2f{ 0, lineHeight / 2.f });

	return { propertyStartRight, NODE_PADDING * _camera.getScale() };
}

int NodeTemplate::getInputsCount() const
{
	return inputs.size();
}

int NodeTemplate::getOutputsCount() const
{
	return outputs.size();
}

float NodeTemplate::getNodeFontSize(Camera & _camera)
{
	return ImGui::GetFontSize() * _camera.getScale() * START_NODE_FONT_SCALE;
}

//sf::Vector2f NodeTemplate::getInputJointPos(int propertyIndex, Camera & _camera, const NodeData & _data) const
//{
//	auto contentStartLocal = _data.position + sf::Vector2f{ NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
//	auto propertyStartLocal = contentStartLocal + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
//	auto propertyStart = _camera.getTransform().transformPoint(propertyStartLocal + sf::Vector2f{ 0, lineHeight / 2.f });
//
//	return propertyStart;
//}
//
//sf::Vector2f NodeTemplate::getOutputJointPos(int propertyIndex, Camera & _camera, const NodeData & _data) const
//{
//	auto contentStartLocalRight = _data.position + sf::Vector2f{ size.x - NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
//	auto propertyStartLocalRight = contentStartLocalRight + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
//	auto propertyStartRight = _camera.getTransform().transformPoint(propertyStartLocalRight + sf::Vector2f{ 0, lineHeight / 2.f });
//
//	return propertyStartRight;
//}