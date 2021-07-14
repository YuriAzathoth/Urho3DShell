#ifndef CONFIG_H
#define CONFIG_H

#include <Urho3D/Container/FlagSet.h>
#include <Urho3D/Core/Object.h>
#include <functional>

namespace Urho3D
{
class XMLElement;
}

class Config : public Urho3D::Object
{
	URHO3D_OBJECT(Config, Urho3D::Object)

public:
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
		EnumVariant(Urho3D::String caption, Urho3D::Variant value)
			: caption_(caption)
			, value_(value)
		{
		}
	};

	using SimpleReaderFunc = std::function<Urho3D::Variant()>;
	using SimpleWriterFunc = std::function<void(const Urho3D::Variant&)>;
	using ComplexWriterFunc = std::function<void(const Urho3D::VariantMap&)>;
	using EnumVector = Urho3D::Vector<EnumVariant>;
	using EnumConstructorFunc = std::function<EnumVector()>;

	explicit Config(Urho3D::Context* context);

	void LoadXML(const Urho3D::XMLElement& source);
	void SaveXML(Urho3D::XMLElement& dst) const;

	void ExtractEngineParameters(Urho3D::VariantMap& dst);
	void Apply(bool engineToo = true);
	void Clear();

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

	void GetComplexValue(const Urho3D::VariantMap& complexParameters, Urho3D::StringHash parameter) const;

	void GetDebugString(Urho3D::String& dst);

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
		Urho3D::String name_;
		Urho3D::StringHash settingsTab_;
		Urho3D::VariantType type_;
		Urho3D::FlagSet<ParameterFlags> flags_;
	};

	struct SettingsTab
	{
		Urho3D::PODVector<Urho3D::StringHash> settings_;
		Urho3D::String name_;
	};

	class SimpleReader : public Reader
	{
	public:
		explicit SimpleReader(SimpleReaderFunc reader)
			: reader_(reader)
		{
		}
		Urho3D::Variant Read() override { return reader_(); }

	private:
		const SimpleReaderFunc reader_;
	};

	class SimpleWriter : public Writer
	{
	public:
		explicit SimpleWriter(SimpleWriterFunc writer)
			: writer_(writer)
		{
		}
		void Write(const Urho3D::Variant& value) override { writer_(value); }

	private:
		const SimpleWriterFunc writer_;
	};

	struct ComplexStorage : public Urho3D::RefCounted
	{
		ComplexWriterFunc writer_;
		Urho3D::VariantMap parameters_;
		unsigned char parametersCount_;
	};

	class ComplexWriter : public Writer
	{
	public:
		ComplexWriter(Urho3D::SharedPtr<ComplexStorage> storage, Urho3D::StringHash name)
			: storage_(storage)
			, name_(name)
		{
			++storage_->parametersCount_;
		}
		~ComplexWriter() { --storage_->parametersCount_; }
		void Write(const Urho3D::Variant& value) override { storage_->parameters_[name_] = value; }

	private:
		Urho3D::SharedPtr<ComplexStorage> storage_;
		const Urho3D::StringHash name_;
	};

	using ParametersMap = Urho3D::HashMap<Urho3D::StringHash, Parameter>;

	ParametersMap parameters_;
	Urho3D::HashMap<Urho3D::StringHash, EnumConstructorFunc> enumConstructors_;
	Urho3D::HashMap<Urho3D::StringHash, Urho3D::SharedPtr<ComplexStorage>> storages_;
	Urho3D::VariantMap changedParameters_;
	Urho3D::HashMap<Urho3D::StringHash, SettingsTab> settings_;
};

#endif // CONFIG_H
