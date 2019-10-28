
#include "stdafx.h"
#include "editmanager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

using namespace framework;
using namespace framework::vprog;

void BuildNode(cNode* node);


cEditManager::cEditManager()
	: m_editor(nullptr)
	, m_contextNodeId(0)
	, m_contextLinkId(0)
	, m_contextPinId(0)
	, m_isCreateNewNode(false)
	, m_newNodeLinkPin(nullptr)
	, m_newLinkPin(nullptr)
	, m_headerBackground(nullptr)
{
}

cEditManager::~cEditManager()
{
	Clear();

	if (m_editor)
	{
		ed::DestroyEditor(m_editor);
		m_editor = nullptr;
	}
}


// Initialize vProg Edit Manager
bool cEditManager::Init(graphic::cRenderer &renderer)
{
	ed::Config config;
	m_editor = ed::CreateEditor(&config);

	m_headerBackground = graphic::cResourceManager::Get()->LoadTexture(renderer
		, "BlueprintBackground.png");

	return true;
}


bool cEditManager::Read(const StrPath &fileName)
{
	Clear();

	ed::SetCurrentEditor(m_editor);

	cNodeFile nodeFile;
	if (!nodeFile.Read(fileName))
		return false;
	m_nodes = nodeFile.m_nodes;
	BuildNodes();
	for (auto &node : m_nodes)
		ed::SetNodePosition(node.m_id, node.m_pos);

	// link id 가 중복될 수 있기 때문에, 새로 생성해야 한다.
	for (auto &link : nodeFile.m_links)
		AddLink(link.fromId, link.toId);

	return true;
}


bool cEditManager::Write(const StrPath &fileName)
{
	cNodeFile nodeFile;
	nodeFile.m_nodes = m_nodes;
	nodeFile.m_links = m_links;
	for (auto &node : nodeFile.m_nodes)
	{
		node.m_pos = ed::GetNodePosition(node.m_id);
		node.m_size = ed::GetNodeSize(node.m_id);
	}
	return nodeFile.Write(fileName);
}


// render vprog node
bool cEditManager::Render(graphic::cRenderer &renderer)
{
	namespace util = ax::NodeEditor::Utilities;

	ed::SetCurrentEditor(m_editor);
	
	ed::Begin("My Editor");
	{
		const int bgWidth = m_headerBackground->m_imageInfo.Width;
		const int bgHeight = m_headerBackground->m_imageInfo.Height;
		util::BlueprintNodeBuilder builder(m_headerBackground->m_texSRV, bgWidth, bgHeight);

		// node rendering
		for (auto& node : m_nodes)
			node.Render(*this, builder, m_newLinkPin);

		// setup link
		for (auto& link : m_links)
			ed::Link(link.id, link.fromId, link.toId, link.color, 2.0f);

		// new/delete node, link operation
		EditOperation();
	} // ~ed::Begin("My Editor");

	// render popup
	RenderPopup(renderer);

	ed::End();

	return true;
}


// new/delete node, link operation
bool cEditManager::EditOperation()
{
	if (m_isCreateNewNode)
		return true;

	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ed::PinId fromPinId = 0, toPinId = 0;
		if (ed::QueryNewLink(&fromPinId, &toPinId))
		{
			auto startPin = FindPin(fromPinId);
			auto endPin = FindPin(toPinId);

			m_newLinkPin = startPin ? startPin : endPin;

			if (startPin->kind == PinKind::Input)
			{
				std::swap(startPin, endPin);
				std::swap(fromPinId, toPinId);
			}

			if (startPin && endPin)
			{
				if (endPin == startPin)
				{
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (endPin->kind == startPin->kind)
				{
					showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				//else if (endPin->Node == startPin->Node)
				//{
				//    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
				//    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				//}
				else if (endPin->type != startPin->type)
				{
					showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else
				{
					showLabel("+ Create Link", ImColor(32, 45, 32, 180));
					if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						m_links.emplace_back(vprog::sLink(GetUniqueId(), fromPinId, toPinId));
						m_links.back().color = GetIconColor(startPin->type);
					}
				}
			}
		}

		ed::PinId pinId = 0;
		if (ed::QueryNewNode(&pinId))
		{
			m_newLinkPin = FindPin(pinId);
			if (m_newLinkPin)
				showLabel("+ Create Node", ImColor(32, 45, 32, 180));

			if (ed::AcceptNewItem())
			{
				m_isCreateNewNode = true;
				m_newNodeLinkPin = FindPin(pinId);
				m_newLinkPin = nullptr;
				ed::Suspend();
				ImGui::OpenPopup("Create New Node");
				ed::Resume();
			}
		}
	}
	else
	{
		m_newLinkPin = nullptr;
	}

	ed::EndCreate();


	if (ed::BeginDelete())
	{
		ed::LinkId linkId = 0;
		while (ed::QueryDeletedLink(&linkId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto id = std::find_if(m_links.begin()
					, m_links.end()
					, [linkId](auto& link) { return link.id == linkId; });
				if (id != m_links.end())
					m_links.erase(id);
			}
		}

		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto id = std::find_if(m_nodes.begin()
					, m_nodes.end(
					), [nodeId](auto& node) { return node.m_id == nodeId; });
				if (id != m_nodes.end())
					m_nodes.erase(id);
			}
		}
	}
	ed::EndDelete();

	return true;
}


