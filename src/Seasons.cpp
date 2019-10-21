#include "plugin.hpp"


struct Seasons : Module {
	enum ParamIds {
		STEP1_PARAM,
		STEP2_PARAM,
		STEP3_PARAM,
		STEP4_PARAM,
		OUTPUT_MODE,
		OUT_GAIN,
		MODE_BUTTON,
		NUM_PARAMS,
	};
	enum InputIds {
		IN1_INPUT,
		RESET_INPUT,
		FLIP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		OUTPUT_2,
		OUTPUT_3,
		OUTPUT_4,
		NUM_OUTPUTS
	};
	enum LightIds {
		MODE_LIGHT_1,
		MODE_LIGHT_2,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger stepTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger flipTrigger;
	dsp::SchmittTrigger flipButton;

	Seasons() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OUTPUT_MODE, 0.0f, 1.0f, 0.0f, "Output Mode");
		configParam(MODE_BUTTON , 0.0f, 1.0f, 0.0f, "Mode");
		configParam(STEP1_PARAM, 0.0f, 10.0f, 0.0f, "Step 1");
		configParam(STEP2_PARAM, 0.0f, 10.0f, 1.0f, "Step 2");
		configParam(STEP3_PARAM, 0.0f, 10.0f, 3.0f, "Step 3");
		configParam(STEP4_PARAM, 0.0f, 10.0f, 4.0f, "Step 4");
		configParam(OUT_GAIN, 0.0f, 1.0f, 0.5f, "Gain");
	}

	int stepindex= 0;
	float step_values[8] = {};
	int out2_rot = 0;
	int out3_rot = 0;
	int out4_rot = 0;
	int flipflop = 0;
	float output_mod = 5.0f;
	int seq_length = 4;
	bool resetting = false;
	bool stepping = false;

	void process(const ProcessArgs& args) override {

			// Fill Step Array
		float step_values[] = {params[STEP1_PARAM].getValue(), params[STEP2_PARAM].getValue(), params[STEP3_PARAM].getValue(), params[STEP4_PARAM].getValue(), 10.0f - params[STEP4_PARAM].getValue(), 10.0f - params[STEP3_PARAM].getValue(), 10.0f - params[STEP2_PARAM].getValue(), 10.0f - params[STEP1_PARAM].getValue()};

		// Set counter range
		if (params[OUTPUT_MODE].getValue() == 1.0) {
			output_mod = 0.0f;
		}
		else {
			output_mod = 5.0f;
		}

		// Check RESET input
		if (resetTrigger.process(inputs[RESET_INPUT].getVoltage() / 0.01f)) {
			resetting = true;
		}
		else {
			resetting = false;
		}

		// Check STEP input
		if (stepTrigger.process(inputs[IN1_INPUT].getVoltage() / 0.01f)){
			stepping = true;
		}
		else {
			stepping = false;
		}

		// Check flip-flop stage
		if (flipflop == 1) {
			lights[MODE_LIGHT_1].setBrightness(1.f);
			lights[MODE_LIGHT_2].setBrightness(0.f);
			out2_rot = 2;
			out3_rot = 4;
			out4_rot = 6;
			seq_length = 8;
		}
		else {
			lights[MODE_LIGHT_1].setBrightness(0.f);
			lights[MODE_LIGHT_2].setBrightness(1.f);
			out2_rot = 1;
			out3_rot = 2;
			out4_rot = 3;
			seq_length = 4;
		}


		// Reset counter
		if (resetting == true)
			stepindex = 0;

		// Advance step
		if (stepping == true and resetting == false)
			stepindex = (stepindex + 1) % seq_length;

		// Check flip-flop trigger input (MODE)
		if (flipTrigger.process(inputs[FLIP_INPUT].getVoltage() / 0.01f) or flipButton.process(params[MODE_BUTTON].getValue() / 0.01f))
			flipflop = (flipflop + 1) % 2;
		

		
		// Generate four outputs
		outputs[OUTPUT_1].setVoltage((step_values[stepindex] - output_mod) * params[OUT_GAIN].getValue());
		outputs[OUTPUT_2].setVoltage((step_values[((stepindex + out2_rot) % seq_length)] - output_mod) * params[OUT_GAIN].getValue());
		outputs[OUTPUT_3].setVoltage((step_values[((stepindex + out3_rot) % seq_length)] - output_mod) * params[OUT_GAIN].getValue());
		outputs[OUTPUT_4].setVoltage((step_values[((stepindex + out4_rot) % seq_length)] - output_mod) * params[OUT_GAIN].getValue());
	}
};


struct SeasonsWidget : ModuleWidget {
	SeasonsWidget(Seasons* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Seasons.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<CKSS>(Vec(90.5f, 208), module, Seasons::OUTPUT_MODE));
		addParam(createParamCentered<sa_GreenPushButton>(Vec(65, 276), module, Seasons::MODE_BUTTON ));
		addParam(createParamCentered<sa_GreenKnobSmall>(Vec(38, 132), module, Seasons::STEP1_PARAM));
		addParam(createParamCentered<sa_GreenKnobSmall>(Vec(73, 132), module, Seasons::STEP2_PARAM));
		addParam(createParamCentered<sa_GreenKnobSmall>(Vec(109, 132), module, Seasons::STEP3_PARAM));
		addParam(createParamCentered<sa_GreenKnobSmall>(Vec(143, 132), module, Seasons::STEP4_PARAM));
		addParam(createParamCentered<sa_GreenKnobBig>(Vec(141, 209), module, Seasons::OUT_GAIN));


		// addInput(createInputCentered<PJ301MPort>(Vec(18, 311), module, Seasons::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(30.5f, 323.5f), module, Seasons::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(65.2f, 323.5f), module, Seasons::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(30.5f, 276.5f), module, Seasons::FLIP_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(110.5f, 276.5f), module, Seasons::OUTPUT_1));
		addOutput(createOutputCentered<PJ301MPort>(Vec(143.5f, 276.5f), module, Seasons::OUTPUT_2));
		addOutput(createOutputCentered<PJ301MPort>(Vec(110.5f, 322.5f), module, Seasons::OUTPUT_3));
		addOutput(createOutputCentered<PJ301MPort>(Vec(143.5f, 322.5f), module, Seasons::OUTPUT_4));

		addChild(createLightCentered<MediumLight<GreenLight>>(Vec(23, 219), module, Seasons::MODE_LIGHT_1));
		addChild(createLightCentered<MediumLight<GreenLight>>(Vec(23, 193), module, Seasons::MODE_LIGHT_2));
	}
};


Model* modelSeasons = createModel<Seasons, SeasonsWidget>("Seasons");