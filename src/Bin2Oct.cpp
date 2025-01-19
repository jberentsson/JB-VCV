/*
	Bin2Oct
	Binary number to octal.
	Jóhann Berentsson
	January 2025
*/

#include <iostream>
#include "plugin.hpp"
#include "lib/counter.cpp"


struct Bin2Oct : Module {
	enum ParamId {
		BPUSH_PARAM,
		PARAMS_LEN
	};

	enum InputId {
		ENUMS(BIT_INPUTS, 4),
		INPUTS_LEN
	};

	enum OutputId {
		ENUMS(OCTAL_OUTPUTS, 8),
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	dsp::SchmittTrigger enableTrigger;

	Counter pc;

	bool enabledGate = true;
	bool directionGate = false;
	int n = 32;

	Bin2Oct() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		for (int i = 0; i < 4; i++){
			configInput(BIT_INPUTS + i, string::f("%d",i));
		}

		for (int i = 0; i < 8; i++){
			configOutput(OCTAL_OUTPUTS + i, string::f("%d",i));
		}
	}

	void process(const ProcessArgs& args) override {
		/* ENABLE */
		if (inputs[BIT_INPUTS + 3].isConnected()){
			bool enableTriggered = enableTrigger.process(inputs[BIT_INPUTS + 3].getVoltage(), 0.1f, 2.f);
			enabledGate = enableTrigger.isHigh();
		} else {
			enabledGate = true;
		}

		int index = ((inputs[BIT_INPUTS + 0].getVoltage() && 1) * 1) + ((inputs[BIT_INPUTS + 1].getVoltage() && 1) * 2) + ((inputs[BIT_INPUTS + 2].getVoltage() && 1) * 4);

		/* OUTPUTS */
		for(int i = 0; i < 8; i++){
			outputs[OCTAL_OUTPUTS + i].setVoltage(((i == index) && enabledGate)* 10.0);
		}
	}
};


struct Bin2OctWidget : ModuleWidget {
	Bin2OctWidget(Bin2Oct* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Bin2Oct.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 12.31)), module, Bin2Oct::BIT_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 22.31)), module, Bin2Oct::BIT_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 32.31)), module, Bin2Oct::BIT_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 42.31)), module, Bin2Oct::BIT_INPUTS + 3));
		
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 22.53)), module, Bin2Oct::OCTAL_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 32.53)), module, Bin2Oct::OCTAL_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 42.732)), module, Bin2Oct::OCTAL_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 52.732)), module, Bin2Oct::OCTAL_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 62.958)), module, Bin2Oct::OCTAL_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 72.958)), module, Bin2Oct::OCTAL_OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 82.115)), module, Bin2Oct::OCTAL_OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.297, 92.115)), module, Bin2Oct::OCTAL_OUTPUTS + 7));
	}
};


Model* modelBin2Oct = createModel<Bin2Oct, Bin2OctWidget>("Bin2Oct");