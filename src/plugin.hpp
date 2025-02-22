#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
extern Model* modelPresettableCounter;
extern Model* model_8Steps;
extern Model* modelBinaryCounter;
extern Model* modelBin2Oct;


// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

template <typename TBase = GrayModuleLightWidget>
struct YellowBlueLight : TBase {
	YellowBlueLight() {
		this->addBaseColor(SCHEME_YELLOW);
	}
};