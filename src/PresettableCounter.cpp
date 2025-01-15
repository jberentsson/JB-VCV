#include "plugin.hpp"
#include <iostream>

struct PresettableCounter : Module {
	enum ParamId {
		PARAMS_LEN
	};

	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		ENABLE_INPUT,
		PRESET_INPUT,
		DIRECTION_INPUT,
		ENUMS(PRESET_INPUTS, 4),
		INPUTS_LEN
	};
	
	enum OutputId {
		COUNTER_OUTPUTS_A,
		COUNTER_OUTPUTS_B,
		COUNTER_OUTPUTS_C,
		COUNTER_OUTPUTS_D,
		COUNTER_OUTPUTS_CARRY,
		OUTPUTS_LEN
	};

	enum LightId {
		CLOCK_LIGHT,
		RESET_LIGHT,
		ENABLE_LIGHT,
		PRESET_LIGHT,
		PRE_LIGHT,
		A_OUT_LIGHT,
		B_OUT_LIGHT,
		C_OUT_LIGHT,
		D_OUT_LIGHT,
		LIGHTS_LEN
	};
	
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger presetTrigger;
	dsp::SchmittTrigger directionTrigger;
	dsp::SchmittTrigger enableTrigger;

	dsp::PulseGenerator clockPulse;
	dsp::PulseGenerator resetPulse;
	dsp::PulseGenerator presetPulse;
	dsp::PulseGenerator directionPulse;
	dsp::PulseGenerator enablePulse;

	int index = 0;
	bool statusReset = false;
	bool statusClock = false;
	bool statusPreset = false;
	bool statusEnable = false;
	bool statusDirection = false;

	PresettableCounter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		/* INPUTS */
		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(ENABLE_INPUT, "Enable");
		configInput(PRESET_INPUT, "Preset");
		configInput(DIRECTION_INPUT, "Direction");
		
		std::string PRESET_DESCRIPTIONS[4] = {"A bit", "B bit", "C bit", "D bit"};

		for (int i = 0; i < 4; i++) {
			configInput(PRESET_INPUTS + i, PRESET_DESCRIPTIONS[i]);
		}

		/* OUTPUTS */
		configOutput(COUNTER_OUTPUTS_A, "A bit");
		configOutput(COUNTER_OUTPUTS_B, "B bit");
		configOutput(COUNTER_OUTPUTS_C, "C bit");
		configOutput(COUNTER_OUTPUTS_D, "D bit");
		configOutput(COUNTER_OUTPUTS_CARRY, "Carry");
	}

	void updateOutputs(int a, int b, int c, int d, int carry){
		outputs[COUNTER_OUTPUTS_A].setVoltage(a);
		outputs[COUNTER_OUTPUTS_B].setVoltage(b);
		outputs[COUNTER_OUTPUTS_C].setVoltage(c);
		outputs[COUNTER_OUTPUTS_D].setVoltage(d);
		outputs[COUNTER_OUTPUTS_CARRY].setVoltage(carry);
	}

	void outputsOff(){
		updateOutputs(0, 0, 0, 0, 0);
	}

	void reset(){
		index = 0;
		outputsOff();
	}

	bool getClockTrigger(){
		return clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
	}
	bool getResetTrigger(){
		return resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
	}

	bool getEnableTrigger(){
		return enableTrigger.process(inputs[ENABLE_INPUT].getVoltage(), 0.1f, 2.f);
	}

	bool getPresetTrigger(){
		return presetTrigger.process(inputs[PRESET_INPUT].getVoltage(), 0.1f, 2.f);
	}

	void intToOutput(int index ){
		double OUTPUT_VALUES[5] = {
			((index >> 0) & 0x1) * 10.0,
			((index >> 1) & 0x1) * 10.0,
			((index >> 2) & 0x1) * 10.0,
			((index >> 3) & 0x1) * 10.0,
			((index >> 4) & 0x1) * 10.0
		};

		// UPDATE THE OUTPUTS
		outputs[COUNTER_OUTPUTS_A].setVoltage(OUTPUT_VALUES[0]);
		outputs[COUNTER_OUTPUTS_B].setVoltage(OUTPUT_VALUES[1]);
		outputs[COUNTER_OUTPUTS_C].setVoltage(OUTPUT_VALUES[2]);
		outputs[COUNTER_OUTPUTS_D].setVoltage(OUTPUT_VALUES[3]);
		outputs[COUNTER_OUTPUTS_CARRY].setVoltage(OUTPUT_VALUES[4]);
	}

	void updateInputs(){
		/* DEBUG */
		bool clockTriggered = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
		bool resetTriggered = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
		bool enableTriggered = enableTrigger.process(inputs[ENABLE_INPUT].getVoltage(), 0.1f, 2.f);
		bool presetTriggered = presetTrigger.process(inputs[PRESET_INPUT].getVoltage(), 0.1f, 2.f);
		bool directionTriggered = presetTrigger.process(inputs[DIRECTION_INPUT].getVoltage(), 0.1f, 2.f);;

		if (statusClock != clockTriggered){
			if (clockTriggered){
				std::cout << "Clock Trigger" << std::endl;
			}

			statusClock = !statusClock;
		}
		
		if (statusReset != resetTriggered){
			if(resetTriggered){
				std::cout << "Reset Trigger" << std::endl;
			}

			statusReset = !statusReset;
		}
		
		if (statusEnable != enableTriggered){
			if (enableTriggered){
				std::cout << "Enable Trigger" << std::endl;
			}

			statusEnable = !statusEnable;
		}
		
		if (statusPreset != presetTriggered){
			if(presetTriggered){
				std::cout << "Preset Trigger" << std::endl;
			}

			statusPreset = !statusPreset;
		}
		
		if (statusDirection != directionTriggered){
			if(presetTriggered){
				std::cout << "Direction Trigger" << std::endl;
			}

			statusDirection = !statusDirection;
		}

		// Make true if no cable is connected to the enable input.
		if (!inputs[ENABLE_INPUT].isConnected()){
			statusEnable = true;
		}
	}

	void process(const ProcessArgs& args) override {
		bool currentClockTrigger = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
		bool currentResetTrigger = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
		bool currentDirectionGate = directionTrigger.process(inputs[DIRECTION_INPUT].getVoltage(), 0.1f, 2.f);

		if (currentDirectionGate){
			// Do something if direction is on.
			if(currentDirectionGate != statusDirection){
				if (currentDirectionGate){
					//index--;
				} else {
					index++;
				}
			}
		}

		if (currentResetTrigger) {
			if (statusReset != currentResetTrigger){
				index = 0;
			}
		}

		if (currentClockTrigger){
			if (statusClock != currentClockTrigger) {
				double OUTPUT_VALUES[5] = {
					((index >> 0) & 0x1) * 10.0,
					((index >> 1) & 0x1) * 10.0,
					((index >> 2) & 0x1) * 10.0,
					((index >> 3) & 0x1) * 10.0,
					((index >> 4) & 0x1) * 10.0
				};

				// UPDATE THE OUTPUTS
				outputs[COUNTER_OUTPUTS_A].setVoltage(OUTPUT_VALUES[0]);
				outputs[COUNTER_OUTPUTS_B].setVoltage(OUTPUT_VALUES[1]);
				outputs[COUNTER_OUTPUTS_C].setVoltage(OUTPUT_VALUES[2]);
				outputs[COUNTER_OUTPUTS_D].setVoltage(OUTPUT_VALUES[3]);
				outputs[COUNTER_OUTPUTS_CARRY].setVoltage(OUTPUT_VALUES[4]);

				if (!statusDirection){
					index++;
				} else {
					index--;
				}
			}
		}

		updateInputs();
	}
};


