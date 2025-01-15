#include "plugin.hpp"
#include <iostream>


struct _8Steps : Module {
	enum ParamId {
		ENUMS(CV_PARAMS, 32),
		PARAMS_LEN
	};

	enum InputId {
		BIT_A_INPUT,
		BIT_B_INPUT,
		BIT_C_INPUT,
		ENABLE_INPUT,
		INPUTS_LEN
	};

	enum OutputId {
		ENUMS(SEQUENCER_OUTPUT, 4),
		ENUMS(CV_OUTPUTS, 4),
		OUTPUTS_LEN
	};

	enum LightId {
		ENUMS(STEP_LIGHT, 9),
		LIGHTS_LEN
	};

	dsp::ClockDivider lightDivider;

	int COUNTER = 0;
	bool DISABLED = false;
	int LAST_X = -1;

	_8Steps() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configInput(BIT_A_INPUT, "Bit A");
		configInput(BIT_B_INPUT, "Bit B");
		configInput(BIT_C_INPUT, "Bit C");
		configInput(ENABLE_INPUT, "Enable");

		/* CV */
		for (int j = 0; j < 3; j++) {
			for (int i = 0; i < 8; i++) {
				configParam(CV_PARAMS + 8 * j + i, -10.f, 10.f, 0.f, string::f("CV %d step %d", j + 1, i + 1), " V");
			}
		}

		/* OUTPUTS */
		configOutput(CV_OUTPUTS + 0, "CV 0");
		configOutput(CV_OUTPUTS + 1, "CV 1");
		configOutput(CV_OUTPUTS + 2, "CV 2");
		configOutput(CV_OUTPUTS + 3, "CV 3");
	}

	int bits2dec(bool a, bool b, bool c, bool d){
		int x = 0;

		if (a){
			x += 1;
		}
		
		if (b){
			x += 2;
		}

		if (c){
			x += 4;
		}

		return x;
	}

	void process(const ProcessArgs& args) override {
		float lightTime = args.sampleTime * lightDivider.getDivision();

		bool a = inputs[BIT_A_INPUT].getVoltage();
		bool b = inputs[BIT_B_INPUT].getVoltage();
		bool c = inputs[BIT_C_INPUT].getVoltage();
		bool d = inputs[ENABLE_INPUT].getVoltage();

		int x = bits2dec(a, b, c, d);

		if (!inputs[ENABLE_INPUT].isConnected()) {
			d = !d;
		}

		for (int i = 0; i < 8; i++) {
			lights[STEP_LIGHT + i].setBrightness(false);
		}

		if(d){
			// Step outputs
			lights[STEP_LIGHT + ((x - 1) % 8)].setBrightness(false);
			lights[STEP_LIGHT + x].setBrightness(true);
		}
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
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 57.53)), module, _8Steps::BIT_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 67.53)), module, _8Steps::BIT_B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 77.53)), module, _8Steps::BIT_C_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.297, 87.53)), module, _8Steps::ENABLE_INPUT));

		/* LIGHTS */
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(21.864, 15.393)), module, _8Steps::STEP_LIGHT + 0));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(41.864, 15.393)), module, _8Steps::STEP_LIGHT + 1));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(61.864, 15.393)), module, _8Steps::STEP_LIGHT + 2));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(81.864, 15.393)), module, _8Steps::STEP_LIGHT + 3));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(101.864, 15.393)), module, _8Steps::STEP_LIGHT + 4));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(121.864, 15.393)), module, _8Steps::STEP_LIGHT + 5));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(141.864, 15.393)), module, _8Steps::STEP_LIGHT + 6));
		addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(161.864, 15.393)), module, _8Steps::STEP_LIGHT + 7));
		
		/* KNOBS ROW 0 */
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 24.048)), module, _8Steps::CV_PARAMS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.351, 24.048)), module, _8Steps::CV_PARAMS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.351, 24.048)), module, _8Steps::CV_PARAMS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.351, 24.048)), module, _8Steps::CV_PARAMS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.351, 24.048)), module, _8Steps::CV_PARAMS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(120.351, 24.048)), module, _8Steps::CV_PARAMS + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(140.351, 24.048)), module, _8Steps::CV_PARAMS + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(160.351, 24.048)), module, _8Steps::CV_PARAMS + 7));

		/* KNOBS ROW 1 */
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 44.048)), module, _8Steps::CV_PARAMS + 8));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.351, 44.048)), module, _8Steps::CV_PARAMS + 9));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.351, 44.048)), module, _8Steps::CV_PARAMS + 10));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.351, 44.048)), module, _8Steps::CV_PARAMS + 11));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.351, 44.048)), module, _8Steps::CV_PARAMS + 12));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(120.351, 44.048)), module, _8Steps::CV_PARAMS + 13));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(140.351, 44.048)), module, _8Steps::CV_PARAMS + 14));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(160.351, 44.048)), module, _8Steps::CV_PARAMS + 15));

		/* KNOBS ROW 2 */
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 64.048)), module, _8Steps::CV_PARAMS + 16));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.351, 64.048)), module, _8Steps::CV_PARAMS + 17));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.351, 64.048)), module, _8Steps::CV_PARAMS + 18));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.351, 64.048)), module, _8Steps::CV_PARAMS + 19));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.351, 64.048)), module, _8Steps::CV_PARAMS + 20));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(120.351, 64.048)), module, _8Steps::CV_PARAMS + 21));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(140.351, 64.048)), module, _8Steps::CV_PARAMS + 22));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(160.351, 64.048)), module, _8Steps::CV_PARAMS + 23));

		/* KNOBS ROW 3 */
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 84.048)), module, _8Steps::CV_PARAMS + 24));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.351, 84.048)), module, _8Steps::CV_PARAMS + 25));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.351, 84.048)), module, _8Steps::CV_PARAMS + 26));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.351, 84.048)), module, _8Steps::CV_PARAMS + 27));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.351, 84.048)), module, _8Steps::CV_PARAMS + 28));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(120.351, 84.048)), module, _8Steps::CV_PARAMS + 29));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(140.351, 84.048)), module, _8Steps::CV_PARAMS + 30));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(160.351, 84.048)), module, _8Steps::CV_PARAMS + 31));
	}
};


Model* model_8Steps = createModel<_8Steps, _8StepsWidget>("8Steps");