#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/CameraUi.h"
#include "cinder/Camera.h"

#include "CiSpoutOut.h"

// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Animation
#include "VDAnimation.h"

#include "VDTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VideodrommImagesApp : public App {
public:
	VideodrommImagesApp();
	void mouseDrag(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	void						fileDrop(FileDropEvent event) override;
private:
	// Spout
	SpoutOut					mSpoutOut;
	// Settings
	VDSettingsRef				mVDSettings;
	// Session
	VDSessionRef				mVDSession;
	// Log
	VDLogRef					mVDLog;
	// Animation
	VDAnimationRef				mVDAnimation;

	VDTextureList				mTexs;
	fs::path					mTexturesFilepath;
	int							i, x, texIndex;
};

VideodrommImagesApp::VideodrommImagesApp()
	: mSpoutOut("vdTextures", app::getWindowSize())
{
	// Settings
	mVDSettings = VDSettings::create();
	// Session
	mVDSession = VDSession::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	// initialize 
	texIndex = 0;
	mTexturesFilepath = getAssetPath("") / "textures.xml";
	if (fs::exists(mTexturesFilepath)) {
		// load textures from file if one exists
		mTexs = VDTexture::readSettings(mVDAnimation, loadFile(mTexturesFilepath));
		i = 0;
		for (auto tex : mTexs)
		{
			if (tex->getType() == VDTexture::SEQUENCE) {
				texIndex = i;
				mTexs[texIndex]->setSpeed(0.02f);
			}
			i++;
		}
	}
	else {
		// otherwise create a texture from scratch
		mTexs.push_back(TextureAudio::create(mVDAnimation));
	}

}
void VideodrommImagesApp::fileDrop(FileDropEvent event)
{
	int index = 1;
	string ext = "";
	// use the last of the dropped files
	ci::fs::path mPath = event.getFile(event.getNumFiles() - 1);
	string mFile = mPath.string();
	int dotIndex = mFile.find_last_of(".");
	int slashIndex = mFile.find_last_of("\\");
	bool found = false;

	if (dotIndex != std::string::npos && dotIndex > slashIndex) ext = mFile.substr(mFile.find_last_of(".") + 1);

	if (ext == "wav" || ext == "mp3")
	{
		for (auto tex : mTexs)
		{
			if (!found) {
				if (tex->getType() == VDTexture::AUDIO) {
					tex->loadFromFullPath(mFile);
					found = true;
				}
			}
		}
	}
	else if (ext == "png" || ext == "jpg")
	{
		for (auto tex : mTexs)
		{
			if (!found) {
				if (tex->getType() == VDTexture::IMAGE) {
					tex->loadFromFullPath(mFile);
					found = true;
				}
			}
		}
	}
	else if (ext == "")
	{
		// try loading image sequence from dir
		for (auto tex : mTexs)
		{
			if (!found) {
				if (tex->getType() == VDTexture::SEQUENCE) {
					tex->loadFromFullPath(mFile);
					found = true;
				}
			}
		}
	}

}
void VideodrommImagesApp::mouseDrag(MouseEvent event)
{
	mTexs[texIndex]->setSpeed((float)event.getX() / (float)getWindowWidth() / 10.0f);
}

void VideodrommImagesApp::mouseDown(MouseEvent event)
{
		
}

void VideodrommImagesApp::mouseUp(MouseEvent event)
{
	
}
void VideodrommImagesApp::keyDown(KeyEvent event)
{
	if (!mVDSession->handleKeyDown(event)) {
		switch (event.getCode()) {
		case KeyEvent::KEY_KP_PLUS:
			texIndex++;
			if (texIndex > mTexs.size() - 1) texIndex = 0;
			break;
		case KeyEvent::KEY_KP_MINUS:
			texIndex--;
			if (texIndex < 0) texIndex = mTexs.size() - 1;
			break;
		case KeyEvent::KEY_ESCAPE:
			// quit the application
			quit();
			break;
		}
	}
}
void VideodrommImagesApp::update()
{
}

void VideodrommImagesApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::draw(mTexs[texIndex]->getTexture(), getWindowBounds());

	mSpoutOut.sendViewport();
	
	i = 0;
	for (auto tex : mTexs)
	{
		int x = 128 * i;
		gl::draw(tex->getTexture(), Rectf(0 + x, 0, 128 + x, 128));
		i++;
	}
}

void prepareSettings(VideodrommImagesApp::Settings *settings)
{
	settings->setWindowSize(640, 480);
}

CINDER_APP(VideodrommImagesApp, RendererGl, prepareSettings)

