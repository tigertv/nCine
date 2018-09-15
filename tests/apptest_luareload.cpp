﻿#include "apptest_luareload.h"
#include "Application.h"
#include "Texture.h"
#include "ParticleSystem.h"
#include "ParticleInitializer.h"
#include "Timer.h"
#include "LuaStateManager.h"
#include "LuaIAppEventHandler.h"
#include "LuaIInputEventHandler.h"
#include "LuaClassWrapper.h"
#include "LuaUtils.h"
#include "LuaColorUtils.h"
#include "IFile.h" // for dataPath()
#include "apptest_datapath.h"

namespace {

const char *InitScriptFile = "init.lua";
const char *ReloadScriptFile = "reload.lua";

}

nc::IAppEventHandler *createAppEventHandler()
{
	return new MyEventHandler;
}

MyEventHandler::MyEventHandler()
	: luaState_(nc::LuaStateManager::ApiType::EDIT_ONLY,
	            nc::LuaStateManager::StatisticsTracking::DISABLED,
	            nc::LuaStateManager::StandardLibraries::LOADED),
	  variationIndex_(0)
{

}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	setDataPath(config);

	luaState_.exposeScriptApi();
	luaState_.run((config.dataPath() + "scripts/" + InitScriptFile).data());
	nc::LuaIAppEventHandler::onPreInit(luaState_.state(), config);
}

void MyEventHandler::onInit()
{
	lua_State *L = luaState_.state();

	luaState_.run((nc::IFile::dataPath() + "scripts/" + InitScriptFile).data());
	nc::LuaUtils::retrieveGlobalFunction(L, "load");
	nc::LuaUtils::call(L, 0, 1);

	const char *textureFile = nc::LuaUtils::retrieveField<const char *>(L, -1, "texture");
	const unsigned int count = nc::LuaUtils::retrieveField<uint32_t>(L, -1, "count");
	const float xPos = nc::LuaUtils::retrieveField<float>(L, -1, "x");
	const float yPos = nc::LuaUtils::retrieveField<float>(L, -1, "y");

	nc::SceneNode &rootNode = nc::theApplication().rootNode();

	texture_ = nctl::makeUnique<nc::Texture>((nc::IFile::dataPath() + "textures/" + textureFile).data());
	particleSystem_ = nctl::makeUnique<nc::ParticleSystem>(&rootNode, count, texture_.get(), texture_->rect());
	particleSystem_->setPosition(xPos, yPos);

	emitTimer_ = nctl::makeUnique<nc::Timer>();
	emitTimer_->start();

	runScript();
}

void MyEventHandler::onFrameStart()
{
	if (emitTimer_->interval() > 0.085f)
	{
		nc::ParticleInitializer init;
		init.setAmount(16);
		init.setLife(0.85f, 1.0f);
		init.setPositionAndRadius(nc::Vector2f::Zero, 5.0f);
		init.setVelocity(-10.0f, 200.0f, 10.0f, 200.0f);

		emitTimer_->start();
		particleSystem_->emitParticles(init);
	}
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	particleSystem_->x = event.pointers[0].x;
	particleSystem_->y = event.pointers[0].y;
}

void MyEventHandler::onTouchMove(const nc::TouchEvent &event)
{
	particleSystem_->x = event.pointers[0].x;
	particleSystem_->y = event.pointers[0].y;
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::N5)
		runScript();
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
		nc::theApplication().togglePause();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
	{
		particleSystem_->x = static_cast<float>(event.x);
		particleSystem_->y = static_cast<float>(event.y);
	}
}

void MyEventHandler::onMouseMoved(const nc::MouseState &state)
{
	if (state.isLeftButtonDown())
	{
		particleSystem_->x = static_cast<float>(state.x);
		particleSystem_->y = static_cast<float>(state.y);
	}
}

void MyEventHandler::runScript()
{
	variationIndex_++;
	variationIndex_ %= 3;

	lua_State *L = luaState_.state();

	luaState_.run((nc::IFile::dataPath() + "scripts/" + ReloadScriptFile).data());
	nc::LuaUtils::retrieveGlobalFunction(L, "execute");
	nc::LuaUtils::createTable(L, 1, 0);
	nc::LuaClassWrapper<nc::ParticleSystem>::pushFieldUntrackedUserData(L, "particlesys", particleSystem_.get());
	nc::LuaUtils::push(L, variationIndex_);
	nc::LuaUtils::call(L, 2, 0);
}