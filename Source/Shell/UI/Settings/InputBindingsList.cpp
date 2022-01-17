//
// Copyright (c) 2021-2022 Yuriy Zinchenko.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Container/HashSet.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UIEvents.h>
#include "Input/ActionsRegistry.h"
#include "Input/ControllersRegistry.h"
#include "Input/InputEvents.h"
#include "InputBindingsList.h"

#define PRESS_A_KEY_TEXT "PressAKey"

using namespace Urho3D;

InputBindingsList::InputBindingsList(Urho3D::Context* context, Urho3D::ListView* settings)
	: SettingsList(context, settings)
	, binding_(false)
{
	const StringVector controllers = CreateControllersList();
	CreateSensitivitySlider();

	const ActionsRegistry* actions = GetSubsystem<ActionsRegistry>();
	for (StringHash action : actions->GetAll())
		CreateActionBinding(actions->GetName(action));

	if (!controllers.Empty())
		LoadControllerSettings(GetSubsystem<ControllersRegistry>()->Get(controllers[0]));

	SubscribeToEvent(E_INPUTBINDINGEND, URHO3D_HANDLER(InputBindingsList, OnBindingEnd));
}

InputBindingsList::~InputBindingsList() { StopBinding(); }

void InputBindingsList::Apply()
{
	const ControllersRegistry* controllers = GetSubsystem<ControllersRegistry>();
	InputController* controller;
	const ChangedParameters* parameters;
	for (const auto& ctrlPair : changedBindings_)
	{
		parameters = &ctrlPair.second_;
		controller = controllers->Get(ctrlPair.first_);
		for (const auto& actionPair : parameters->bindings_)
			controller->SetBinding(actionPair.first_, actionPair.second_);
		if (parameters->sensitivityChanged_)
			controller->SetSensitivity(parameters->sensitivity_);
	}
	changedBindings_.Clear();
}

Urho3D::StringVector InputBindingsList::CreateControllersList()
{
	SharedPtr<UIElement> item;
	Text* caption;
	currentCtlr_ = CreateCaption("Controller")->CreateChild<DropDownList>();
	currentCtlr_->SetLayout(LM_VERTICAL, 0, {4, 4, 4, 4});
	currentCtlr_->SetResizePopup(true);
	currentCtlr_->SetStyleAuto();
	SubscribeToEvent(currentCtlr_, E_ITEMSELECTED, URHO3D_HANDLER(InputBindingsList, OnControllerSelected));

	const StringVector controllers = GetSubsystem<ControllersRegistry>()->GetEnabledNames();
	for (const String& controller : controllers)
	{
		item = MakeShared<UIElement>(context_);
		currentCtlr_->AddItem(item);
		item->SetName(controller);
		item->SetLayoutMode(LM_VERTICAL);
		item->SetStyleAuto();

		caption = item->CreateChild<Text>();
		caption->SetText(controller);
		caption->SetStyleAuto();
	}
	return controllers;
}

void InputBindingsList::CreateSensitivitySlider()
{
	UIElement* item = CreateCaption("Sensitivity")->CreateChild<UIElement>();
	item->SetLayoutMode(LM_HORIZONTAL);
	item->SetStyleAuto();

	sensitivity_ = item->CreateChild<Slider>();
	sensitivity_->SetRange(1.0f);
	sensitivity_->SetStyleAuto();

	LineEdit* lineEdit = item->CreateChild<LineEdit>();
	lineEdit->SetStyleAuto();

	SubscribeToEvent(sensitivity_,
					 E_SLIDERCHANGED,
					 [this, lineEdit](StringHash, VariantMap& eventData)
					 {
						 using namespace SliderChanged;
						 const float value = eventData[P_VALUE].GetFloat();
						 lineEdit->SetText(ToString("%f", value).Substring(0, 4));
						 ChangedParameters& parameters = changedBindings_[GetCurrentControllerName()];
						 parameters.sensitivity_ = value;
						 parameters.sensitivityChanged_ = true;
					 });
	SubscribeToEvent(lineEdit,
					 E_TEXTCHANGED,
					 [this](StringHash, VariantMap& eventData)
					 {
						 using namespace TextChanged;
						 const String& text = eventData[P_TEXT].GetString();
						 sensitivity_->SetValue(ToFloat(text));
					 });
}

void InputBindingsList::CreateActionBinding(const Urho3D::String& actionName)
{
	UIElement* item = CreateCaption(actionName);
	actions_[actionName] = item;

	Button* button = item->CreateChild<Button>();
	button->SetName(actionName);
	button->SetLayout(LM_VERTICAL, 4, {4, 4, 4, 4});
	button->SetStyleAuto();

	Text* caption = button->CreateChild<Text>();
	caption->SetStyleAuto();

	button->SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(InputBindingsList, OnBindingBegin));
}

void InputBindingsList::LoadControllerSettings(const InputController* controller)
{
	HashSet<StringHash> emptyActions;
	for (const auto& p : actions_)
		emptyActions.Insert(p.first_);

	sensitivity_->SetValue(controller->GetSensitivity());

	const InputController::BindingsMap& bindings = controller->GetBindings();
	decltype(actions_)::ConstIterator actionIt;
	Text* caption;
	for (const auto& p : bindings)
	{
		actionIt = actions_.Find(p.second_);
		if (actionIt != actions_.End())
		{
			caption = actionIt->second_->GetChild(1)->GetChildStaticCast<Text>(0);
			caption->SetText(controller->GetKeyName(p.first_));
		}
	}
}

const Urho3D::String& InputBindingsList::GetCurrentControllerName() const
{
	return currentCtlr_->GetSelectedItem()->GetName();
}

InputController* InputBindingsList::GetCurrentController() const
{
	return GetSubsystem<ControllersRegistry>()->Get(GetCurrentControllerName());
}

void InputBindingsList::StopBinding()
{
	if (binding_)
		GetCurrentController()->EndBinding();
}

void InputBindingsList::OnControllerSelected(Urho3D::StringHash, Urho3D::VariantMap&)
{
	StopBinding();
	LoadControllerSettings(GetCurrentController());
}

void InputBindingsList::OnBindingBegin(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace Pressed;

	const UIElement* button = static_cast<const UIElement*>(eventData[P_ELEMENT].GetPtr());
	Text* caption = button->GetChildStaticCast<Text>(0);
	caption->SetText(PRESS_A_KEY_TEXT);
	caption->SetAutoLocalizable(true);

	GetCurrentController()->StartBinding(button->GetName());

	binding_ = true;
}

void InputBindingsList::OnBindingEnd(Urho3D::StringHash, Urho3D::VariantMap& eventData)
{
	using namespace InputBindingEnd;
	InputController* controller = GetCurrentController();
	const StringHash action = eventData[P_ACTION].GetStringHash();
	const unsigned keyCode = eventData[P_KEY].GetInt();
	controller->SetBinding(action, keyCode);

	const String keyName = controller->GetKeyName(keyCode);
	Text* caption = actions_[action]->GetChild(1)->GetChildStaticCast<Text>(0);
	caption->SetText(keyName);
	caption->SetAutoLocalizable(false);

	binding_ = false;
}
