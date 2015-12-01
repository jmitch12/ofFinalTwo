#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    soundStream.listDevices();
    
    int bufferSize = 500;
    
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    
    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;
    
    soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    // Box2D
    
    ofDisableAntiAliasing();
    ofBackgroundHex(0x1F2C30);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    // scrolling ground
    groundSpeed = 0.02;
    groundTimer = 0;
    groundRes   = 50;
    
    // build the ground first
    for(int i=0; i<groundRes; i++) {
        float x = ofMap(i, 0, groundRes-1, 0, ofGetWidth());
        float y = ofGetHeight()/2 + ofRandom(-30, 100);
        groundLine.addVertex(x, y);
    }
    
    
    box2d.init();
    box2d.setGravity(0, 20);
    box2d.setFPS(60.0);
    box2d.registerGrabbing();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    //lets scale the vol up to a 0-1 range
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //lets record the volume into an array
    volHistory.push_back( scaledVol );
    
    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
    
    //updating circles when they fall
    
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    box2d.update();
    
    
    ground.clear();
    ground.addVertexes(groundLine);
    ground.create(box2d.getWorld());
   
}

//--------------------------------------------------------------
void ofApp::draw(){
    for(int i=0; i<circles.size(); i++) {
        ofFill();
        ofSetHexColor(0x90d4e3);
        circles[i].get()->draw();
    }
    
    int wave= scaledVol * 300.0f +125;
    int smoothing_amount=50;
    
    float   t = ofGetElapsedTimef() * 1.2;
    float   y = ofSignedNoise(t) + wave*0.2 + scaledVol * 1.0f;
    
    if(ofGetElapsedTimef() - groundTimer > groundSpeed) {
        
        float newHeight = ofGetHeight()/2 + y;
        int n = (int)groundLine.size();
        
        groundTimer = ofGetElapsedTimef();
        
        for(int i=n-1; i>=1; i--) {
            groundLine[i].y = groundLine[i-1].y;
        }
        groundLine[0].y = newHeight;
        
    }
    
    ofSetColor(255);
    ground.updateShape();
    ground.draw();

}

void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < bufferSize; i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if( key == 's' ){
        soundStream.start();
    }
    
    if( key == 'e' ){
        soundStream.stop();
    }
    
    if(key == 'c') {
        float r = ofRandom(4, 20);		// a random radius 4px - 20px
        circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
        ofxBox2dCircle * circle = circles.back().get();
        circle->setPhysics(3.0, 0.53, 0.1);
        circle->setup(box2d.getWorld(), mouseX, mouseY, r);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}


