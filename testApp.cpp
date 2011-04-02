#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp(){

}

//--------------------------------------------------------------
void testApp::setup(){

    //Lataa fontti
    arial64.loadFont("arial.ttf",64);
    arial12.loadFont("arial.ttf",12);
    //aseta tausta harmaa
    ofBackground(100,100,100);

    //ofVideoGrabber
    camWidth 		= 640;	// try to grab at this size.
	camHeight 		= 480;

	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);

	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);

    //ofxDirList
    DIR.setVerbose(false);
    nImages = DIR.listDir("images/kannet");
 	images = new ofImage[nImages];
    //you can now iterate through the files as you like
    for(int i = 0; i < nImages; i++){
            images[i].loadImage(DIR.getPath(i));
    }
    currentImage = 0;

    //ofSerial
    ofSetVerticalSync(true);

	bSendSerialMessage = false;
	ofBackground(255,255,255);


	serial.enumerateDevices();


	//----------------------------------- note:
	// < this should be set
	// to whatever com port
	// your serial device is
	// connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....

	serial.setup("COM3", 9600);  						  // windows example
	//serial.setup("/dev/tty.usbserial-A4001JEC",9600); // mac osx example
	//serial.setup("/dev/ttyUSB0", 9600);			  //linux example

	//-----------------------------------
	nTimesRead = 0;
	nBytesRead = 0;
	readTime = 0;
	memset(bytesReadString, 0, 4);

	//ofVideo
    frameByframe = false;

	fingerMovie.loadMovie("movies/Fog_01.mov");
	fingerMovie.play();
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(100,100,100);

	vidGrabber.grabFrame();

	if (vidGrabber.isFrameNew()){
		int totalPixels = camWidth*camHeight*3;
		unsigned char * pixels = vidGrabber.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoInverted[i] = 255 - pixels[i];
		}
		videoTexture.loadData(videoInverted, camWidth,camHeight, GL_RGB);
	}
//ofSerial
if (bSendSerialMessage){

		// (1) write the letter "a" to serial:
		serial.writeByte('a');

		// (2) read
		// now we try to read 3 bytes
		// since we might not get them all the time 3 - but sometimes 0, 6, or something else,
		// we will try to read three bytes, as much as we can
		// otherwise, we may have a "lag" if we don't read fast enough
		// or just read three every time. now, we will be sure to
		// read as much as we can in groups of three...

		nTimesRead = 0;
		nBytesRead = 0;
		int nRead  = 0;  // a temp variable to keep count per read

		unsigned char bytesReturned[3];

		memset(bytesReadString, 0, 4);
		memset(bytesReturned, 0, 3);

		while( (nRead = serial.readBytes( bytesReturned, 3)) > 0){
			nTimesRead++;
			nBytesRead = nRead;
		};

		memcpy(bytesReadString, bytesReturned, 3);

		bSendSerialMessage = false;
		readTime = ofGetElapsedTimef();
	}
    //ofVideo
    fingerMovie.idleMovie();
}


//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(0xffffff);

//ofVideoGrabber
	vidGrabber.draw(20,20);
	videoTexture.draw(20+camWidth,20,camWidth,camHeight);

//Hello World
	arial64.drawString("KARAOKE!",40,580);

//ofxDirList
    if (nImages > 0){
            ofSetColor(0xffffff);
            images[currentImage].draw(500,530);
            ofSetColor(0x999999);
            string pathInfo = DIR.getPath(currentImage)
                              + "\n\n" + "press any key to advance current image"
                               + "\n\n" + "many thanks to hikaru furuhashi for the OFs" ;
            ofDrawBitmapString(pathInfo, 1000,images[currentImage].height + 200);
        }

        ofSetColor(0x000000);
        for(int i = 0; i < nImages; i++){
                if (currentImage == i) ofSetColor(0xff0000);
                else ofSetColor(0x000000);
                string fileInfo = "file " + ofToString(i+1) + " = " + DIR.getName(i); // +  "path is " + DIR.getPath(i);
                ofDrawBitmapString(fileInfo, 50,i*20 + 600);
        }

//ofSerial
    char tempStr[1024];
	sprintf(tempStr, "click to test serial:\nnBytes read %i\nnTimes read %i\nread: %s\n(at time %0.3f)", nBytesRead, nTimesRead, bytesReadString, readTime);

	if (nBytesRead > 0 && ((ofGetElapsedTimef() - readTime) < 0.5f)){
		ofSetColor(0x000000);
	} else {
		ofSetColor(0xdddddd);
	}
	arial12.drawString(tempStr, 50,850);

//ofVideo
    ofEnableAlphaBlending();
    fingerMovie.draw(20,20);
    ofSetColor(0x000000);
    unsigned char * pixels = fingerMovie.getPixels();
    // let's move through the "RGB" char array
    // using the red pixel to control the size of a circle.
    for (int i = 4; i < 320; i+=8){
        for (int j = 4; j < 240; j+=8){
            unsigned char r = pixels[(j * 320 + i)*3];
            float val = 1 - ((float)r / 255.0f);
            ofCircle(400 + i,20+j,10*val);
        }
    }


    ofSetColor(0x000000);
	ofDrawBitmapString("press f to change",20,320);
    if(frameByframe) ofSetColor(0xCCCCCC);
    ofDrawBitmapString("mouse speed position",20,340);
    if(!frameByframe) ofSetColor(0xCCCCCC); else ofSetColor(0x000000);
    ofDrawBitmapString("keys <- -> frame by frame " ,190,340);
    ofSetColor(0x000000);

    ofDrawBitmapString("frame: " + ofToString(fingerMovie.getCurrentFrame()) + "/"+ofToString(fingerMovie.getTotalNumFrames()),20,380);
    ofDrawBitmapString("duration: " + ofToString(fingerMovie.getPosition()*fingerMovie.getDuration(),2) + "/"+ofToString(fingerMovie.getDuration(),2),20,400);
    ofDrawBitmapString("speed: " + ofToString(fingerMovie.getSpeed(),2),20,420);

    if(fingerMovie.getIsMovieDone()){
        ofSetColor(0xFF0000);
        ofDrawBitmapString("end of movie",20,440);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    // in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...

	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
		arial64.drawString("Hello WORLD!",600,680);
	}
    if (nImages > 0){
        currentImage++;
        currentImage %= nImages;
    }
 //ofVideo
    switch(key){
        case 'f':
            frameByframe=!frameByframe;
            fingerMovie.setPaused(frameByframe);
        break;
        case OF_KEY_LEFT:
            fingerMovie.previousFrame();
        break;
        case OF_KEY_RIGHT:
            fingerMovie.nextFrame();
        break;
        case '0':
            fingerMovie.firstFrame();
        break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
//ofVideo
     if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        float speed = (2 * pct - 1) * 5.0f;
        fingerMovie.setSpeed(speed);
	}

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
//ofVideo
    if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        fingerMovie.setPosition(pct);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    bSendSerialMessage = true;
//ofVideo
	if(!frameByframe){
        fingerMovie.setPaused(true);
	}
}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//ofVideo
    if(!frameByframe){
        fingerMovie.setPaused(false);
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

