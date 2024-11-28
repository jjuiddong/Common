
#include "stdafx.h"
#include "node.h"
#include "editmanager.h"
#include "../external/NodeEditor/imgui_node_editor_internal.h"

using namespace framework;
using namespace vpl;


cNode::cNode(int id, const StrId &name
	, ImColor color //= ImColor(255, 255, 255)
)
	: m_id(id)
	, m_name(name)
	, m_desc(name)
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
	if (m_type != eNodeType::Function
		&& m_type != eNodeType::Operator
		&& m_type != eNodeType::Event
		&& m_type != eNodeType::Control
		&& m_type != eNodeType::Macro
		&& m_type != eNodeType::Variable)
		return true;

	const auto isSimple = (m_type == eNodeType::Variable) 
		|| (m_type == eNodeType::Operator);

	bool hasOutputDelegates = false;
	for (auto& output : m_outputs)
	{
		if (output.type == ePinType::Delegate)
			hasOutputDelegates = true;
	}

	builder.Begin(m_id);
	if (!isSimple)
	{
		builder.Header(m_color);
		ImGui::Spring(0);
		ImGui::TextUnformatted(m_name.c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		if (hasOutputDelegates)
		{
			ImGui::BeginVertical("delegates", ImVec2(0, 28));
			ImGui::Spring(1, 0);
			for (auto& output : m_outputs)
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

	for (auto& input : m_inputs)
	{
		auto alpha = ImGui::GetStyle().Alpha;
		if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
			alpha = alpha * (48.0f / 255.0f);

		builder.Input(input.id);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		DrawPinIcon(input, editMgr.IsPinLinked(input.id), (int)(alpha * 255));
		ImGui::Spring(0);

		switch (input.type)
		{
		case ePinType::Any:
			// set variable node?
			if (m_name == "Set")
			{
				RenderSetInputPin(editMgr, builder, input, newLinkPin);
			}
			else
			{
				if (!input.name.empty())
				{
					ImGui::TextUnformatted(input.name.c_str());
					ImGui::Spring(0);
				}
			}
			break;

		//case ePinType::Bool:
			// todo: check or combo
			//ImGui::Spring(0);
			//break;
		default:
			if (!input.name.empty())
			{
				ImGui::TextUnformatted(input.name.c_str());
				ImGui::Spring(0);
			}
			break;
		}

		ImGui::PopStyleVar();
		builder.EndInput();
	} //~input

	if (isSimple)
	{
		if (eNodeType::Operator == m_type)
		{
			builder.Middle();
			ImGui::Spring(1, 0);
			ImGui::TextUnformatted(m_name.c_str());
			ImGui::Spring(1, 0);
		}
		else
		{
			builder.Middle();
			ImGui::Spring(1, 0);
			ImGui::TextUnformatted("    ");
			ImGui::Spring(1, 0);
		}
	}

	for (auto& output : m_outputs)
	{
		if (!isSimple && output.type == ePinType::Delegate)
			continue;

		auto alpha = ImGui::GetStyle().Alpha;
		if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
			alpha = alpha * (48.0f / 255.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
		builder.Output(output.id);
		if (eNodeType::Variable == m_type)
		{
			switch (output.type)
			{
			case ePinType::String: 
				RenderStringPin(editMgr, builder, output, newLinkPin); 
				break;			
			case ePinType::Enums: 
				RenderEnumPin(editMgr, builder, output, newLinkPin); 
				break;
			}
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

	if ((eNodeType::Control == m_type) && (m_desc == "Switch"))
		RenderSwitchCaseNode(editMgr, builder, newLinkPin);
	else if ((eNodeType::Control == m_type) && (m_desc == "Sequence"))
		RenderSequenceNode(editMgr, builder, newLinkPin);

	builder.End();

	return true;
}


// render Set node input pin
// render input variable combo box
bool cNode::RenderSetInputPin(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin &pin
	, sPin* newLinkPin //= nullptr
)
{
	char *prevStr = "Empty";

	// enum selection combo
	ImGui::PushItemWidth(100);
	const ImVec2 offset = ed::GetCurrentViewTransformPosition();
	const float scale = 1.f / ed::GetCurrentZoom();
	if (ImGui::BeginCombo2("##enum combo", prevStr, scale, offset))
	{
		ed::Suspend();
		for (auto &kv : editMgr.m_symbTable.m_vars)
		{
			const string &scopeName = kv.first;
			for (auto &kv2 : kv.second)
			{
				const string &name = kv2.first;
				sPin *varPin = editMgr.FindPin(scopeName, name);
				if (!varPin)
					continue;

				if (ImGui::Selectable(varPin->name.c_str()))
				{
					// update pin information
					pin.name = varPin->name;
					pin.type = varPin->type;
					pin.typeStr = varPin->typeStr;
					
					cNode *node = editMgr.FindContainNode(pin.id);
					node->m_desc = scopeName;
				}
			}
		}

		ed::Resume();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	//~combo

	ImGui::Spring(0);
	return true;
}


// render string pin
bool cNode::RenderStringPin(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin &pin
	, sPin* newLinkPin //= nullptr
)
{
	const string scopeName =
		common::script::cSymbolTable::MakeScopeName(
			m_name.c_str(), (int)m_id.Get());

	common::script::sVariable *varInfo =
		editMgr.m_symbTable.FindVarInfo(scopeName, pin.name.c_str());
	if (!varInfo)
		return true;

	Str128 buffer = common::variant2str(varInfo->var);
	static bool wasActive = false;

	ImGui::PushItemWidth(100.0f);
	if (ImGui::InputText("##edit", buffer.m_str, buffer.SIZE))
	{
		common::clearvariant(varInfo->var);
		varInfo->var = common::str2variant(VT_BSTR, buffer.c_str());
	}
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
	ImGui::Spring(0);

	return true;
}


// render enumeration pin
bool cNode::RenderEnumPin(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin &pin
	, sPin* newLinkPin //= nullptr
)
{
	const string scopeName =
		common::script::cSymbolTable::MakeScopeName(
			m_name.c_str(), (int)m_id.Get());
	common::script::sVariable *varInfo =
		editMgr.m_symbTable.FindVarInfo(scopeName, pin.name.c_str());
	if (!varInfo)
		return true;

	auto *type = editMgr.m_symbTable.FindSymbol(m_name.c_str());
	if (!type)
		return true;

	auto &var = varInfo->var;
	const char *prevStr = nullptr;
	if (type->enums.size() > (uint)var.intVal)
		prevStr = type->enums[var.intVal].name.c_str();
	else
		prevStr = type->enums[0].name.c_str();

	// enum selection combo
	ImGui::PushItemWidth(200);
	const ImVec2 offset = ed::GetCurrentViewTransformPosition();
	const float scale = 1.f / ed::GetCurrentZoom();
	if (ImGui::BeginCombo2("##enum combo", prevStr, scale, offset))
	{
		ed::Suspend();
		for (uint i = 0; i < type->enums.size(); ++i)
		{
			auto &e = type->enums[i];
			if (ImGui::Selectable(e.name.c_str()))
				var.intVal = (int)i;
		}
		ed::Resume();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	//~combo

	ImGui::Spring(0);

	return true;
}


// render switch case, default type
bool cNode::RenderSwitchCaseNode(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin* newLinkPin //= nullptr
)
{
	// check int selection type?
	ePinType pinType = ePinType::COUNT;
	for (uint i = 0; i < m_inputs.size(); ++i)
	{
		auto &pin = m_inputs[i];
		if (pin.name == "Selection")
		{
			pinType = pin.type;
			break;
		}
	}

	// int type switch case
	// render + Add pin Button
	if (ePinType::Int == pinType)
	{
		static int val = 0;

		ImGui::Spring(0);
		ImGui::PushItemWidth(35);
		ImGui::InputInt("##val", &val, 0);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Add Pin+"))
		{
			// check same value?
			auto it = find_if(m_outputs.begin(), m_outputs.end()
				, [&](const auto &a) { return a.value == val; });
			if (it == m_outputs.end())
			{
				// insert case, before Default case
				StrId text;
				text.Format("%d", val);
				vpl::sPin pin(editMgr.GetUniqueId(), text.c_str(), ePinType::Flow);
				pin.typeStr = "Flow";
				pin.nodeId = m_id;
				pin.kind = ePinKind::Output;
				pin.value = val;
				m_outputs.push_back(pin);

				// find default iterator?
				auto it2 = find_if(m_outputs.begin(), m_outputs.end()
					, [](const auto &a) { return a.name == "Default"; });
				if (m_outputs.end() != it)
				{
					std::swap(*it2, m_outputs.back());
				}
			}
		}
		//ImGui::Spring(0);
	}

	return true;
}


// render button "Add pin+"
bool cNode::RenderSequenceNode(cEditManager &editMgr
	, util::BlueprintNodeBuilder &builder
	, sPin* newLinkPin //= nullptr
)
{
	ImGui::Spring(0);
	if (ImGui::Button("Add Pin+"))
	{
		StrId text;
		text.Format("Then %d", m_outputs.size());
		vpl::sPin pin(editMgr.GetUniqueId(), text.c_str(), ePinType::Flow);
		pin.typeStr = "Flow";
		pin.nodeId = m_id;
		pin.kind = ePinKind::Output;
		pin.value = 0;
		m_outputs.push_back(pin);
	}
	return true;
}


void cNode::Clear()
{
	m_id = 0;
	m_name.clear();
	m_desc.clear();
	m_inputs.clear();
	m_outputs.clear();
}
