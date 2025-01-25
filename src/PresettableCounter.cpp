/*
	PresettableCounter
	Presettable Up/Down Binary Counter

	Jóhann Berentsson
	January 2025
*/

#include "plugin.hpp"
#include <iostream>
#include "lib/counter.cpp"

/*
	TODO:
	- Find out which triggers and gates can be removed.
	- Finish the presets.
	- Compare it to a real presettable counter.
	- Finish the panel.
*/

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
		ENUMS(COUNTER_OUTPUTS, 5),
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
		ENUMS(OUTPUT_LIGHTS, 5),
		LIGHTS_LEN
	};
	
	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger presetTrigger;
	dsp::SchmittTrigger directionTrigger;
	dsp::SchmittTrigger enableTrigger;

	dsp::PulseGenerator resetPulse;
	dsp::PulseGenerator presetPulse;

	Counter pc;

	bool enabledGate = true;
	bool directionGate = false;
	int n = 32;

	PresettableCounter() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		pc.main(n);

		/* INPUTS */
		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(ENABLE_INPUT, "Enable");
		configInput(PRESET_INPUT, "Preset");
		configInput(DIRECTION_INPUT, "Direction");
		
		std::string PRESET_DESCRIPTIONS[4] = {"A bit", "B bit", "C bit", "D bit"};

		/* PRESET INPUTS */
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

	void process(const ProcessArgs& args) override {
		/* ENABLE */
		if (inputs[ENABLE_INPUT].isConnected()){
			bool enableTriggered = enableTrigger.process(inputs[ENABLE_INPUT].getVoltage(), 0.1f, 2.f);
			enabledGate = enableTrigger.isHigh();
		} else {
			enabledGate = true;
		}

		/* DIRECTION */
		if (inputs[DIRECTION_INPUT].isConnected()){
			bool currentDirectionGate = directionTrigger.process(inputs[DIRECTION_INPUT].getVoltage(), 0.1f, 2.f);
			directionGate = directionTrigger.isHigh();
		} else {
			directionGate = false;
		}

		/* RESET */
		if (inputs[RESET_INPUT].isConnected()){
			bool currentResetTrigger = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
			if (currentResetTrigger) {
				resetPulse.trigger(1e-3f);
				
				if (directionGate){
					pc.setIndex(n-1);
				} else {
					pc.setIndex(0);
				}
			}
		}

		bool resetGate = resetPulse.process(args.sampleTime);

		/* PRESET */
		int presetValue = -1;
		bool presetGate = false;
		
		if (inputs[PRESET_INPUT].isConnected()){
			
			bool currentPresetTrigger = presetTrigger.process(inputs[PRESET_INPUT].getVoltage(), 0.1f, 2.f);
			if (currentPresetTrigger) {
				presetValue = 0;
				for(int i = 0; i < 4; i++){
					if (inputs[PRESET_INPUTS + i].getVoltage() && true){
						presetValue += (2^n);
					}
				}

				presetPulse.trigger(1e-3f);
				presetGate = presetTrigger.isHigh();
				pc.setIndex(presetValue);
			}
		}
		

		/* CLOCK */
		bool clockTriggered = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
		bool clockGate = false;
		double OUTPUT_VALUES[5] = {0};

		if (clockTriggered && !resetGate && enabledGate){
			clockGate = clockTrigger.isHigh();

			if ((presetValue == -1) && !presetGate){
				// Check in what direction we are going.
				if (directionGate && inputs[DIRECTION_INPUT].isConnected()){
					pc.decrement();
				} else {
					pc.increment();
				}
			} else {
				pc.setIndex(presetValue);
			}
		}

		// Set the bit outputs.
		for(int i = 0; i < 5; i++){
			OUTPUT_VALUES[i] = ((pc.index() >> i) & 0x1) * 10.0;
		}

		/* OUTPUTS */
		for (int i = 0; i < 5; i++){
			outputs[COUNTER_OUTPUTS + i].setVoltage(enabledGate ? OUTPUT_VALUES[i] : 0);
			lights[OUTPUT_LIGHTS + i].setBrightness(enabledGate && OUTPUT_VALUES[i] ? true : false);
		}
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

		/* PRESET INPUTS */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 12.31)), module, PresettableCounter::PRESET_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 27.31)), module, PresettableCounter::PRESET_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 37.31)), module, PresettableCounter::PRESET_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 47.31)), module, PresettableCounter::PRESET_INPUTS + 3));
		
		/* CONTROL INPUTS*/
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 57.31)), module, PresettableCounter::PRESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 67.31)), module, PresettableCounter::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 77.31)), module, PresettableCounter::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 87.31)), module, PresettableCounter::DIRECTION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.299, 97.31)), module, PresettableCounter::ENABLE_INPUT));

		/* OUTPUTS */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 12.31)), module, PresettableCounter::COUNTER_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 27.53)), module, PresettableCounter::COUNTER_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 37.53)), module, PresettableCounter::COUNTER_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 47.53)), module, PresettableCounter::COUNTER_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.134, 57.53)), module, PresettableCounter::COUNTER_OUTPUTS + 4));		

		/* LIGHTS */
		/*
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 63.805)), module, PresettableCounter::CLOCK_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 63.805)), module, PresettableCounter::RESET_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 79.007)), module, PresettableCounter::ENABLE_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 79.007)), module, PresettableCounter::PRESET_LIGHT));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(11.027, 94.233)), module, PresettableCounter::PRE_LIGHT));
		*/
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(23.864, 8.233)), module, PresettableCounter::OUTPUT_LIGHTS + 0));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(23.864, 20.393)), module, PresettableCounter::OUTPUT_LIGHTS + 1));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(23.864, 35.393)), module, PresettableCounter::OUTPUT_LIGHTS + 2));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(23.864, 45.393)), module, PresettableCounter::OUTPUT_LIGHTS + 3));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(23.864, 55.393)), module, PresettableCounter::OUTPUT_LIGHTS + 4));
	}
};


Model* modelPresettableCounter = createModel<PresettableCounter, PresettableCounterWidget>("PresettableCounter");