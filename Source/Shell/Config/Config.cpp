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

#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/XMLElement.h>
#include "Config.h"
#include "ConfigDefs.h"

using namespace Urho3D;

static IntVector3 StrToRes(const String& str);
static String ResToStr(const IntVector3& res);

Config::Config(Urho3D::Context* context)
	: Object(context)
{
	RegisterParameters();
}

void Config::LoadXML(const Urho3D::XMLElement& source)
{
	String name;
	Variant value;
	for (XMLElement parameter = source.GetChild("parameter"); !parameter.IsNull(); parameter = parameter.GetNext())
	{
		name = parameter.GetAttribute("name");
		value = parameter.GetVariant();
		if (!name.Empty() && !value.IsEmpty() && parameters_.Contains(name))
			changedParameters_[name] = value;
	}
}

void Config::SaveXML(Urho3D::XMLElement& dst) const
{
	XMLElement parameter;
	for (const auto& p : parameters_)
	{
		parameter = dst.CreateChild("parameter");
		parameter.SetAttribute("name", p.second_.name_);
		parameter.SetVariant(p.second_.reader_->Read());
	}
}

void Config::ExtractEngineParameters(Urho3D::VariantMap& dst)
{
	// TODO: Implement registrable engine parameters forwarding
	auto it = changedParameters_.Find(ECP_RESOLUTION);
	if (it != changedParameters_.End())
	{
		const IntVector3 resolution = StrToRes(it->second_.GetString());
		dst[EP_WINDOW_WIDTH] = resolution.x_;
		dst[EP_WINDOW_HEIGHT] = resolution.y_;
		dst[EP_REFRESH_RATE] = resolution.z_;
	}
	it = changedParameters_.Find(ECP_WINDOW_MODE);
	if (it != changedParameters_.End())
	{
		const int windowMode = it->second_.GetInt();
		dst[EP_FULL_SCREEN] = windowMode >= 1;
		dst[EP_BORDERLESS] = windowMode == 2;
	}
	for (it = changedParameters_.Begin(); it != changedParameters_.End();)
		if (parameters_[it->first_].flags_ & ParameterFlags::ENGINE)
		{
			dst.Insert(it);
			it = changedParameters_.Erase(it);
		}
		else
			++it;
}

void Config::Apply(bool engineToo)
{
	if (!changedParameters_.Empty())
	{
		ParametersMap::Iterator it;
		for (const auto& p : changedParameters_)
		{
			it = parameters_.Find(p.first_);
			if (it != parameters_.End() && !(engineToo && it->second_.flags_ & ParameterFlags::ENGINE))
				it->second_.writer_->Write(p.second_);
		}
		changedParameters_.Clear();
	}
	for (auto& p : storages_)
	{
		SharedPtr<ComplexStorage> storage = p.second_;
		if (!storage->parameters_.Empty() && !(engineToo && storage->isEngine_))
		{
			storage->writer_(p.second_->parameters_);
			storage->parameters_.Clear();
		}
	}
}

void Config::Clear()
{
	changedParameters_.Clear();
	enumConstructors_.Clear();
	for (auto& p : storages_)
		p.second_->parameters_.Clear();
}

void Config::RegisterSettingsTab(const Urho3D::String& tabName) { settings_[tabName].name_ = tabName; }

void Config::RemoveSettingsTab(Urho3D::StringHash tab) { settings_.Erase(tab); }

Urho3D::StringVector Config::GetSettingsTabs() const
{
	StringVector ret;
	ret.Reserve(settings_.Size());
	for (const auto& p : settings_)
		ret.Push(p.second_.name_);
	return ret;
}

Urho3D::StringVector Config::GetSettings(Urho3D::StringHash settingsTab) const
{
	StringVector ret;
	const auto it = settings_.Find(settingsTab);
	if (it != settings_.End())
	{
		const SettingsTab& tab = it->second_;
		ret.Reserve(tab.settings_.Size());
		for (StringHash setting : tab.settings_)
			ret.Push(parameters_[setting]->name_);
	}
	return ret;
}

