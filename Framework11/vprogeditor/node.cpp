
#include "stdafx.h"
#include "node.h"
#include "editmanager.h"

using namespace framework;
using namespace vprog;


cNode::cNode(int id, const StrId &name
	, ImColor color //= ImColor(255, 255, 255)
)
	: m_id(id)
	, m_name(name)
	, m_varName(name)
	, m_color(color)
	, m_type(eNodeType::Function)
	, m_size(0, 0)
{
}

cNode::~cNode()
{
	Clear();
}


// render visual programming node
bool cNode::Render(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin* newLinkPin //= nullptr
)
{
	cNode &node = *this;

	if (node.m_type != eNodeType::Function 
		&& node.m_type != eNodeType::Operator
		&& node.m_type != eNodeType::Event
		&& node.m_type != eNodeType::Control
		&& node.m_type != eNodeType::Macro
		&& node.m_type != eNodeType::Variable)
		return true;

	const auto isSimple = (node.m_type == eNodeType::Variable) 
		|| (node.m_type == eNodeType::Operator);

	bool hasOutputDelegates = false;
	for (auto& output : node.m_outputs)
	{
		if (output.type == ePinType::Delegate)
			hasOutputDelegates = true;
	}

	builder.Begin(node.m_id);
	if (!isSimple)
	{
		builder.Header(node.m_color);
		ImGui::Spring(0);
		ImGui::TextUnformatted(node.m_name.c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		if (hasOutputDelegates)
		{
			ImGui::BeginVertical("delegates", ImVec2(0, 28));
			ImGui::Spring(1, 0);
			for (auto& output : node.m_outputs)
			{
				if (output.type != ePinType::Delegate)
					continue;

				auto alpha = ImGui::GetStyle().Alpha;
				if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
					alpha = alpha * (48.0f / 255.0f);

				ed::BeginPin(output.id, ed::ePinKind::Output);
				ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
				ed::PinPivotSize(ImVec2(0, 0));
				ImGui::BeginHorizontal(output.id.AsPointer());
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
				if (!output.name.empty())
				{
					ImGui::TextUnformatted(output.name.c_str());
					ImGui::Spring(0);
				}
				DrawPinIcon(output, editMgr.IsPinLinked(output.id), (int)(alpha * 255));
				ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
				ImGui::EndHorizontal();
				ImGui::PopStyleVar();
				ed::EndPin();

				//DrawItemRect(ImColor(255, 0, 0));
			}
			ImGui::Spring(1, 0);
			ImGui::EndVertical();
			ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
		}
		else
			ImGui::Spring(0);
		builder.EndHeader();
	}

	for (auto& input : node.m_inputs)
	{
		auto alpha = ImGui::GetStyle().Alpha;
		if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
			alpha = alpha * (48.0f / 255.0f);

		builder.Input(input.id);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		DrawPinIcon(input, editMgr.IsPinLinked(input.id), (int)(alpha * 255));
		ImGui::Spring(0);
		if (!input.name.empty())
		{
			ImGui::TextUnformatted(input.name.c_str());
			ImGui::Spring(0);
		}
		if (input.type == ePinType::Bool)
		{
			ImGui::Button("Hello");
			ImGui::Spring(0);
		}
		ImGui::PopStyleVar();
		builder.EndInput();
	} //~input

	if (isSimple)
	{
		builder.Middle();

		ImGui::Spring(1, 0);
		ImGui::TextUnformatted(node.m_varName.c_str());
		ImGui::Spring(1, 0);
	}

	for (auto& output : node.m_outputs)
	{
		if (!isSimple && output.type == ePinType::Delegate)
			continue;

		auto alpha = ImGui::GetStyle().Alpha;
		if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
			alpha = alpha * (48.0f / 255.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		builder.Output(output.id);
		if ((eNodeType::Variable == m_type) 
			&& (output.type == ePinType::String))
		{
			//static char buffer[128] = "Edit Me\nMultiline!";
			cSymbolTable::sValue *value = editMgr.m_symbTable.FindSymbol(output.id);
			if (value)
			{
				Str128 buffer = value->str;
				static bool wasActive = false;

				ImGui::PushItemWidth(100.0f);
				if (ImGui::InputText("##edit", buffer.m_str, buffer.SIZE))
					value->str = buffer.c_str();
				ImGui::PopItemWidth();
				if (ImGui::IsItemActive() && !wasActive)
				{
					ed::EnableShortcuts(false);
					wasActive = true;
				}
				else if (!ImGui::IsItemActive() && wasActive)
				{
					ed::EnableShortcuts(true);
					wasActive = false;
				}
			}
			ImGui::Spring(0);
		}
		if (!output.name.empty())
		{
			ImGui::Spring(0);
			ImGui::TextUnformatted(output.name.c_str());
		}
		ImGui::Spring(0);
		DrawPinIcon(output, editMgr.IsPinLinked(output.id), (int)(alpha * 255));
		ImGui::PopStyleVar();
		builder.EndOutput();
	} //~output

	builder.End();

	return true;
}


void cNode::Clear()
{
	m_id = 0;
	m_name.clear();
	m_inputs.clear();
	m_outputs.clear();
}
