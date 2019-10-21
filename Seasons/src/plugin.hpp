#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelSeasons;

// COMPONENTS
struct sa_GreenKnobBig : SvgKnob {
	sa_GreenKnobBig() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_GreenKnob.svg")) );
	}
};

struct sa_GreenKnobSmall : SvgKnob {
	sa_GreenKnobSmall() {
        minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_GreenKnobSmall.svg")) );
	}
};

struct sa_GreenPushButton : SvgSwitch {
	sa_GreenPushButton() {
		momentary = true;
		addFrame( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_GreeenPushButton.svg")) );
		addFrame( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_GreeenPushButton_pressed.svg")) );
	}
};

struct sa_SeasonsModeLEDS : SvgSwitch {
	sa_SeasonsModeLEDS() {
		addFrame( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_SeasonsMode_1.svg")) );
		addFrame( APP->window->loadSvg(asset::plugin(pluginInstance, "res/SA_SeasonsMode_2.svg")) );
	}
};