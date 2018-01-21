#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Websocket
#include "VDWebsocket.h"
// Midi
#include "MidiIn.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip; 
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDRouter instance
	typedef std::shared_ptr<class VDRouter> VDRouterRef;
	// midi
	typedef std::shared_ptr<class MIDI> MIDIRef;

	struct midiInput
	{
		string			portName;
		bool			isConnected;
	};
	struct midiOutput
	{
		string			portName;
		bool			isConnected;
	};

	class VDRouter {
	public:
		VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket);
		static VDRouterRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDWebsocketRef aVDWebsocket)
		{
			return shared_ptr<VDRouter>(new VDRouter(aVDSettings, aVDAnimation, aVDWebsocket));
		}
		//void						update();
		void						shutdown();
		// messages
		void						updateParams(int iarg0, float farg1);
		// MIDI
		void						midiSetup();
		int							getMidiInPortsCount() { return mMidiInputs.size(); };
		string						getMidiInPortName(unsigned int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].portName : "No midi in ports"; };
		bool						isMidiInConnected(unsigned int i) { return (i < mMidiInputs.size()) ? mMidiInputs[i].isConnected : false; };
		int							getMidiOutPortsCount() { return mMidiOutputs.size(); };
		string						getMidiOutPortName(unsigned int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].portName : "No midi out ports"; };
		bool						isMidiOutConnected(unsigned int i) { return (i < mMidiOutputs.size()) ? mMidiOutputs[i].isConnected : false; };
		void						midiOutSendNoteOn(int i, int channel, int pitch, int velocity);

		void						openMidiInPort(int i);
		void						closeMidiInPort(int i);
		void						openMidiOutPort(int i);
		void						closeMidiOutPort(int i);

		int							selectedWarp() { return mSelectedWarp; };
		int							selectedFboA() { return mSelectedFboA; };
		int							selectedFboB() { return mSelectedFboB; };
		void						setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) { mSelectedFboA = aWarpFboIndex; }
		void						setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) { mSelectedFboB = aWarpFboIndex; }
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		// VDWebsocket
		VDWebsocketRef				mVDWebsocket;
		// lights4events
		void						colorWrite();
		// MIDI
		vector<midiInput>			mMidiInputs;
		// midi inputs: couldn't make a vector
		midi::Input					mMidiIn0;
		midi::Input					mMidiIn1;
		midi::Input					mMidiIn2;
		midi::Input					mMidiIn3;
		void						midiListener(midi::Message msg);
		// midi output
		midi::MidiOut				mMidiOut0;
		midi::MidiOut				mMidiOut1;
		midi::MidiOut				mMidiOut2;
		midi::MidiOut				mMidiOut3;
		vector<midiOutput>			mMidiOutputs;
		string						midiControlType;
		int							midiControl;
		int							midiPitch;
		int							midiVelocity;
		float						midiNormalizedValue;
		int							midiValue;
		int							midiChannel;

		int							mSelectedWarp;
		int							mSelectedFboA;
		int							mSelectedFboB;

		static const int			MAX = 16;

	};
}

