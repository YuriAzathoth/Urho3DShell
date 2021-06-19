#ifndef SHELLAPPLICATION_H
#define SHELLAPPLICATION_H

#include <Urho3D/Engine/Application.h>

class ShellApplication : public Urho3D::Application
{
public:
	using Urho3D::Application::Application;
	void Setup() override;
	void Start() override;
	void Stop() override;
};

#endif // SHELLAPPLICATION_H
