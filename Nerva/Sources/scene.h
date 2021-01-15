#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <d3d11.h>

class Scene
{
public:
	virtual void initialize() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void uninitialize() = 0;
	virtual ~Scene() {};
	virtual LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return 0; }
};

class SceneManager
{
public:
	std::map<std::string, std::shared_ptr<Scene>> scenes;
	std::string current_scene;
	const char* next_scene = nullptr;

	std::vector<std::string> scene_names;

private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& instance()
	{
		static SceneManager inst;
		return inst;
	}

	void initialize();
	void update();
	void render();
	void change_scene(const char* next_scene);
};