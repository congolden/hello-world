#pragma once

#ifndef __connorMultiDelay__
#define __connorMultiDelay__

#include "connorStereoDelay.h"
#include "connorModFilter.h"
#include "fxobjects.h"

/**
\struct connorMultiDelayParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the connorMultiDelay object.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
struct connorMultiDelayParameters
{
	connorMultiDelayParameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	connorMultiDelayParameters& operator=(const connorMultiDelayParameters& params)	// need this override for collections to work
	{
		// --- it is possible to try to make the object equal to itself
		//     e.g. thisObject = thisObject; so this code catches that
		//     trivial case and just returns this object
		if (this == &params)
			return *this;

		// --- copy from params (argument) INTO our variables
		delayTime1_mSec = params.delayTime1_mSec;
		delayTime2_mSec = params.delayTime2_mSec;
		delayTime3_mSec = params.delayTime3_mSec;
		delayTime4_mSec = params.delayTime4_mSec;
		tap1Level_dB = params.tap1Level_dB;
		tap2Level_dB = params.tap2Level_dB;
		tap3Level_dB = params.tap3Level_dB;
		tap4Level_dB = params.tap4Level_dB;

		delayFeedback_Pct = params.delayFeedback_Pct;
		delayRatio_Pct = params.delayRatio_Pct;
		wetLevel_dB = params.wetLevel_dB;
		dryLevel_dB = params.dryLevel_dB;

		lfoRate_Hz = params.lfoRate_Hz;
		lfoDepth_Pct = params.lfoDepth_Pct;
		feedback_Pct = params.feedback_Pct;

		enableFlange = params.enableFlange;
		enableChorus = params.enableChorus;
		enableDouble = params.enableDouble;
		// --- MUST be last
		return *this;
	}

	// --- individual parameters
	double delayTime1_mSec = 0.0;
	double delayTime2_mSec = 0.0;
	double delayTime3_mSec = 0.0;
	double delayTime4_mSec = 0.0;
	double tap1Level_dB = 0.0;
	double tap2Level_dB = 0.0;
	double tap3Level_dB = 0.0;
	double tap4Level_dB = 0.0;
	double delayFeedback_Pct = 0.0;
	double delayRatio_Pct = 0.0;
	double wetLevel_dB = 0.0;
	double dryLevel_dB = 0.0;

	//

	double lfoRate_Hz = 2.0;
	double lfoDepth_Pct = 0.0;
	double feedback_Pct = 0.0;

	bool enableFlange = false;
	bool enableChorus = false;
	bool enableDouble = false;


	// --- Discrete Plugin Variables 
	int delayAlgorithm = 0;
	enum class delayAlgorithmEnum { normal, pingpong };		///< init
};


/**
\class connorMultiDelay
\ingroup FX-Objects
\brief
The connorMultiDelay object implements ....

Audio I/O:
- Processes mono input to mono output.
- *** Optionally, process frame *** Modify this according to your object functionality

Control I/F:
- Use connorMultiDelayParameters structure to get/set object params.

\author <Your Name> <http://www.yourwebsite.com>
\remark <Put any remarks or notes here>
\version Revision : 1.0
\date Date : 2019 / 01 / 31
*/
class connorMultiDelay : public IAudioSignalProcessor
{
public:
	connorMultiDelay(void) {}	/* C-TOR */
	~connorMultiDelay(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- store the sample rate
		sampleRate = _sampleRate;

		stereoDelay.reset(sampleRate);

		// --- THEN create 2 second delay buffers
		stereoDelay.createDelayBuffers(sampleRate, 2000.0);

		// --- do any other per-audio-run inits here
		flange.reset(sampleRate);
		chorus.reset(sampleRate);
		doubler.reset(sampleRate);

		return true;
	}

	/** process MONO input */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		// --- the output variable
		double yn = 0.0;

		// --- do your DSP magic here to create yn

