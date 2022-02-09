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

#ifndef SCRIPTPLUGIN_H
#define SCRIPTPLUGIN_H

#include "Plugin.h"

namespace Urho3D
{
class ScriptFile;
}

class ScriptPlugin : public Plugin
{
	URHO3D_OBJECT(ScriptPlugin, Plugin)

public:
	explicit ScriptPlugin(Urho3D::Context* context);
	~ScriptPlugin();

	bool Load(const Urho3D::String& fileName) override;

	const Urho3D::String& GetName() const override;

private:
	bool Execute(const Urho3D::String& functionName,
				 const Urho3D::VariantVector& parameters = Urho3D::Variant::emptyVariantVector,
				 Urho3D::Variant* returnVar = nullptr);

	Urho3D::ScriptFile* script_;

public:
	static const char* GetExtension() { return ".as"; }
};

#endif // SCRIPTPLUGIN_H
