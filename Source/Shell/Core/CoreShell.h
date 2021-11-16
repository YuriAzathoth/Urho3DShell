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

#ifndef CORESHELL_H
#define CORESHELL_H

#include <Urho3D/Core/Object.h>
#include "Urho3DShellAPI.h"

class URHO3DSHELLAPI_EXPORT CoreShell : public Urho3D::Object
{
	URHO3D_OBJECT(CoreShell, Urho3D::Object)

public:
	explicit CoreShell(Urho3D::Context* context);
	~CoreShell();

	void LoadGameLibrary(const Urho3D::String& gameLib);
	void LoadConfig(Urho3D::VariantMap& engineParameters, const Urho3D::String& appName);

	void LoadPlugin(const Urho3D::String& plugin);
	void ApplyConfig();

	const Urho3D::Variant& GetShellParameter(Urho3D::StringHash parameter,
											 const Urho3D::Variant& defaultValue = Urho3D::Variant::EMPTY) const;

private:
	void ParseParameters();

	Urho3D::VariantMap shellParameters_;
};

#endif // CORESHELL_H
