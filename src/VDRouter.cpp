#include "VDRouter.h"

using namespace VideoDromm;

VDRouter::VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mVDWebsocket = aVDWebsocket;
	CI_LOG_V("VDRouter constructor");

	// midi
	if (mVDSettings->mMIDIOpenAllInputPorts) midiSetup();
	mSelectedWarp = 0;
	mSelectedFboA = 1;
	mSelectedFboB = 2;
}

void VDRouter::shutdown() {

	mMidiIn0.closePort();
	mMidiIn1.closePort();
	mMidiIn2.closePort();
	mMidiOut0.closePort();
	mMidiOut1.closePort();
	mMidiOut2.closePort();

}

void VDRouter::midiSetup() {
	stringstream ss;
	ss << "setupMidi: ";

	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i < mMidiIn0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiInputs.size(); j++)
			{
				if (mMidiInputs[j].portName == mMidiIn0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiInput mIn;
				mIn.portName = mMidiIn0.getPortName(i);
				mMidiInputs.push_back(mIn);
				if (mVDSettings->mMIDIOpenAllInputPorts) {
					openMidiInPort(i);
					mMidiInputs[i].isConnected = true;
					ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName;
				}
				else {
					mMidiInputs[i].isConnected = false;
					ss << "Available MIDI in port " << i << " " << mMidiIn0.getPortName(i);
				}
			}
		}
	}
	else {
		ss << "No MIDI in ports found!" << std::endl;
	}
	ss << std::endl;

	mVDSettings->mNewMsg = true;
	mVDSettings->mMsg = ss.str();
	// midi out
	//mMidiOut0.getPortList();
	if (mMidiOut0.getNumPorts() > 0) {
		for (int i = 0; i < mMidiOut0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiOutputs.size(); j++)
			{
				if (mMidiOutputs[j].portName == mMidiOut0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiOutput mOut;
				mOut.portName = mMidiOut0.getPortName(i);
				mMidiOutputs.push_back(mOut);

				mMidiOutputs[i].isConnected = false;
				ss << "Available MIDI output port " << i << " " << mMidiOut0.getPortName(i);

			}
		}
	}
	else {
		ss << "No MIDI Out Ports found!!!!" << std::endl;
	}
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
}

