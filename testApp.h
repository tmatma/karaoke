#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxDirList.h"
#include "ofxAlphaVideoPlayer.h"

class testApp : public ofBaseApp{

	public:

		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        //Hello World Arial
        ofTrueTypeFont arial64;
        ofTrueTypeFont arial12;

        //ofVideoGrabber
		ofVideoGrabber 		vidGrabber;
		unsigned char * 	videoInverted;
		ofTexture			videoTexture;
		int 				camWidth;
		int 				camHeight;

		//ofxDirList
		int 		nImages;
		ofImage	* 	images;
        ofxDirList   DIR;

        //ofSerial
        int         currentImage;

        bool		bSendSerialMessage;			// a flag for sending serial
		char		bytesRead[3];				// data from serial, we will be trying to read 3
		char		bytesReadString[4];			// a string needs a null terminator, so we need 3 + 1 bytes
		int			nBytesRead;					// how much did we read?
		int			nTimesRead;					// how many times did we read?
		float		readTime;					// when did we last read?

		ofSerial	serial;

        //ofVideo
        ofxAlphaVideoPlayer 		fingerMovie;
		bool                frameByframe;
};

#endif