bool Config::RegisterParameter(const Urho3D::String& parameterName,
							   Urho3D::VariantType type,
							   Urho3D::StringHash settingsTab,
							   bool isEngine,
							   bool localized)
{
	auto it = settings_.Find(settingsTab);
	if (!parameters_.Contains(parameterName) && it != settings_.End())
	{
		Parameter& parameter = parameters_[parameterName];
		parameter.name_ = parameterName;
		parameter.settingsTab_ = settingsTab;
		parameter.type_ = type;
		if (isEngine)
			parameter.flags_ |= ParameterFlags::ENGINE;
		if (localized)
			parameter.flags_ |= ParameterFlags::LOCALIZED;
		it->second_.settings_.Push(parameterName);
		return true;
	}
	else
	{
		URHO3D_LOGWARNINGF("Failed to register parameter %s: parameter is already registered.",
						   parameterName.CString());
		return false;
	}
}

void Config::RemoveParameter(Urho3D::StringHash parameter)
{
	auto itParameter = parameters_.Find(parameter);
	if (itParameter != parameters_.End())
	{
		auto itSetting = settings_.Find(itParameter->second_.settingsTab_);
		if (itSetting != settings_.End())
			settings_.Erase(itSetting);
		if (itParameter->second_.flags_ & ParameterFlags::ENUM)
			enumConstructors_.Erase(itParameter->first_);
		parameters_.Erase(itParameter);
	}
}

void Config::RegisterReader(Urho3D::StringHash parameter, SimpleReaderFunc reader)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
		it->second_.reader_ = new SimpleReader(reader);
}

void Config::RegisterWriter(Urho3D::StringHash parameter, SimpleWriterFunc writer)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
		it->second_.writer_ = new SimpleWriter(writer);
}

void Config::RegisterEnum(Urho3D::StringHash parameter, EnumConstructorFunc enumConstructor)
{
	auto it = parameters_.Find(parameter);
	if (it != parameters_.End())
	{
		it->second_.flags_ |= ParameterFlags::ENUM;
		enumConstructors_[parameter] = enumConstructor;
	}
}

bool Config::RegisterComplexStorage(Urho3D::StringHash cathegory, ComplexWriterFunc writer)
{
	if (!storages_.Contains(cathegory))
	{
		SharedPtr<ComplexStorage> storage = MakeShared<ComplexStorage>();
		storage->writer_ = writer;
		return true;
	}
	else
		return false;
}

bool Config::RegisterComplexWriter(Urho3D::StringHash parameter, Urho3D::StringHash cathegory)
{
	auto itParameter = parameters_.Find(parameter);
	auto itStorage = storages_.Find(cathegory);
	if (itParameter != parameters_.End() && itStorage != storages_.End())
	{
		itParameter->second_.writer_ = new ComplexWriter(itStorage->second_, parameter);
		itStorage->second_->isEngine_ = itParameter->second_.flags_ & ParameterFlags::ENGINE;
		return true;
	}
	else
		return false;
}

void Config::GetDebugString(Urho3D::String& dst) const
{
	// ATTENTION! Slow code! Use it ONLY for debugging purposes!
	StringVector tabs = GetSettingsTabs();
	StringVector parameters;
	Variant value;
	EnumVector enumVector;
	for (const String& tabName : tabs)
	{
		dst.Append(tabName).Append('\n');
		parameters = GetSettings(tabName);
		for (const String& parameterName : parameters)
		{
			const Parameter& parameter = *parameters_[parameterName];
			value = parameter.reader_->Read();
			dst.Append("\t").Append(parameterName).Append('\n');
			dst.Append("\t\tType  = ").Append(value.GetTypeName()).Append('\n');
			dst.Append("\t\tValue = ").Append(value.ToString()).Append('\n');
			dst.Append("\t\tStore = ");
			if (parameter.flags_ & ParameterFlags::ENGINE)
				dst.Append("ENGINE\n");
			else
				dst.Append("CUSTOM\n");
			if (parameter.flags_ & ParameterFlags::ENUM)
			{
				dst.Append("\t\tEnum Variants:\n");
				enumVector = (*enumConstructors_[parameterName])();
				for (const EnumVariant& enumVariant : enumVector)
					dst.Append("\t\t\t")
						.Append(enumVariant.caption_)
						.Append(" = ")
						.Append(enumVariant.value_.ToString())
						.Append('\n');
			}
		}
	}
}

