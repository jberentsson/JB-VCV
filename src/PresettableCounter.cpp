#include "plugin.hpp"

struct PresettableCounter : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		ENABLE_INPUT,
		PRESET_INPUT,
		PRE_A_INPUT,
		PRE_B_INPUT,
		PRE_C_INPUT,
		PRE_D_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		A_OUTPUT,
		B_OUTPUT,
		C_OUTPUT,
		D_OUTPUT,
		CARRY_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(CLOCK_LIGHT, 2),
		ENUMS(RESET_LIGHT, 2),
		ENUMS(ENABLE_LIGHT, 2),
		ENUMS(PRESET_LIGHT, 2),
		ENUMS(PRE_LIGHT, 2),
		ENUMS(A_OUT_LIGHT, 2),
		ENUMS(B_OUT_LIGHT, 2),
		ENUMS(C_OUT_LIGHT, 2),
		ENUMS(D_OUT_LIGHT, 2),
		LIGHTS_LEN
	};

	PresettableCounter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		/* INPUTS */
		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(ENABLE_INPUT, "Enable");
		configInput(PRESET_INPUT, "Preset");

		configInput(PRE_A_INPUT, "A bit");
		configInput(PRE_B_INPUT, "B bit");
		configInput(PRE_C_INPUT, "C bit");
		configInput(PRE_D_INPUT, "D bit");

		/* INPUTS */
		configOutput(A_OUTPUT, "A bit");
		configOutput(B_OUTPUT, "B bit");
		configOutput(C_OUTPUT, "C bit");
		configOutput(D_OUTPUT, "D bit");
		configOutput(CARRY_OUTPUT, "Carry");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct PresettableCounterWidget : ModuleWidget {
	PresettableCounterWidget(PresettableCounter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PresettableCounter.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		/* INPUTS */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 12.31)), module, PresettableCounter::PRE_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 27.31)), module, PresettableCounter::PRE_B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 37.31)), module, PresettableCounter::PRE_C_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 47.31)), module, PresettableCounter::PRE_D_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 57.31)), module, PresettableCounter::PRESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 67.31)), module, PresettableCounter::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 77.31)), module, PresettableCounter::CLOCK_INPUT));

		/* OUTPUTS */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 17.53)), module, PresettableCounter::A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 27.53)), module, PresettableCounter::B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 37.53)), module, PresettableCounter::C_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 47.53)), module, PresettableCounter::D_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 57.53)), module, PresettableCounter::CARRY_OUTPUT));		
	}
};


Model* modelPresettableCounter = createModel<PresettableCounter, PresettableCounterWidget>("PresettableCounter");