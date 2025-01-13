#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
extern Model* modelPresettableCounter;


// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

template <typename TBase = GrayModuleLightWidget>
struct YellowBlueLight : TBase {
	YellowBlueLight() {
		this->addBaseColor(SCHEME_YELLOW);
		this->addBaseColor(SCHEME_BLUE);
	}
};