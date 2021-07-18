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
#include "UIDialog.h"

using namespace Urho3D;

UIDialog::UIDialog(Urho3D::Context* context)
	: Object(context)
{
}

UIDialog::~UIDialog()
{
	if (root_.NotNull())
		root_->Remove();
}

void UIDialog::LoadLayout(const Urho3D::String& filename)
{
	UI* ui = GetSubsystem<UI>();
	XMLFile* layoutFilePtr = GetSubsystem<ResourceCache>()->GetResource<XMLFile>(filename);
	if (layoutFilePtr)
		root_ = ui->LoadLayout(layoutFilePtr);
	else
	{
		XMLFile layoutFile(context_);
		if (layoutFile.LoadFile(filename))
			root_ = ui->LoadLayout(&layoutFile);
	}
}
