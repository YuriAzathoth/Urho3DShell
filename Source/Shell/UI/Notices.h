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

#ifndef NOTICES_H
#define NOTICES_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class MessageBox;
}

class URHO3DSHELLAPI_EXPORT Notices : public Urho3D::Object
{
	URHO3D_OBJECT(Notices, Urho3D::Object)

public:
	explicit Notices(Urho3D::Context* context);

	void Error(const Urho3D::String& text, const Urho3D::String& title);
	void Question(const Urho3D::String& text, const Urho3D::String& title);

private:
	void ShowDialog(const Urho3D::String& text, const Urho3D::String& title);
	void ShowToConsole(const Urho3D::String& text, const Urho3D::String& title);
	void EnableCancelButton();

	void OnMessageACK(Urho3D::StringHash, Urho3D::VariantMap& eventData);

	Urho3D::MessageBox* message_;
	bool exist_;
};

#endif // NOTICES_H
