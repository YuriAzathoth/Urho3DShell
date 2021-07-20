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
#include <Urho3D/Resource/Localization.h>
#include "Config.h"
#include "ConfigDefs.h"

using namespace Urho3D;

void Config::RegisterClientParameters()
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
					(it != values.End() ? it->second_ : parameters_[ECP_RESOLUTION].reader_->Read()).GetString());
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
		RegisterWriter(EP_MATERIAL_QUALITY,
					   [this](const Variant& value)
					   { GetSubsystem<Renderer>()->SetMaterialQuality(static_cast<MaterialQuality>(value.GetInt())); });
		RegisterEnum(EP_MATERIAL_QUALITY,
					 [this]() -> EnumVector
					 {
						 return {{"Low", MaterialQuality::QUALITY_LOW},
								 {"Medium", MaterialQuality::QUALITY_MEDIUM},
								 {"High", MaterialQuality::QUALITY_HIGH}};
					 });

		RegisterParameter(EP_TEXTURE_QUALITY, VAR_INT, ST_VIDEO, true, true);
		RegisterReader(EP_TEXTURE_QUALITY, [this]() { return GetSubsystem<Renderer>()->GetTextureQuality(); });
		RegisterWriter(EP_TEXTURE_QUALITY,
					   [this](const Variant& value)
					   { GetSubsystem<Renderer>()->SetTextureQuality(static_cast<MaterialQuality>(value.GetInt())); });
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
			{ GetSubsystem<Renderer>()->SetTextureFilterMode(static_cast<TextureFilterMode>(value.GetInt())); });
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
		RegisterWriter(EP_TEXTURE_ANISOTROPY,
					   [this](const Variant& value) {
						   GetSubsystem<Renderer>()->SetTextureAnisotropy(static_cast<MaterialQuality>(value.GetInt()));
					   });
		RegisterEnum(EP_TEXTURE_ANISOTROPY,
					 [this]() -> EnumVector
					 {
						 constexpr int stages[] = {2, 4, 6, 8, 12, 16};
						 EnumVector ret;
						 ret.Reserve(6);
						 for (int stage : stages)
							 ret.EmplaceBack(ToString("%d", stage), stage);
						 return ret;
					 });

		RegisterParameter(EP_SHADOWS, VAR_BOOL, ST_VIDEO, true, false);
		RegisterReader(EP_SHADOWS, [this]() { return GetSubsystem<Renderer>()->GetDrawShadows(); });
		RegisterWriter(EP_SHADOWS,
					   [this](const Variant& value) { GetSubsystem<Renderer>()->SetDrawShadows(value.GetBool()); });

		RegisterParameter(CP_SHADOW_QUALITY, VAR_INT, ST_VIDEO, false, false);
		RegisterReader(CP_SHADOW_QUALITY, [this]() { return GetSubsystem<Renderer>()->GetShadowQuality(); });
		RegisterWriter(CP_SHADOW_QUALITY,
					   [this](const Variant& value)
					   { GetSubsystem<Renderer>()->SetShadowQuality(static_cast<ShadowQuality>(value.GetInt())); });
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
		RegisterWriter(CP_SHADOW_RESOLUTION,
					   [this](const Variant& value)
					   { GetSubsystem<Renderer>()->SetShadowMapSize(static_cast<ShadowQuality>(value.GetInt())); });
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

void Config::RegisterServerParameters() {}

Urho3D::IntVector3 Config::StrToRes(const Urho3D::String& str)
{
	const StringVector firstSplit = str.Split('x', true);
	const StringVector secondSplit = firstSplit[1].Split(':', true);
	return {ToInt(firstSplit[0]), ToInt(secondSplit[0]), ToInt(secondSplit[1])};
}

Urho3D::String Config::ResToStr(const Urho3D::IntVector3& res) { return ToString("%dx%d:%d", res.x_, res.y_, res.z_); }
