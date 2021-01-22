#include "scene.h"
#include "imgui.h"

#include "framework.h"

#include "game_scene.h"
#include "debug_scene.h"
#include "Title.h"

void SceneManager::initialize()
{
	// TITLE
	{
		scene_names.push_back("TITLE");
		std::string scene_name = scene_names.rbegin()->c_str();
		scenes.insert(std::make_pair(scene_name, std::make_shared<TitleScene>()));
	}
	//GAME
	{
		scene_names.push_back("GAME");
		std::string scene_name = scene_names.rbegin()->c_str();
		scenes.insert(std::make_pair(scene_name, std::make_shared<GameScene>()));
	}

	//DEBUG
	{
		scene_names.push_back("DEBUG");
		std::string scene_name = scene_names.rbegin()->c_str();
		scenes.insert(std::make_pair(scene_name, std::make_shared<DebugScene>()));
	}

	current_scene = "GAME";

	scenes.at(current_scene)->initialize();
}

void SceneManager::update()
{
	if (next_scene)
	{
		scenes.at(current_scene)->uninitialize();
		current_scene = next_scene;
		next_scene = nullptr;
		scenes.at(current_scene)->initialize();
	}
	scenes.at(current_scene)->update();
}

void SceneManager::render()
{
	scenes.at(current_scene)->render();
}

void SceneManager::change_scene(const char* next_scene)
{
	if (!scenes.at(next_scene)) return;
	this->next_scene = next_scene;
}