void Config::RegisterParameters()
{
	RegisterSettingsTab(ST_GAME);
	{
		RegisterParameter(CP_LANGUAGE, VAR_STRING, ST_GAME, false, false);
		RegisterReader(CP_LANGUAGE, [this]() { return GetSubsystem<Localization>()->GetLanguage(); });
		RegisterWriter(CP_LANGUAGE,
					   [this](const Urho3D::Variant& value)
					   { GetSubsystem<Localization>()->SetLanguage(value.GetString()); });
		RegisterEnum(CP_LANGUAGE,
					 [this]()
					 {
						 Localization* l10n = GetSubsystem<Localization>();
						 const int numLanguages = l10n->GetNumLanguages();
						 EnumVector ret;
						 ret.Reserve(numLanguages);
						 String language;
						 for (int i = 0; i < numLanguages; ++i)
						 {
							 language = l10n->GetLanguage(i);
							 ret.EmplaceBack(language, language);
						 }
						 return ret;
					 });
	}

	RegisterSettingsTab(ST_VIDEO);
	{
		// TODO: Convenient complex storage values extractor
		RegisterComplexStorage(
			ECP_VIDEO_MODE,
			[this](const VariantMap& values)
			{
				VariantMap::ConstIterator it = values.Find(ECP_WINDOW_MODE);
				bool borderless, fullscreen;
				const int windowMode =
					(it != values.End() ? it->second_ : parameters_[ECP_WINDOW_MODE].reader_->Read()).GetInt();
				switch (windowMode)
				{
				case 0:
					borderless = false;
					fullscreen = false;
					break;
				case 1:
					borderless = false;
					fullscreen = true;
					break;
				case 2:
					borderless = true;
					fullscreen = true;
					break;
				}

				it = values.Find(ECP_RESOLUTION);
				const IntVector3 res = StrToRes(
					(it != values.End() ? it->second_ : parameters_[ECP_WINDOW_MODE].reader_->Read()).GetString());
				const int width = res.x_;
				const int height = res.y_;
				const int refreshRate = res.z_;

				it = values.Find(EP_VSYNC);
				const bool vsync = (it != values.End() ? it->second_ : parameters_[EP_VSYNC].reader_->Read()).GetBool();
				it = values.Find(EP_TRIPLE_BUFFER);
				const bool trippleBuffer =
					(it != values.End() ? it->second_ : parameters_[EP_TRIPLE_BUFFER].reader_->Read()).GetBool();
				it = values.Find(EP_MULTI_SAMPLE);
				const bool multiSample =
					(it != values.End() ? it->second_ : parameters_[EP_MULTI_SAMPLE].reader_->Read()).GetInt();
				it = values.Find(EP_MONITOR);
				const bool monitor =
					(it != values.End() ? it->second_ : parameters_[EP_MONITOR].reader_->Read()).GetInt();

				GetSubsystem<Graphics>()->SetMode(width,
												  height,
												  fullscreen,
												  borderless,
												  false,
												  false,
												  vsync,
												  trippleBuffer,
												  multiSample,
												  monitor,
												  refreshRate);
			});

		RegisterParameter(EP_MONITOR, VAR_INT, ST_VIDEO, true, false);
		RegisterReader(EP_MONITOR, [this]() { return GetSubsystem<Graphics>()->GetMonitor(); });
		RegisterComplexWriter(EP_MONITOR, ECP_VIDEO_MODE);
		RegisterEnum(EP_MONITOR,
					 [this]()
					 {
						 const int monitorCount = GetSubsystem<Graphics>()->GetMonitorCount();
						 EnumVector ret;
						 ret.Reserve(monitorCount);
						 for (int i = 0; i < monitorCount; ++i)
							 ret.EmplaceBack(ToString("%d", i), i);
						 return ret;
					 });

		RegisterParameter(ECP_RESOLUTION, VAR_STRING, ST_VIDEO, true, false);
		RegisterReader(ECP_RESOLUTION,
					   [this]()
					   {
						   Graphics* graphics = GetSubsystem<Graphics>();
						   return ResToStr({graphics->GetWidth(), graphics->GetHeight(), graphics->GetRefreshRate()});
					   });
		RegisterComplexWriter(ECP_RESOLUTION, ECP_VIDEO_MODE);
		RegisterEnum(ECP_RESOLUTION,
					 [this]()
					 {
						 Graphics* graphics = GetSubsystem<Graphics>();
						 const PODVector<IntVector3> resolutions = graphics->GetResolutions(graphics->GetMonitor());
						 EnumVector ret;
						 ret.Reserve(resolutions.Size());
						 String resStr;
						 for (const IntVector3& resolution : resolutions)
						 {
							 resStr = ResToStr(resolution);
							 ret.EmplaceBack(resStr, resStr);
						 }
						 return ret;
					 });

		RegisterParameter(ECP_WINDOW_MODE, VAR_INT, ST_VIDEO, true, true);
		RegisterReader(ECP_WINDOW_MODE,
					   [this]()
					   {
						   Graphics* graphics = GetSubsystem<Graphics>();
						   return graphics->GetBorderless() ? 2 : graphics->GetFullscreen() ? 1 : 0;
					   });
		RegisterComplexWriter(ECP_WINDOW_MODE, ECP_VIDEO_MODE);
		RegisterEnum(ECP_RESOLUTION,
					 [this]() -> EnumVector {
						 return {{"Windowed", 0}, {"FullScreen", 1}, {"Borderless", 2}};
					 });

		RegisterParameter(EP_VSYNC, VAR_BOOL, ST_VIDEO, true, false);
		RegisterReader(EP_VSYNC, [this]() { return GetSubsystem<Graphics>()->GetVSync(); });
		RegisterComplexWriter(EP_VSYNC, ECP_VIDEO_MODE);

		RegisterParameter(EP_TRIPLE_BUFFER, VAR_BOOL, ST_VIDEO, true, false);
		RegisterReader(EP_TRIPLE_BUFFER, [this]() { return GetSubsystem<Graphics>()->GetTripleBuffer(); });
		RegisterComplexWriter(EP_TRIPLE_BUFFER, ECP_VIDEO_MODE);

		RegisterParameter(EP_MULTI_SAMPLE, VAR_INT, ST_VIDEO, true, false);
		RegisterReader(EP_MULTI_SAMPLE, [this]() { return GetSubsystem<Graphics>()->GetMultiSample(); });
		RegisterComplexWriter(EP_MULTI_SAMPLE, ECP_VIDEO_MODE);
		RegisterEnum(EP_MULTI_SAMPLE,
					 [this]()
					 {
						 const PODVector<int> msLevels = GetSubsystem<Graphics>()->GetMultiSampleLevels();
						 EnumVector ret;
						 ret.Reserve(msLevels.Size());
						 for (int level : msLevels)
							 ret.EmplaceBack(ToString("%d", level), level);
						 return ret;
					 });

		RegisterParameter(EP_MATERIAL_QUALITY, VAR_INT, ST_VIDEO, true, true);
		RegisterReader(EP_MATERIAL_QUALITY, [this]() { return GetSubsystem<Renderer>()->GetMaterialQuality(); });
		RegisterWriter(
			EP_MATERIAL_QUALITY,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetMaterialQuality(static_cast<MaterialQuality>(value.GetInt())); });
		RegisterEnum(EP_MATERIAL_QUALITY,
					 [this]() -> EnumVector
					 {
						 return {{"Low", MaterialQuality::QUALITY_LOW},
								 {"Medium", MaterialQuality::QUALITY_MEDIUM},
								 {"High", MaterialQuality::QUALITY_HIGH}};
					 });

		RegisterParameter(EP_TEXTURE_QUALITY, VAR_INT, ST_VIDEO, true, true);
		RegisterReader(EP_TEXTURE_QUALITY, [this]() { return GetSubsystem<Renderer>()->GetTextureQuality(); });
		RegisterWriter(
			EP_TEXTURE_QUALITY,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetTextureQuality(static_cast<MaterialQuality>(value.GetInt())); });
		RegisterEnum(EP_TEXTURE_QUALITY,
					 [this]() -> EnumVector
					 {
						 return {{"Low", MaterialQuality::QUALITY_LOW},
								 {"Medium", MaterialQuality::QUALITY_MEDIUM},
								 {"High", MaterialQuality::QUALITY_HIGH}};
					 });

		RegisterParameter(EP_TEXTURE_FILTER_MODE, VAR_INT, ST_VIDEO, true, true);
		RegisterReader(EP_TEXTURE_FILTER_MODE, [this]() { return GetSubsystem<Renderer>()->GetTextureFilterMode(); });
		RegisterWriter(
			EP_TEXTURE_FILTER_MODE,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetTextureFilterMode(static_cast<TextureFilterMode>(value.GetInt())); });
		RegisterEnum(EP_TEXTURE_FILTER_MODE,
					 [this]() -> EnumVector
					 {
						 return {{"Nearest", TextureFilterMode::FILTER_NEAREST},
								 {"Bilinear", TextureFilterMode::FILTER_BILINEAR},
								 {"Trilinear", TextureFilterMode::FILTER_TRILINEAR},
								 {"Anisotropic", TextureFilterMode::FILTER_ANISOTROPIC},
								 {"NearestAnisotropic", TextureFilterMode::FILTER_NEAREST_ANISOTROPIC}};
					 });

		RegisterParameter(EP_TEXTURE_ANISOTROPY, VAR_INT, ST_VIDEO, true, false);
		RegisterReader(EP_TEXTURE_ANISOTROPY, [this]() { return GetSubsystem<Renderer>()->GetTextureAnisotropy(); });
		RegisterWriter(
			EP_TEXTURE_ANISOTROPY,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetTextureAnisotropy(static_cast<MaterialQuality>(value.GetInt())); });
		RegisterEnum(EP_TEXTURE_ANISOTROPY,
					 [this]() -> EnumVector
					 {
						 constexpr int stages[] = {2, 4, 6, 8, 12, 16};
						 EnumVector ret;
						 ret.Reserve(6);
						 for (int i = 0; i < 6; ++i)
							 ret.EmplaceBack(ToString("%d", i), i);
						 return ret;
					 });

		RegisterParameter(EP_SHADOWS, VAR_BOOL, ST_VIDEO, true, false);
		RegisterReader(EP_SHADOWS, [this]() { return GetSubsystem<Renderer>()->GetDrawShadows(); });
		RegisterWriter(EP_SHADOWS,
					   [this](const Variant& value)
					   { return GetSubsystem<Renderer>()->SetDrawShadows(value.GetBool()); });

		RegisterParameter(CP_SHADOW_QUALITY, VAR_INT, ST_VIDEO, false, false);
		RegisterReader(CP_SHADOW_QUALITY, [this]() { return GetSubsystem<Renderer>()->GetShadowQuality(); });
		RegisterWriter(
			CP_SHADOW_QUALITY,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetShadowQuality(static_cast<ShadowQuality>(value.GetInt())); });
		RegisterEnum(CP_SHADOW_QUALITY,
					 [this]() -> EnumVector
					 {
						 return {{"Simple16", ShadowQuality::SHADOWQUALITY_SIMPLE_16BIT},
								 {"Simple24", ShadowQuality::SHADOWQUALITY_SIMPLE_24BIT},
								 {"PCF16", ShadowQuality::SHADOWQUALITY_PCF_16BIT},
								 {"PCF24", ShadowQuality::SHADOWQUALITY_PCF_24BIT},
								 {"VSM", ShadowQuality::SHADOWQUALITY_VSM},
								 {"VSMBlur", ShadowQuality::SHADOWQUALITY_BLUR_VSM}};
					 });

		RegisterParameter(CP_SHADOW_RESOLUTION, VAR_INT, ST_VIDEO, false, false);
		RegisterReader(CP_SHADOW_RESOLUTION, [this]() { return GetSubsystem<Renderer>()->GetShadowMapSize(); });
		RegisterWriter(
			CP_SHADOW_RESOLUTION,
			[this](const Variant& value)
			{ return GetSubsystem<Renderer>()->SetShadowMapSize(static_cast<ShadowQuality>(value.GetInt())); });
		RegisterEnum(CP_SHADOW_RESOLUTION,
					 [this]() -> EnumVector
					 {
						 EnumVector ret;
						 ret.Reserve(5);
						 for (int i = 256; i <= 4096; i *= 2)
							 ret.EmplaceBack(ToString("%dx%d", i, i), i);
						 return ret;
					 });
	}

	RegisterSettingsTab(ST_AUDIO);
	{
		RegisterParameter(CP_SOUND_MASTER, VAR_FLOAT, ST_AUDIO, false, false);
		RegisterReader(CP_SOUND_MASTER, [this]() { return GetSubsystem<Audio>()->GetMasterGain(SOUND_MASTER); });
		RegisterWriter(CP_SOUND_MASTER,
					   [this](const Variant& value)
					   { GetSubsystem<Audio>()->SetMasterGain(SOUND_MASTER, value.GetFloat()); });

		RegisterParameter(CP_SOUND_EFFECT, VAR_FLOAT, ST_AUDIO, false, false);
		RegisterReader(CP_SOUND_EFFECT, [this]() { return GetSubsystem<Audio>()->GetMasterGain(SOUND_EFFECT); });
		RegisterWriter(CP_SOUND_EFFECT,
					   [this](const Variant& value)
					   { GetSubsystem<Audio>()->SetMasterGain(SOUND_EFFECT, value.GetFloat()); });

		RegisterParameter(CP_SOUND_AMBIENT, VAR_FLOAT, ST_AUDIO, false, false);
		RegisterReader(CP_SOUND_AMBIENT, [this]() { return GetSubsystem<Audio>()->GetMasterGain(SOUND_AMBIENT); });
		RegisterWriter(CP_SOUND_AMBIENT,
					   [this](const Variant& value)
					   { GetSubsystem<Audio>()->SetMasterGain(SOUND_AMBIENT, value.GetFloat()); });

		RegisterParameter(CP_SOUND_VOICE, VAR_FLOAT, ST_AUDIO, false, false);
		RegisterReader(CP_SOUND_VOICE, [this]() { return GetSubsystem<Audio>()->GetMasterGain(SOUND_VOICE); });
		RegisterWriter(CP_SOUND_VOICE,
					   [this](const Variant& value)
					   { GetSubsystem<Audio>()->SetMasterGain(SOUND_VOICE, value.GetFloat()); });

		RegisterParameter(CP_SOUND_MUSIC, VAR_FLOAT, ST_AUDIO, false, false);
		RegisterReader(CP_SOUND_MUSIC, [this]() { return GetSubsystem<Audio>()->GetMasterGain(SOUND_MUSIC); });
		RegisterWriter(CP_SOUND_MUSIC,
					   [this](const Variant& value)
					   { GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, value.GetFloat()); });
	}

	RegisterSettingsTab(ST_INPUT);
	{
	}
}

static IntVector3 StrToRes(const String& str)
{
	const StringVector firstSplit = str.Split('x', true);
	const StringVector secondSplit = firstSplit[1].Split(':', true);
	return {ToInt(firstSplit[0]), ToInt(secondSplit[0]), ToInt(secondSplit[1])};
}

static String ResToStr(const IntVector3& res) { return ToString("%dx%d:%d", res.x_, res.y_, res.z_); }