		// --- done
		return yn;
	}

	/** query to see if this object can process frames */
	virtual bool canProcessAudioFrame() { return true; } // <-- change this!

	/** process audio frame: implement this function if you answer "true" to above query */
	virtual bool processAudioFrame(const float* inputFrame,	/* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
					     float* outputFrame,
					     uint32_t inputChannels,
					     uint32_t outputChannels)
	{
		// --- do nothing
		double xnL = inputFrame[0];
		double xnR = inputFrame[1];

		double ynL = xnL;
		double ynR = xnR;

		stereoDelay.processAudioFrame(inputFrame, outputFrame, inputChannels, outputChannels);

		if (parameters.enableFlange || parameters.enableChorus || parameters.enableDouble)
		{
			if (parameters.enableFlange)
				flange.processAudioFrame(outputFrame, outputFrame, inputChannels, outputChannels);
			if (parameters.enableChorus)
				chorus.processAudioFrame(outputFrame, outputFrame, inputChannels, outputChannels);
			if (parameters.enableDouble)
				doubler.processAudioFrame(outputFrame, outputFrame, inputChannels, outputChannels);

			return true;
		}
		else
			return true;
		/*
		if (inputChannels == 1 && outputChannels == 1)
		{
			outputFrame[0] = ynL;
			return true; // --- processed!
		}


		// --- mono/stereo: pan ledt input channel to left + right outputs
		if (inputChannels == 1 && outputChannels == 2)
		{
			outputFrame[0] = ynL;
			outputFrame[1] = ynL;
			// --- outbound variables

			return true; // --- processed
		}

		// --- stereo/stereo: pan is actually a balance control
		else if (inputChannels == 2 && outputChannels == 2)
		{
			outputFrame[0] = ynL;
			outputFrame[1] = ynR;
			// --- outboud variables
			return true; // --- processed!
		} */

		//return false; // NOT handled
	}


	/** get parameters: note use of custom structure for passing param data */
	/**
	\return connorMultiDelayParameters custom data structure
	*/
	connorMultiDelayParameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param connorMultiDelayParameters custom data structure
	*/
	void setParameters(const connorMultiDelayParameters& params)
	{
		// --- copy them; note you may choose to ignore certain items
		//     and copy the variables one at a time, or you may test
		//     to see if cook-able variables have changed; if not, then
		//     do not re-cook them as it just wastes CPU
		parameters = params;
		connorStereoDelayParameters delayParameters;
		delayParameters.leftDelay_mSec = parameters.delayTime1_mSec;
		delayParameters.rightDelay_mSec = parameters.delayTime1_mSec;
		delayParameters.delay2_mSec = parameters.delayTime2_mSec;
		delayParameters.delay3_mSec = parameters.delayTime3_mSec;
		delayParameters.delay4_mSec = parameters.delayTime4_mSec;
		delayParameters.tap1Level_db = parameters.tap1Level_dB;
		delayParameters.tap2Level_db = parameters.tap2Level_dB;
		delayParameters.tap3Level_db = parameters.tap3Level_dB;
		delayParameters.tap4Level_db = parameters.tap4Level_dB;

		delayParameters.feedback_Pct = parameters.delayFeedback_Pct;
		delayParameters.delayRatio_Pct = parameters.delayRatio_Pct;
		//delayParameters.algorithm = parameters.delayAlgorithm;   /// come back to this
		delayParameters.wetLevel_dB = parameters.wetLevel_dB;
		delayParameters.dryLevel_dB = parameters.dryLevel_dB;

		stereoDelay.setParameters(delayParameters);

		ConnorModulatedDelayParameters modParameters;
		modParameters.algorithm = modDelaylgorithm::kFlanger;
		modParameters.lfoRate_Hz = parameters.lfoRate_Hz;
		modParameters.lfoDepth_Pct = parameters.lfoDepth_Pct;
		modParameters.feedback_Pct = parameters.feedback_Pct;
		flange.setParameters(modParameters);

		ConnorModulatedDelayParameters chorusParameters;
		chorusParameters.algorithm = modDelaylgorithm::kChorus;
		chorusParameters.lfoRate_Hz = parameters.lfoRate_Hz;
		chorusParameters.lfoDepth_Pct = parameters.lfoDepth_Pct;
		chorusParameters.feedback_Pct = parameters.feedback_Pct;
		chorus.setParameters(chorusParameters);

		ConnorModulatedDelayParameters doubleParameters;
		doubleParameters.algorithm = modDelaylgorithm::kVibrato;
		doubleParameters.lfoRate_Hz = parameters.lfoRate_Hz;
		doubleParameters.lfoDepth_Pct = parameters.lfoDepth_Pct;
		doubleParameters.feedback_Pct = parameters.feedback_Pct;
		doubler.setParameters(doubleParameters);
		
		// --- cook parameters here
	}

private:
	connorMultiDelayParameters parameters; ///< object parameters
	ConnorModulatedDelay flange;
	ConnorModulatedDelay chorus;
	ConnorModulatedDelay doubler;
	// --- local variables used by this object
	double sampleRate = 0.0;	///< sample rate
	ConnorStereoDelay stereoDelay;

};

#endif
