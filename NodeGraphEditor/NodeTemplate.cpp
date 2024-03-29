#include "NodeTemplate.h"

#include "NodeData.h"
#include "Program.h"
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

nlohmann::json NodeTemplate::serialise() const
{
	return nlohmann::json();
}

NodeTemplate NodeTemplate::deserialise(const nlohmann::json & _j)
{
	auto & inputsJson = _j.at("inputs");
	auto & outputsJson = _j.at("outputs");
	std::vector<Property> inputs(inputsJson.size());
	std::vector<Property> outputs(outputsJson.size());

	for (int i = 0; i < inputsJson.size(); ++i)
	{
		inputs[i] = Property::deserialise(inputsJson[i]);
	}

	for (int i = 0; i < outputsJson.size(); ++i)
	{
		outputs[i] = Property::deserialise(outputsJson[i]);
	}

	return NodeTemplate{ _j.at("id").get<int>(), _j.at("name"), inputs, outputs};
}

void NodeTemplate::draw(ImDrawList * _drawList, Camera & _camera, const NodeData & _data, bool _isSelection)
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
	if (_isSelection)
	{
		_drawList->AddRect(rectStart, rectEnd, ImColor(style.Colors[ImGuiCol_Text]), 5.0f * scale, 15, 2.0f);
	}
	else
	{
		_drawList->AddRect(rectStart, rectEnd, ImColor(style.Colors[ImGuiCol_Border]), 5.0f * scale);
	}

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
		auto inputJointCircle = getInputJointCircleRegion(i, _camera, _data);
		_drawList->AddCircleFilled(inputJointCircle.first, inputJointCircle.second * 1.1f, ImColor(style.Colors[ImGuiCol_Text]));
		_drawList->AddCircleFilled(inputJointCircle.first, inputJointCircle.second, getConnectionColor(p.type));
	}

	//Outputs
	for (int i = 0; i < outputs.size(); ++i)
	{
		const Property & p = outputs[i];

		auto propertyStart = _camera.getTransform().transformPoint(_data.position + sf::Vector2f{ size.x - p.textUnscaledWidth - PROPERTY_PADDING_FROM_SIDES, _yOffsetBeforeContent + i * (lineHeight + NODE_PADDING) });

		//Property label
		_drawList->AddText(ImGui::GetFont(), getNodeFontSize(_camera), propertyStart, ImColor(style.Colors[ImGuiCol_Text]), p.name.c_str());

		//Property joint
		auto outputJointCircle = getOutputJointCircleRegion(i, _camera, _data);
		_drawList->AddCircleFilled(outputJointCircle.first, outputJointCircle.second * 1.1f, ImColor(style.Colors[ImGuiCol_Text]));
		_drawList->AddCircleFilled(outputJointCircle.first, outputJointCircle.second, getConnectionColor(p.type));
	}
}

void NodeTemplate::drawNodeConnections(ImDrawList * _drawList, Camera & _camera, const NodeData & _data) const
{
	for (auto & connectionPair : _data.outputConnections)
	{
		const NodeConnection & connection = connectionPair.second;


		const NodeData & otherNodeData = Program::get().getNodeData(connection.endNodeID);
		const NodeTemplate & otherNodeTemplate = Program::get().getNodeTemplate(otherNodeData.nodeTemplateID);

		sf::Vector2f startPos = getOutputJointCircleRegion(connection.startNodeOutputPropertyIndex, _camera, _data).first;
		sf::Vector2f endPos = otherNodeTemplate.getInputJointCircleRegion(connection.endNodeInputPropertyIndex, _camera, otherNodeData).first;

		/*auto c1 = startPos + 0.3f * (endPos - startPos);
		c1.y = startPos.y;

		auto c2 = startPos + 0.3f * (endPos - startPos);
		c2.y = endPos.y;

		_drawList->AddBezierCurve(startPos, c1, c2, endPos, IM_COL32_WHITE, 1.f);*/

		drawConnectionRaw(_drawList, _camera, startPos, endPos, outputs[connection.startNodeOutputPropertyIndex].type);
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

std::pair<sf::Vector2f, float> NodeTemplate::getInputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto contentStartLocal = _data.position + sf::Vector2f{ NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
	auto propertyStartLocal = contentStartLocal + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
	auto propertyStart = _camera.getTransform().transformPoint(propertyStartLocal + sf::Vector2f{ 0, lineHeight / 2.f });

	return { propertyStart, NODE_PADDING * _camera.getScale() };
}

std::pair<sf::Vector2f, float> NodeTemplate::getOutputJointCircleRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto contentStartLocalRight = _data.position + sf::Vector2f{ size.x - NODE_PADDING, lineHeight + NODE_PADDING * 3.f };
	auto propertyStartLocalRight = contentStartLocalRight + sf::Vector2f{ 0, propertyIndex * (lineHeight + NODE_PADDING) };
	auto propertyStartRight = _camera.getTransform().transformPoint(propertyStartLocalRight + sf::Vector2f{ 0, lineHeight / 2.f });

	return { propertyStartRight, NODE_PADDING * _camera.getScale() };
}

sf::FloatRect NodeTemplate::getInputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto inputJointCircle = getInputJointCircleRegion(propertyIndex, _camera, _data);
	return sf::FloatRect{ 
		inputJointCircle.first - 1.5f * sf::Vector2f{inputJointCircle.second, inputJointCircle.second},
		3.0f * sf::Vector2f{ inputJointCircle.second, inputJointCircle.second }
	};
}

sf::FloatRect NodeTemplate::getOutputJointRegion(int propertyIndex, const Camera & _camera, const NodeData & _data) const
{
	auto outputJointCircle = getOutputJointCircleRegion(propertyIndex, _camera, _data);
	return sf::FloatRect{
		outputJointCircle.first - 1.5f * sf::Vector2f{ outputJointCircle.second, outputJointCircle.second },
		3.0f * sf::Vector2f{ outputJointCircle.second, outputJointCircle.second }
	};
}

int NodeTemplate::getInputsCount() const
{
	return (int)inputs.size();
}

int NodeTemplate::getOutputsCount() const
{
	return (int)outputs.size();
}

const Property & NodeTemplate::getInputProperty(int _inputPropertyIndex) const
{
	return inputs[_inputPropertyIndex];
}

const Property & NodeTemplate::getOutputProperty(int _outputPropertyIndex) const
{
	return outputs[_outputPropertyIndex];
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