bool cEditManager::RenderPopup(graphic::cRenderer &renderer)
{
	auto openPopupPosition = ImGui::GetMousePos();


	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("Create New Node"))
	{
		auto newNodePostion = openPopupPosition;
		//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

		//auto drawList = ImGui::GetWindowDrawList();
		//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

		vprog::cNode* node = nullptr;
		if (ImGui::MenuItem("Input Action"))
			node = Generate_InputActionNode();
		if (ImGui::MenuItem("Output Action"))
			node = Generate_OutputActionNode();
		if (ImGui::MenuItem("Branch"))
			node = Generate_BranchNode();
		if (ImGui::MenuItem("Do N"))
			node = Generate_DoNNode();
		if (ImGui::MenuItem("Set Timer"))
			node = Generate_SetTimerNode();
		if (ImGui::MenuItem("Less"))
			node = Generate_LessNode();
		if (ImGui::MenuItem("Weird"))
			node = Generate_WeirdNode();
		if (ImGui::MenuItem("Trace by Channel"))
			node = Generate_TraceByChannelNode();
		if (ImGui::MenuItem("Print String"))
			node = Generate_PrintStringNode();
		ImGui::Separator();
		if (ImGui::MenuItem("Comment"))
			node = Generate_Comment();
		ImGui::Separator();
		if (ImGui::MenuItem("Sequence"))
			node = Generate_TreeSequenceNode();
		if (ImGui::MenuItem("Move To"))
			node = Generate_TreeTaskNode();
		if (ImGui::MenuItem("Random Wait"))
			node = Generate_TreeTask2Node();
		ImGui::Separator();
		if (ImGui::MenuItem("Message"))
			node = Generate_MessageNode();

		if (node)
		{
			m_isCreateNewNode = false;

			ed::SetNodePosition(node->m_id, newNodePostion);

			if (auto startPin = m_newNodeLinkPin)
			{
				auto& pins = startPin->kind == vprog::PinKind::Input ?
					node->m_outputs : node->m_inputs;

				for (auto& pin : pins)
				{
					if (CanCreateLink(startPin, &pin))
					{
						auto endPin = &pin;
						if (startPin->kind == PinKind::Input)
							std::swap(startPin, endPin);

						m_links.emplace_back(vprog::sLink(GetUniqueId(), startPin->id, endPin->id));
						m_links.back().color = GetIconColor(startPin->type);

						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	}
	else
	{
		m_isCreateNewNode = false;
	}

	ImGui::PopStyleVar();
	ed::Resume();

	return true;
}


cNode* cEditManager::FindNode(const ed::NodeId id)
{
	for (auto& node : m_nodes)
		if (node.m_id == id)
			return &node;

	return nullptr;
}


sLink* cEditManager::FindLink(const ed::LinkId id)
{
	for (auto& link : m_links)
		if (link.id == id)
			return &link;

	return nullptr;
}


sPin* cEditManager::FindPin(const ed::PinId id)
{
	if (!id)
		return nullptr;

	for (auto& node : m_nodes)
	{
		for (auto& pin : node.m_inputs)
			if (pin.id == id)
				return &pin;

		for (auto& pin : node.m_outputs)
			if (pin.id == id)
				return &pin;
	}

	return nullptr;
}


bool cEditManager::IsPinLinked(const ed::PinId id)
{
	if (!id)
		return false;

	for (auto& link : m_links)
		if (link.fromId == id || link.toId == id)
			return true;

	return false;
}


void BuildNode(cNode* node)
{
	for (auto& input : node->m_inputs)
	{
		input.node = node;
		input.kind = PinKind::Input;
	}

	for (auto& output : node->m_outputs)
	{
		output.node = node;
		output.kind = PinKind::Output;
	}
}


void cEditManager::BuildNodes()
{
	for (auto& node : m_nodes)
		BuildNode(&node);
}


cNode* cEditManager::Generate_InputActionNode()
{
	m_nodes.emplace_back(GetUniqueId(), "InputAction Fire", ImColor(255, 128, 128));
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Delegate);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Pressed", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Released", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_BranchNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Branch");
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Condition", PinType::Bool);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "True", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "False", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_DoNNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Do N");
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Enter", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "N", PinType::Int);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Reset", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Exit", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Counter", PinType::Int);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_OutputActionNode()
{
	m_nodes.emplace_back(GetUniqueId(), "OutputAction");
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Sample", PinType::Float);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Condition", PinType::Bool);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Event", PinType::Delegate);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_PrintStringNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Print String");
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "In String", PinType::String);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_MessageNode()
{
	m_nodes.emplace_back(GetUniqueId(), "", ImColor(128, 195, 248));
	m_nodes.back().m_type = NodeType::Variable;
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Message", PinType::String);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_SetTimerNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Set Timer", ImColor(128, 195, 248));
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Object", PinType::Object);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Function Name", PinType::Function);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Time", PinType::Float);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Looping", PinType::Bool);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_LessNode()
{
	m_nodes.emplace_back(GetUniqueId(), "<", ImColor(128, 195, 248));
	m_nodes.back().m_type = NodeType::Operator;
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Float);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Float);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Float);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_WeirdNode()
{
	m_nodes.emplace_back(GetUniqueId(), "o.O", ImColor(128, 195, 248));
	m_nodes.back().m_type = NodeType::Operator;
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Float);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Float);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Float);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_TraceByChannelNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Start", PinType::Flow);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "End", PinType::Int);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Trace Channel", PinType::Float);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Trace Complex", PinType::Bool);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Actors to Ignore", PinType::Int);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Draw Debug Type", PinType::Bool);
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "Ignore Self", PinType::Bool);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Out Hit", PinType::Float);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "Return Value", PinType::Bool);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_TreeSequenceNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Sequence");
	m_nodes.back().m_type = NodeType::Tree;
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);
	m_nodes.back().m_outputs.emplace_back(GetUniqueId(), "", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_TreeTaskNode()
{
	m_nodes.emplace_back(GetUniqueId(), "Move To");
	m_nodes.back().m_type = NodeType::Tree;
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_TreeTask2Node()
{
	m_nodes.emplace_back(GetUniqueId(), "Random Wait");
	m_nodes.back().m_type = NodeType::Tree;
	m_nodes.back().m_inputs.emplace_back(GetUniqueId(), "", PinType::Flow);

	BuildNode(&m_nodes.back());

	return &m_nodes.back();
}


cNode* cEditManager::Generate_Comment()
{
	m_nodes.emplace_back(GetUniqueId(), "Test Comment");
	m_nodes.back().m_type = NodeType::Comment;
	m_nodes.back().m_size = ImVec2(300, 200);

	return &m_nodes.back();
}


// generate reserved definition node
// from ReadDefinitionFile()
cNode* cEditManager::Generate_ReservedDefinition(const StrId &name)
{
	cNode *defNode = NULL;
	for (auto &def : m_definitions)
	{
		if (def.m_name == name)
		{
			defNode = &def;
			break;
		}
	}
	if (!defNode)
		return NULL;

	cNode newNode = *defNode;
	newNode.m_id = GetUniqueId();
	for (auto &pin : newNode.m_inputs)
		pin.id = GetUniqueId();
	for (auto &pin : newNode.m_outputs)
		pin.id = GetUniqueId();

	newNode.m_pos = ImVec2(0, 0);
	m_nodes.push_back(newNode);

	BuildNode(&m_nodes.back());
	return &m_nodes.back();
}


bool cEditManager::AddLink(const ed::PinId from, const ed::PinId to)
{
	sPin *frPin = FindPin(from);
	sPin *toPin = FindPin(to);
	if (!frPin || !toPin)
		return false;

	m_links.push_back(sLink(ed::LinkId(GetUniqueId()), from, to));
	m_links.back().color = GetIconColor(frPin->type);
	return true;
}


// read function definition file
// format: vProg node file
// add to reserved node data
bool cEditManager::ReadDefinitionFile(const StrPath &fileName)
{
	cNodeFile nodeFile;
	if (!nodeFile.Read(fileName))
		return false;
	m_definitions = nodeFile.m_nodes;
	return true;
}


// 중복되지 않은 id를 리턴한다.
// node, pin, link와 비교해서 같은 id가 없을 때만 리턴한다.
int cEditManager::GetUniqueId()
{
	while (1)
	{
		const int id = common::GenerateId();
		const ed::NodeId nid = id;
		const ed::PinId pid = id;
		const ed::LinkId lid = id;
		
		// 중복 검사
		for (auto &node : m_nodes)
		{
			if (node.m_id == nid)
				continue;
			for (auto &pin : node.m_inputs)
				if (pin.id == pid)
					continue;
			for (auto &pin : node.m_outputs)
				if (pin.id == pid)
					continue;
		}
		for (auto &link : m_links)
			if (link.id == lid)
				continue;
		return id;
	}	
	return 0;
}


void cEditManager::Clear()
{
	ed::SetCurrentEditor(m_editor);
	ed::ClearEditor();
	m_nodes.clear();
	m_links.clear();
}