void VDRouter::openMidiInPort(int i) {

	// HACK Push2 has 2 midi ports, we keep the internal one not useable 
	if (mMidiIn0.getPortName(i) != "Ableton Push 2 1") {

		stringstream ss;
		if (i < mMidiIn0.getNumPorts()) {
			if (i == 0) {
				mMidiIn0.openPort(i);
				mMidiIn0.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
			if (i == 1) {
				mMidiIn1.openPort(i);
				mMidiIn1.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
			if (i == 2) {
				mMidiIn2.openPort(i);
				mMidiIn2.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
			}
		}
		mMidiInputs[i].isConnected = true;
		ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName << std::endl;
		mVDSettings->mMsg = ss.str();
		mVDSettings->mNewMsg = true;
	}

}
void VDRouter::closeMidiInPort(int i) {

	if (i == 0)
	{
		mMidiIn0.closePort();
	}
	if (i == 1)
	{
		mMidiIn1.closePort();
	}
	if (i == 2)
	{
		mMidiIn2.closePort();
	}
	mMidiInputs[i].isConnected = false;

}
void VDRouter::midiOutSendNoteOn(int i, int channel, int pitch, int velocity) {

	if (i == 0)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut0.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 1)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut1.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 2)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut2.sendNoteOn(channel, pitch, velocity);
	}

}
void VDRouter::openMidiOutPort(int i) {

	stringstream ss;
	ss << "Port " << i << " ";
	if (i < mMidiOutputs.size()) {
		if (i == 0) {
			if (mMidiOut0.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut0.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
		if (i == 1) {
			if (mMidiOut1.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut1.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
		if (i == 2) {
			if (mMidiOut2.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << "Opened MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
				mMidiOut2.sendNoteOn(1, 40, 64);
			}
			else {
				ss << "Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName << std::endl;
			}
		}
	}
	mVDSettings->mMsg = ss.str();
	mVDSettings->mNewMsg = true;

}
void VDRouter::closeMidiOutPort(int i) {

	if (i == 0)
	{
		mMidiOut0.closePort();
	}
	if (i == 1)
	{
		mMidiOut1.closePort();
	}
	if (i == 2)
	{
		mMidiOut2.closePort();
	}
	mMidiOutputs[i].isConnected = false;

}

void VDRouter::midiListener(midi::Message msg) {
	stringstream ss;
	midiChannel = msg.channel;
	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		if (midiControl > 20 && midiControl < 49) {
			if (midiControl > 20 && midiControl < 29) {
				mSelectedWarp = midiControl - 21;
			}
			if (midiControl > 30 && midiControl < 39) {
				mSelectedFboA = midiControl - 31;
			}
			if (midiControl > 40 && midiControl < 49) {
				mSelectedFboB = midiControl - 41;
			}
		}
		else {
			updateParams(midiControl, midiNormalizedValue);
		}
		//mWebSockets->write("{\"params\" :[{" + controlType);
		break;
	case MIDI_NOTE_ON:
		midiControlType = "/on";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		// quick hack!
		mVDAnimation->setFloatUniformValueByIndex(14, 1.0f + midiNormalizedValue);
		break;
	case MIDI_NOTE_OFF:
		midiControlType = "/off";
		midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		break;
	default:
		break;
	}
	ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	CI_LOG_V("Midi: " + ss.str());

	mVDSettings->mMidiMsg = ss.str();
}

void VDRouter::updateParams(int iarg0, float farg1) {
	if (farg1 > 0.1) {
		//avoid to send twice
		if (iarg0 == 58) {
			// track left		
			mVDSettings->iTrack--;
			if (mVDSettings->iTrack < 0) mVDSettings->iTrack = 0;
		}
		if (iarg0 == 59) {
			// track right
			mVDSettings->iTrack++;
			if (mVDSettings->iTrack > 7) mVDSettings->iTrack = 7;
		}
		if (iarg0 == 60) {
			// set (reset blendmode)
			mVDSettings->iBlendmode = 0;
		}
		if (iarg0 == 61) {
			// right arrow
			mVDSettings->iBlendmode--;
			if (mVDSettings->iBlendmode < 0) mVDSettings->iBlendmode = mVDAnimation->getBlendModesCount() - 1;
		}
		if (iarg0 == 62) {
			// left arrow
			mVDSettings->iBlendmode++;
			if (mVDSettings->iBlendmode > mVDAnimation->getBlendModesCount() - 1) mVDSettings->iBlendmode = 0;
		}
	}
	if (iarg0 > 0 && iarg0 < 9) {
		// sliders 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 10 && iarg0 < 19) {
		// rotary 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
		// audio multfactor
		if (iarg0 == 13) mVDAnimation->setFloatUniformValueByIndex(iarg0, (farg1 + 0.01) * 10);
		// exposure
		if (iarg0 == 14) mVDAnimation->setFloatUniformValueByIndex(iarg0, (farg1 + 0.01) * mVDAnimation->getMaxUniformValueByIndex(14));
		// xfade
		if (iarg0 == 18) {
			mVDSettings->xFade = farg1;
			mVDSettings->xFadeChanged = true;
		}
		mVDWebsocket->wsWrite("{\"params\" :[{\"name\":" + toString(iarg0) + ",\"value\":" + toString(mVDAnimation->getFloatUniformValueByIndex(iarg0)) + "}]}");

	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29) {
		// top row
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 30 && iarg0 < 39)
	{
		// middle row
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
	if (iarg0 > 40 && iarg0 < 49) {
		// low row 
		mVDAnimation->setFloatUniformValueByIndex(iarg0, farg1);
	}
}


void VDRouter::colorWrite()
{
#if defined( CINDER_MSW )
	// lights4events
	char col[8];
	int r = mVDAnimation->getFloatUniformValueByIndex(1) * 255;
	int g = mVDAnimation->getFloatUniformValueByIndex(2) * 255;
	int b = mVDAnimation->getFloatUniformValueByIndex(3) * 255;
	sprintf(col, "#%02X%02X%02X", r, g, b);
	mVDWebsocket->wsWrite(col);
#endif
}
