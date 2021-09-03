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
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include "Config.h"
#include "ConfigDefs.h"

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-promo"
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-promo"
#endif // defined(__clang__)

using namespace Urho3D;

void Config::RegisterClientParameters()
{
	Config* config = this;

	config->RegisterSettingsTab(ST_GAME);
	{
		config->RegisterSimpleEnumParameter(
			CP_LANGUAGE,
			VAR_STRING,
			ST_GAME,
			false,
			false,
			[config]() { return config->GetSubsystem<Localization>()->GetLanguage(); },
			[config](const Urho3D::Variant& value)
			{ config->GetSubsystem<Localization>()->SetLanguage(value.GetString()); },
			[config]()
			{
				Localization* l10n = config->GetSubsystem<Localization>();
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

		config->RegisterSimpleEnumParameter(
			EP_LOG_LEVEL,
			VAR_INT,
			ST_GAME,
			true,
			true,
			[config]() { return config->GetSubsystem<Log>()->GetLevel(); },
			[config](const Urho3D::Variant& value) { config->GetSubsystem<Log>()->SetLevel(value.GetInt()); },
			[]() -> EnumVector
			{
				return {{"None", LOG_NONE},
						{"Error", LOG_ERROR},
						{"Warning", LOG_WARNING},
						{"Info", LOG_INFO},
						{"Debug", LOG_DEBUG},
						{"Trace", LOG_TRACE}};
			});
	}

	config->RegisterSettingsTab(ST_VIDEO);
	{
		ComplexParameter* videoMode = config->RegisterBinaryComplexStorage(
			ECP_VIDEO_MODE,
			true,
			[config](const VariantMap& values)
			{
				// TODO: Convenient complex storage values extractor
				VariantMap::ConstIterator it = values.Find(ECP_WINDOW_MODE);
				bool borderless, fullscreen;
				const int windowMode =
					(it != values.End() ? it->second_ : config->parameters_[ECP_WINDOW_MODE]->Read()).GetInt();
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
					(it != values.End() ? it->second_ : config->parameters_[ECP_RESOLUTION]->Read()).GetString());
				const int width = res.x_;
				const int height = res.y_;
				const int refreshRate = res.z_;

				it = values.Find(EP_VSYNC);
				const bool vsync = (it != values.End() ? it->second_ : config->parameters_[EP_VSYNC]->Read()).GetBool();
				it = values.Find(EP_TRIPLE_BUFFER);
				const bool trippleBuffer =
					(it != values.End() ? it->second_ : config->parameters_[EP_TRIPLE_BUFFER]->Read()).GetBool();
				it = values.Find(EP_MULTI_SAMPLE);
				const bool multiSample =
					(it != values.End() ? it->second_ : config->parameters_[EP_MULTI_SAMPLE]->Read()).GetInt();
				it = values.Find(EP_MONITOR);
				const bool monitor =
					(it != values.End() ? it->second_ : config->parameters_[EP_MONITOR]->Read()).GetInt();

				config->GetSubsystem<Graphics>()->SetMode(width,
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

		config->RegisterComplexEnumParameter(
			EP_MONITOR,
			VAR_INT,
			ST_VIDEO,
			true,
			false,
			videoMode,
			[config]() { return config->GetSubsystem<Graphics>()->GetMonitor(); },
			[config]()
			{
				const int monitorCount = config->GetSubsystem<Graphics>()->GetMonitorCount();
				EnumVector ret;
				ret.Reserve(monitorCount);
				for (int i = 0; i < monitorCount; ++i)
					ret.EmplaceBack(ToString("%d", i), i);
				return ret;
			});

		config->RegisterComplexEnumParameter(
			ECP_RESOLUTION,
			VAR_STRING,
			ST_VIDEO,
			true,
			false,
			videoMode,
			[config]()
			{
				const Graphics* graphics = config->GetSubsystem<Graphics>();
				return ResToStr({graphics->GetWidth(), graphics->GetHeight(), graphics->GetRefreshRate()});
			},
			[config]()
			{
				const Graphics* graphics = config->GetSubsystem<Graphics>();
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

		config->RegisterComplexEnumParameter(
			ECP_WINDOW_MODE,
			VAR_INT,
			ST_VIDEO,
			true,
			true,
			videoMode,
			[config]()
			{
				const Graphics* graphics = config->GetSubsystem<Graphics>();
				return graphics->GetBorderless() ? 2 : graphics->GetFullscreen() ? 1 : 0;
			},
			[]() -> EnumVector {
				return {{"Windowed", 0}, {"FullScreen", 1}, {"Borderless", 2}};
			});

		config->RegisterComplexParameter(EP_VSYNC,
										 VAR_BOOL,
										 ST_VIDEO,
										 true,
										 videoMode,
										 [config]() { return config->GetSubsystem<Graphics>()->GetVSync(); });

		config->RegisterComplexParameter(EP_TRIPLE_BUFFER,
										 VAR_BOOL,
										 ST_VIDEO,
										 true,
										 videoMode,
										 [config]() { return config->GetSubsystem<Graphics>()->GetTripleBuffer(); });

		config->RegisterComplexEnumParameter(
			EP_MULTI_SAMPLE,
			VAR_INT,
			ST_VIDEO,
			true,
			false,
			videoMode,
			[config]() { return config->GetSubsystem<Graphics>()->GetMultiSample(); },
			[config]()
			{
				const PODVector<int> msLevels = config->GetSubsystem<Graphics>()->GetMultiSampleLevels();
				EnumVector ret;
				ret.Reserve(msLevels.Size());
				for (int level : msLevels)
					ret.EmplaceBack(ToString("%d", level), level);
				return ret;
			});

		config->RegisterSimpleEnumParameter(
			EP_MATERIAL_QUALITY,
			VAR_INT,
			ST_VIDEO,
			true,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetMaterialQuality(); },
			[config](const Variant& value)
			{ config->GetSubsystem<Renderer>()->SetMaterialQuality(static_cast<MaterialQuality>(value.GetInt())); },
			[]() -> EnumVector
			{
				return {{"Low", MaterialQuality::QUALITY_LOW},
						{"Medium", MaterialQuality::QUALITY_MEDIUM},
						{"High", MaterialQuality::QUALITY_HIGH}};
			});

		config->RegisterSimpleEnumParameter(
			EP_TEXTURE_QUALITY,
			VAR_INT,
			ST_VIDEO,
			true,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetTextureQuality(); },
			[config](const Variant& value)
			{ config->GetSubsystem<Renderer>()->SetTextureQuality(static_cast<MaterialQuality>(value.GetInt())); },
			[]() -> EnumVector
			{
				return {{"Low", MaterialQuality::QUALITY_LOW},
						{"Medium", MaterialQuality::QUALITY_MEDIUM},
						{"High", MaterialQuality::QUALITY_HIGH}};
			});

		config->RegisterSimpleEnumParameter(
			EP_TEXTURE_FILTER_MODE,
			VAR_INT,
			ST_VIDEO,
			true,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetTextureFilterMode(); },
			[config](const Variant& value)
			{ config->GetSubsystem<Renderer>()->SetTextureFilterMode(static_cast<TextureFilterMode>(value.GetInt())); },
			[]() -> EnumVector
			{
				return {{"Nearest", TextureFilterMode::FILTER_NEAREST},
						{"Bilinear", TextureFilterMode::FILTER_BILINEAR},
						{"Trilinear", TextureFilterMode::FILTER_TRILINEAR},
						{"Anisotropic", TextureFilterMode::FILTER_ANISOTROPIC},
						{"NearestAnisotropic", TextureFilterMode::FILTER_NEAREST_ANISOTROPIC}};
			});

		config->RegisterSimpleEnumParameter(
			EP_TEXTURE_ANISOTROPY,
			VAR_INT,
			ST_VIDEO,
			true,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetTextureAnisotropy(); },
			[config](const Variant& value)
			{ config->GetSubsystem<Renderer>()->SetTextureAnisotropy(static_cast<MaterialQuality>(value.GetInt())); },
			[]()
			{
				constexpr int stages[] = {2, 4, 6, 8, 12, 16};
				EnumVector ret;
				ret.Reserve(6);
				for (int stage : stages)
					ret.EmplaceBack(ToString("%d", stage), stage);
				return ret;
			});

		config->RegisterSimpleParameter(
			EP_SHADOWS,
			VAR_BOOL,
			ST_VIDEO,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetDrawShadows(); },
			[config](const Variant& value) { config->GetSubsystem<Renderer>()->SetDrawShadows(value.GetBool()); });

		config->RegisterSimpleEnumParameter(
			CP_SHADOW_QUALITY,
			VAR_INT,
			ST_VIDEO,
			false,
			true,
			[config]() { return config->GetSubsystem<Renderer>()->GetShadowQuality(); },
			[config](const Variant& value)
			{ config->GetSubsystem<Renderer>()->SetShadowQuality(static_cast<ShadowQuality>(value.GetInt())); },
			[]() -> EnumVector
			{
				return {{"Simple16", ShadowQuality::SHADOWQUALITY_SIMPLE_16BIT},
						{"Simple24", ShadowQuality::SHADOWQUALITY_SIMPLE_24BIT},
						{"PCF16", ShadowQuality::SHADOWQUALITY_PCF_16BIT},
						{"PCF24", ShadowQuality::SHADOWQUALITY_PCF_24BIT},
						{"VSM", ShadowQuality::SHADOWQUALITY_VSM},
						{"VSMBlur", ShadowQuality::SHADOWQUALITY_BLUR_VSM}};
			});

		config->RegisterSimpleEnumParameter(
			CP_SHADOW_RESOLUTION,
			VAR_INT,
			ST_VIDEO,
			false,
			false,
			[config]() { return config->GetSubsystem<Renderer>()->GetShadowMapSize(); },
			[config](const Variant& value) { config->GetSubsystem<Renderer>()->SetShadowMapSize(value.GetInt()); },
			[]()
			{
				EnumVector ret;
				ret.Reserve(5);
				String num;
				for (int i = 256; i <= 4096; i *= 2)
				{
					num = ToString("%d", i);
					ret.EmplaceBack(num + "x" + num, i);
				}
				return ret;
			});
	}

	RegisterSettingsTab(ST_AUDIO);
	{
		config->RegisterSimpleParameter(
			CP_SOUND_MASTER,
			VAR_FLOAT,
			ST_AUDIO,
			false,
			[config]() { return config->GetSubsystem<Audio>()->GetMasterGain(SOUND_MASTER); },
			[config](const Variant& value)
			{ config->GetSubsystem<Audio>()->SetMasterGain(SOUND_MASTER, value.GetFloat()); });

		config->RegisterSimpleParameter(
			CP_SOUND_EFFECT,
			VAR_FLOAT,
			ST_AUDIO,
			false,
			[config]() { return config->GetSubsystem<Audio>()->GetMasterGain(SOUND_EFFECT); },
			[config](const Variant& value)
			{ config->GetSubsystem<Audio>()->SetMasterGain(SOUND_EFFECT, value.GetFloat()); });

		config->RegisterSimpleParameter(
			CP_SOUND_AMBIENT,
			VAR_FLOAT,
			ST_AUDIO,
			false,
			[config]() { return config->GetSubsystem<Audio>()->GetMasterGain(SOUND_AMBIENT); },
			[config](const Variant& value)
			{ config->GetSubsystem<Audio>()->SetMasterGain(SOUND_AMBIENT, value.GetFloat()); });

		config->RegisterSimpleParameter(
			CP_SOUND_VOICE,
			VAR_FLOAT,
			ST_AUDIO,
			false,
			[config]() { return config->GetSubsystem<Audio>()->GetMasterGain(SOUND_VOICE); },
			[config](const Variant& value)
			{ config->GetSubsystem<Audio>()->SetMasterGain(SOUND_VOICE, value.GetFloat()); });

		config->RegisterSimpleParameter(
			CP_SOUND_MUSIC,
			VAR_FLOAT,
			ST_AUDIO,
			false,
			[config]() { return config->GetSubsystem<Audio>()->GetMasterGain(SOUND_MUSIC); },
			[config](const Variant& value)
			{ config->GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, value.GetFloat()); });
	}

	RegisterSettingsTab(ST_INPUT);
	{
	}
}

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif // defined (__GNUC__) || defined(__GNUG__)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)

void Config::RegisterServerParameters() {}

Urho3D::IntVector3 Config::StrToRes(const Urho3D::String& str)
{
	const StringVector firstSplit = str.Split('x', true);
	const StringVector secondSplit = firstSplit[1].Split(':', true);
	return {ToInt(firstSplit[0]), ToInt(secondSplit[0]), ToInt(secondSplit[1])};
}

Urho3D::String Config::ResToStr(const Urho3D::IntVector3& res) { return ToString("%dx%d:%d", res.x_, res.y_, res.z_); }
