/*
	BinaryCounter
	Eight bit binary counter.
	Jóhann Berentsson
	January 2025
*/

#include "plugin.hpp"
#include <iostream>
#include "lib/counter.cpp"

struct BinaryCounter : Module {
	enum ParamId {
		PARAMS_LEN
	};
	
	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		ENUMS(COUNTER_OUTPUTS, 8),
		OUTPUTS_LEN
	};

	enum LightId {
		LIGHTS_LEN
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;

	dsp::PulseGenerator resetPulse;

	Counter pc;
	int n = 256;

	BinaryCounter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	
		pc.main(n);

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");

		for(int i = 0; i < 8; i++){
			configOutput(COUNTER_OUTPUTS + i, string::f("%d bit", i));
		}
	}
	
	void process(const ProcessArgs& args) override {
		/* RESET */
		if (inputs[RESET_INPUT].isConnected()){
			bool currentResetTrigger = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
			if (currentResetTrigger) {
				resetPulse.trigger(1e-3f);
				pc.setIndex(0);
			}
		}

		bool resetGate = resetPulse.process(args.sampleTime);
		
		/* CLOCK */
		bool clockTriggered = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
		bool clockGate = false;
		double OUTPUT_VALUES[8] = {0};

		if (clockTriggered && !resetGate){
			clockGate = clockTrigger.isHigh();
			pc.increment();
		}

		/* OUTPUTS */
		for(int i = 0; i < 8; i++){
			OUTPUT_VALUES[i] = ((pc.index() >> i) & 0x1) * 10.0;
			outputs[COUNTER_OUTPUTS + i].setVoltage(OUTPUT_VALUES[i]);
		}
	}
};

struct BinaryCounterWidget : ModuleWidget {
	BinaryCounterWidget(BinaryCounter* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/BinaryCounter.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		/* INPUTS */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 12.31)), module, BinaryCounter::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 22.31)), module, BinaryCounter::RESET_INPUT));

		/* OUTPUTS */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 12.31)), module, BinaryCounter::COUNTER_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 22.31)), module, BinaryCounter::COUNTER_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 32.31)), module, BinaryCounter::COUNTER_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 42.31)), module, BinaryCounter::COUNTER_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 52.31)), module, BinaryCounter::COUNTER_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 62.31)), module, BinaryCounter::COUNTER_OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 72.31)), module, BinaryCounter::COUNTER_OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 82.31)), module, BinaryCounter::COUNTER_OUTPUTS + 7));
	}
};


Model* modelBinaryCounter = createModel<BinaryCounter, BinaryCounterWidget>("BinaryCounter");