struct PresettableCounterWidget : ModuleWidget {
	PresettableCounterWidget(PresettableCounter* module) {
		setModule(module);
		
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PresettableCounter.svg")));
		
		/* SCREWS */
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		/* INPUTS */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 12.31)), module, PresettableCounter::PRESET_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 27.31)), module, PresettableCounter::PRESET_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 37.31)), module, PresettableCounter::PRESET_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 47.31)), module, PresettableCounter::PRESET_INPUTS + 3));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 57.31)), module, PresettableCounter::PRESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 67.31)), module, PresettableCounter::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 77.31)), module, PresettableCounter::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 87.31)), module, PresettableCounter::DIRECTION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 97.31)), module, PresettableCounter::ENABLE_INPUT));

		/* OUTPUTS */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 12.31)), module, PresettableCounter::COUNTER_OUTPUTS_A));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 27.53)), module, PresettableCounter::COUNTER_OUTPUTS_B));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 37.53)), module, PresettableCounter::COUNTER_OUTPUTS_C));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 47.53)), module, PresettableCounter::COUNTER_OUTPUTS_D));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 57.53)), module, PresettableCounter::COUNTER_OUTPUTS_CARRY));		

		/* LIGHTS */
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 63.805)), module, PresettableCounter::CLOCK_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 63.805)), module, PresettableCounter::RESET_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 79.007)), module, PresettableCounter::ENABLE_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 79.007)), module, PresettableCounter::PRESET_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 94.233)), module, PresettableCounter::PRE_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 94.233)), module, PresettableCounter::C_OUT_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 109.393)), module, PresettableCounter::D_OUT_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 109.393)), module, PresettableCounter::LIGHTS_LEN));
	}
};


Model* modelPresettableCounter = createModel<PresettableCounter, PresettableCounterWidget>("PresettableCounter");