//
// Copyright (c) 2021 Yuriy Zinchenko.
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

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include "Dialog.h"
#include "ShellState/ShellState.h"

using namespace Urho3D;

Dialog::Dialog(Urho3D::Context* context)
	: Object(context)
	, closeable_(false)
	, interactive_(false)
{
}

Dialog::~Dialog()
{
	if (root_)
		root_->Remove();
}

void Dialog::LoadLayout(const Urho3D::String& layoutName)
{
	UI* ui = GetSubsystem<UI>();
	XMLFile* layout = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(layoutName);
	root_ = ui->LoadLayout(layout);
	root_->SetStyleAuto();
	ui->GetRoot()->AddChild(root_);
	if (closeable_)
	{
		UIElement* closeButton = root_->GetChild("CloseButton", true);
		if (closeButton)
			SubscribeToEvent(closeButton, E_PRESSED, std::bind(&Dialog::Close, this));
	}
}

void Dialog::ShrinkSize() { root_->SetSize(IntVector2(0, 0)); }
bool Dialog::IsFrontElement() const { return GetSubsystem<UI>()->GetFrontElement() == root_.Get(); }
void Dialog::Close() { GetParent()->RemoveDialog(GetType()); }

void Dialog::BindButtonToClose(Urho3D::UIElement* button)
{
	SubscribeToEvent(button, E_PRESSED, [this](StringHash, VariantMap&) { Close(); });
}
