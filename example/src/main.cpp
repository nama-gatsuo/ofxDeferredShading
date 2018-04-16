#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(3, 2);
    settings.setSize(1024, 768);
    
    ofCreateWindow(settings);
    ofRunApp(new ofApp());

}
