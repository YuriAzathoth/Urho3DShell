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

#ifndef CONFIG_H
#define CONFIG_H

#include <Urho3D/Container/FlagSet.h>
#include <Urho3D/Core/Object.h>
#include <functional>
#include "Urho3DShellAPI.h"

namespace Urho3D
{
class UIElement;
class XMLElement;
} // namespace Urho3D

class URHO3DSHELLAPI_EXPORT Config : public Urho3D::Object
{
	URHO3D_OBJECT(Config, Urho3D::Object)

public:
	using SimpleReaderFunc = std::function<Urho3D::Variant()>;
	using SimpleWriterFunc = std::function<void(const Urho3D::Variant&)>;
	using ComplexWriterFunc = std::function<void(const Urho3D::VariantMap&)>;

	struct Reader : public Urho3D::RefCounted
	{
		virtual Urho3D::Variant Read() = 0;
	};

	struct Writer : public Urho3D::RefCounted
	{
		virtual void Write(const Urho3D::Variant& value) = 0;
	};

	struct EnumVariant
	{
		Urho3D::String caption_;
		Urho3D::Variant value_;
		EnumVariant() = default;
		EnumVariant(const char* caption, Urho3D::Variant value)
			: caption_(caption)
			, value_(value)
		{
		}
		EnumVariant(const Urho3D::String& caption, Urho3D::Variant value)
			: caption_(caption)
			, value_(value)
		{
		}
		EnumVariant(Urho3D::String&& caption, Urho3D::Variant value)
			: caption_(std::move(caption))
			, value_(value)
		{
		}
		EnumVariant(const EnumVariant& src)
			: caption_(src.caption_)
			, value_(src.value_)
		{
		}
		EnumVariant(EnumVariant&& src) noexcept
			: caption_(std::move(src.caption_))
			, value_(src.value_)
		{
		}
		EnumVariant& operator=(const EnumVariant& src)
		{
			caption_ = src.caption_;
			value_ = src.value_;
			return *this;
		}
		EnumVariant& operator=(EnumVariant&& src)
		{
			caption_.Swap(src.caption_);
			value_ = src.value_;
			return *this;
		}
	};

	using EnumVector = Urho3D::Vector<EnumVariant>;
	using EnumConstructorFunc = std::function<EnumVector()>;

	struct ComplexStorage : public Urho3D::RefCounted
	{
		ComplexWriterFunc writer_;
		Urho3D::VariantMap parameters_;
		bool isEngine_;
	};

	using Urho3D::Object::Object;

	void LoadXML(Urho3D::VariantMap& dst, const Urho3D::XMLElement& source);
	void LoadXML(const Urho3D::XMLElement& source);
	void SaveXML(Urho3D::XMLElement& dst) const;

	void Apply(const Urho3D::VariantMap& parameters);
	void ExtractEngineParameters(Urho3D::VariantMap& engineParameters, Urho3D::VariantMap& shellParameters);

	void RegisterSettingsTab(const Urho3D::String& tabName);
	void RemoveSettingsTab(Urho3D::StringHash tab);
	Urho3D::StringVector GetSettingsTabs() const;
	Urho3D::StringVector GetSettings(Urho3D::StringHash settingsTab) const;

	bool RegisterParameter(const Urho3D::String& parameterName,
						   Urho3D::VariantType type,
						   Urho3D::StringHash settingsTab,
						   bool isEngine,
						   bool localized);

	void RemoveParameter(Urho3D::StringHash parameter);

	void RegisterReader(Urho3D::StringHash parameter, SimpleReaderFunc reader);
	void RegisterWriter(Urho3D::StringHash parameter, SimpleWriterFunc writer);
	void RegisterEnum(Urho3D::StringHash parameter, EnumConstructorFunc enumConstructor);

	bool RegisterComplexStorage(Urho3D::StringHash cathegory, ComplexWriterFunc writer);
	bool RegisterComplexWriter(Urho3D::StringHash parameter, Urho3D::StringHash cathegory);

	const Urho3D::String& GetName(Urho3D::StringHash parameter) const;
	Urho3D::VariantType GetType(Urho3D::StringHash parameter) const;
	bool IsEnum(Urho3D::StringHash parameter) const;
	bool IsLocalized(Urho3D::StringHash parameter) const;
	Urho3D::Variant ReadValue(Urho3D::StringHash parameter) const;
	void WriteValue(Urho3D::StringHash parameter, const Urho3D::Variant& value);
	EnumVector ConstructEnum(Urho3D::StringHash parameter) const;

	Urho3D::String GetDebugString() const;

	void CreateParameterControl(Urho3D::StringHash parameter, Urho3D::UIElement* parent);

	void RegisterClientParameters();
	void RegisterServerParameters();

private:
	enum class ParameterFlags : unsigned char
	{
		ENGINE = 0x1,
		ENUM = 0x2,
		LOCALIZED = 0x4
	};

	struct Parameter
	{
		Urho3D::SharedPtr<Reader> reader_;
		Urho3D::SharedPtr<Writer> writer_;
		Urho3D::StringHash settingsTab_;
		Urho3D::VariantType type_;
		Urho3D::FlagSet<ParameterFlags> flags_;
	};

	Urho3D::HashMap<Urho3D::StringHash, Parameter> parameters_;
	Urho3D::HashMap<Urho3D::StringHash, EnumConstructorFunc> enumConstructors_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<ComplexStorage>> storages_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::PODVector<Urho3D::StringHash>> settings_;
	Urho3D::StringMap names_;

private:
	static Urho3D::IntVector3 StrToRes(const Urho3D::String& str);
	static Urho3D::String ResToStr(const Urho3D::IntVector3& res);
};

#endif // CONFIG_H
