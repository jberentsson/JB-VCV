/*
	8Steps
	Eight step binary controller step sequencer.
	Jóhann Berentsson
	January 2025
*/

#include "plugin.hpp"
#include <iostream>

/*
	TODO:
	- Finish the panel.
	- Fix the output names.
*/

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
		ENUMS(STEP_LIGHT, 32),
		LIGHTS_LEN
	};

	std::string createInputLabel(int x, int i){
		char *inputLabels[4] =  {"Bit A", "Bit B", "Bit C", "Enable"};
		return string::f("%s %i", inputLabels[x], i);
	}

	int oldIndex[4] = {0};

	int bits2dec(bool a, bool b, bool c){
		return (1*a) + (2*b) + (4*c);
	}

	void updateLights(int i, int value, int enabled){
		for (int j = 0; j < 8; j++) {
			if(j == value){
				/* Activat step. */
				lights[STEP_LIGHT + ((8 * i) + j)].setBrightness(enabled ? true : false);
			}else{
				/* Deactivate step. */
				lights[STEP_LIGHT + ((8 * i) + j)].setBrightness(false);
			}
		}
	}

	_8Steps() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		/* CV */
		for (int i = 0; i < 4; i++) {
			/* ROW */
			for (int j = 0; j < 8; j++) {
				/* STEP */
				configParam(CV_PARAMS + 8 * i + j, -5.f, 5.f, 0.f, string::f("CV %d step %d", i, j), " V");
			}
		}

		for(int i = 0; i < 4; i++){
			/* INPUTS */
			configInput(BIT_A_INPUT + i, createInputLabel(0, i));
			configInput(BIT_B_INPUT + i, createInputLabel(1, i));
			configInput(BIT_C_INPUT + i, createInputLabel(2, i));
			configInput(ENABLE_INPUT + i, createInputLabel(3, i));

			/* OUTPUTS */
			configOutput(CV_OUTPUTS + i, string::f("CV %d", i));

			/* LIGHTS */
			updateLights(i, 0, 0);
		}
	}

	void process(const ProcessArgs& args) override {
		/* INPUTS */
		bool valuesRows[4][4] = {
			{
				inputs[BIT_A_INPUT + 0].getVoltage() && true,
				inputs[BIT_B_INPUT + 0].getVoltage() && true,
				inputs[BIT_C_INPUT + 0].getVoltage() && true,
				true
			},{
				inputs[BIT_A_INPUT + 1].getVoltage() && true,
				inputs[BIT_B_INPUT + 1].getVoltage() && true,
				inputs[BIT_C_INPUT + 1].getVoltage() && true,
				true
			},{
				inputs[BIT_A_INPUT + 2].getVoltage() && true,
				inputs[BIT_B_INPUT + 2].getVoltage() && true,
				inputs[BIT_C_INPUT + 2].getVoltage() && true,
				true
			},{
				inputs[BIT_A_INPUT + 3].getVoltage() && true,
				inputs[BIT_B_INPUT + 3].getVoltage() && true,
				inputs[BIT_C_INPUT + 3].getVoltage() && true,
				true
			}
		};

		int valueRows[4] = {0};

		for (int i = 0; i < 4; i++){
			/* BIT A */
			if (!inputs[BIT_A_INPUT + i].isConnected()){
				if (i == 0){
					valuesRows[i][0] = true;
				} else {
					valuesRows[i][0] = valuesRows[i-1][0];
				}
			}
			/* BIT B */
			if (!inputs[BIT_B_INPUT + i].isConnected()){
				if (i == 0){
					valuesRows[i][1] = true;
				} else {
					valuesRows[i][1] = valuesRows[i-1][1];
				}
			}

			/* BIT C */
			if (!inputs[BIT_C_INPUT + i].isConnected()){
				if (i == 0){
					valuesRows[i][2] = true;
				} else {
					valuesRows[i][2] = valuesRows[i-1][2];
				}
			}

			/* ENABLE */
			if (!inputs[ENABLE_INPUT + i].isConnected()){
				if (i == 0){
					valuesRows[i][3] = true;
				} else {
					valuesRows[i][3] = valuesRows[i-1][3];
				}
			} else {
				valuesRows[i][3] = inputs[ENABLE_INPUT + i].getVoltage() && true;
			}

			/* BITS TO INT */
			valueRows[i] = bits2dec(valuesRows[i][0], valuesRows[i][1], valuesRows[i][2]);
		}

		for (int i = 0; i < 4; i++){
			/* LIGHTS */
			if(valueRows[i] != oldIndex[i]){
				updateLights(i, valueRows[i], valuesRows[i][3]);
				oldIndex[i] = valueRows[i];
			}
		}

		/* CV OUTPUTS */
		outputs[CV_OUTPUTS + 0].setVoltage(params[CV_PARAMS + 8 * 0 + valueRows[0]].getValue() * (valuesRows[0][3]));
		outputs[CV_OUTPUTS + 1].setVoltage(params[CV_PARAMS + 8 * 1 + valueRows[1]].getValue() * (valuesRows[1][3]));
		outputs[CV_OUTPUTS + 2].setVoltage(params[CV_PARAMS + 8 * 2 + valueRows[2]].getValue() * (valuesRows[2][3]));
		outputs[CV_OUTPUTS + 3].setVoltage(params[CV_PARAMS + 8 * 3 + valueRows[3]].getValue() * (valuesRows[3][3]));
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
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.297, 107.53)), module, _8Steps::BIT_A_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.297, 107.53)), module, _8Steps::BIT_B_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.297, 107.53)), module, _8Steps::BIT_C_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.297, 107.53)), module, _8Steps::ENABLE_INPUT + 0));

		/* ROW 1 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.297, 117.53)), module, _8Steps::BIT_A_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.297, 117.53)), module, _8Steps::BIT_B_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.297, 117.53)), module, _8Steps::BIT_C_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.297, 117.53)), module, _8Steps::ENABLE_INPUT + 1));

		/* ROW 2 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.297, 107.53)), module, _8Steps::BIT_A_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(70.297, 107.53)), module, _8Steps::BIT_B_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.297, 107.53)), module, _8Steps::BIT_C_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(90.297, 107.53)), module, _8Steps::ENABLE_INPUT + 2));

		/* ROW 3 */
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.297, 117.53)), module, _8Steps::BIT_A_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(70.297, 117.53)), module, _8Steps::BIT_B_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.297, 117.53)), module, _8Steps::BIT_C_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(90.297, 117.53)), module, _8Steps::ENABLE_INPUT + 3));

		/* CV Outputs */
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.297, 107.53)), module, _8Steps::CV_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(120.297, 107.53)), module, _8Steps::CV_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.297, 117.53)), module, _8Steps::CV_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(120.297, 117.53)), module, _8Steps::CV_OUTPUTS + 3));

		/* LIGHTS */
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 8; j++){
				addChild(createLightCentered<TinyLight<YellowBlueLight<>>>(mm2px(Vec(10.864 + (15*j), 15.393 + (20 * i))), module, _8Steps::STEP_LIGHT + ((8 * i) + j)));
			}
		}
		
		/* KNOBS ROW 0 */
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 8; j++){
				addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.351 + (15 * j), 24.048 + (20 * i))), module, _8Steps::CV_PARAMS + ((8 * i) + j)));
			}
		}
	}
};


Model* model_8Steps = createModel<_8Steps, _8StepsWidget>("8Steps");