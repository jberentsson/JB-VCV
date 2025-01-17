#include "plugin.hpp"
#include <iostream>
#include "lib/StepSequencer.cpp"

struct _8Steps : Module {
	enum ParamId {
		ENUMS(CV_PARAMS, 32),
		PARAMS_LEN
	};

	enum InputId {
		ENUMS(BIT_A_INPUT, 4),
		ENUMS(BIT_B_INPUT, 4),
		ENUMS(BIT_C_INPUT, 4),
		ENUMS(ENABLE_INPUT, 4),
		INPUTS_LEN
	};

	enum OutputId {
		ENUMS(SEQUENCER_OUTPUT, 4),
		ENUMS(CV_OUTPUTS, 4),
		OUTPUTS_LEN
	};

	enum LightId {
		ENUMS(STEP_LIGHT, 8),
		LIGHTS_LEN
	};

	dsp::ClockDivider lightDivider;

	int COUNTER = 0;
	bool DISABLED = false;
	int LAST_X = -1;

	_8Steps() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		/* INPUTS */
		std::string inputLabels[4] = {"Bit A", "Bit B", "Bit C", "Enable"};

		for(int j = 0; j < 4; j++){
			configInput(BIT_A_INPUT + j, inputLabels[j]);
			configInput(BIT_B_INPUT + j, inputLabels[j]);
			configInput(BIT_C_INPUT + j, inputLabels[j]);
			configInput(ENABLE_INPUT + j, inputLabels[j]);
		}

		/* CV */
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 8; i++) {
				configParam(CV_PARAMS + 8 * j + i, -10.f, 10.f, 0.f, string::f("CV %d step %d", j + 1, i + 1), " V");
			}
		}

		/* OUTPUTS */
		for (int i = 0; i < 4; i++){
			configOutput(CV_OUTPUTS + i, std::string("CV %d", i));
		}
	}

	int bits2dec(bool a, bool b, bool c, bool d){
		return (1*a) + (2*b) + (4*c) + (8*d);
	}

	void process(const ProcessArgs& args) override {
		float lightTime = args.sampleTime * lightDivider.getDivision();

		/* INPUTS */
		bool a = inputs[BIT_A_INPUT].getVoltage();
		bool b = inputs[BIT_B_INPUT].getVoltage();
		bool c = inputs[BIT_C_INPUT].getVoltage();
		bool d = inputs[ENABLE_INPUT].getVoltage();

		int x = bits2dec(a, b, c, d);

		if (!inputs[ENABLE_INPUT].isConnected()) {
			d = !d;
		}

		// Lights
		for (int i = 0; i < 8; i++) {
			if(i == x && d){
				lights[STEP_LIGHT + x].setBrightness(true);
			}else{
				lights[STEP_LIGHT + i].setBrightness(false);
			}
		}
		
		outputs[CV_OUTPUTS + 0].setVoltage(params[CV_PARAMS + 8 * 0 + x].getValue());
		outputs[CV_OUTPUTS + 1].setVoltage(params[CV_PARAMS + 8 * 1 + x].getValue());
		outputs[CV_OUTPUTS + 2].setVoltage(params[CV_PARAMS + 8 * 2 + x].getValue());
		outputs[CV_OUTPUTS + 3].setVoltage(params[CV_PARAMS + 8 * 3 + x].getValue());
	}
};


struct _8StepsWidget : ModuleWidget {
	_8StepsWidget(_8Steps* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/8Steps.svg")));

		/* SCREWS */
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		/* BIT INPUTS */

		/* ROW 0 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.297, 107.53)), module, _8Steps::BIT_A_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(27.297, 107.53)), module, _8Steps::BIT_B_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.297, 107.53)), module, _8Steps::BIT_C_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(47.297, 107.53)), module, _8Steps::ENABLE_INPUT + 0));

		/* ROW 1 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.297, 117.53)), module, _8Steps::BIT_A_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(27.297, 117.53)), module, _8Steps::BIT_B_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.297, 117.53)), module, _8Steps::BIT_C_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(47.297, 117.53)), module, _8Steps::ENABLE_INPUT + 1));

		/* ROW 2 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67.297, 107.53)), module, _8Steps::BIT_A_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(77.297, 107.53)), module, _8Steps::BIT_B_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(87.297, 107.53)), module, _8Steps::BIT_C_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97.297, 107.53)), module, _8Steps::ENABLE_INPUT + 2));

		/* ROW 3 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(67.297, 117.53)), module, _8Steps::BIT_A_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(77.297, 117.53)), module, _8Steps::BIT_B_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(87.297, 117.53)), module, _8Steps::BIT_C_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97.297, 117.53)), module, _8Steps::ENABLE_INPUT + 3));

		/* CV Outputs */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117.297, 107.53)), module, _8Steps::CV_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(127.297, 107.53)), module, _8Steps::CV_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117.297, 117.53)), module, _8Steps::CV_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(127.297, 117.53)), module, _8Steps::CV_OUTPUTS + 3));

		/* LIGHTS */
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 8; j++){
				addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864 + (20*j), 15.393 + (20 * i))), module, _8Steps::STEP_LIGHT + j));
			}
		}
		
		/* KNOBS ROW 0 */
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 8; j++){
				addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351 + (20 * j), 24.048 + (20 * i))), module, _8Steps::CV_PARAMS + ((8 * i) + j)));
			}
		}
	}
};


Model* model_8Steps = createModel<_8Steps, _8StepsWidget>("8Steps");