# ofxDeferredShading
![](./screenshot.jpg)

Here is [demo](https://www.youtube.com/watch?v=xhm2CdpSpcI).

## Concept
### Modern OpenGL compatible
I was satisfied with [ofxPostPrpcessing](https://github.com/neilmendoza/ofxPostProcessing) long time, which is fabulous and has so many beautiful vfx in it. I really appreciate the author and contributors.
But there is little problem when I use my own shader with it. It is not written in "modern" shader version so cannot coexist with the "**programmble renderer**".
At this moment, however we have to decide to abandon legacy but helpful functions including ```ofLight```, ```glBegin()```-```glEnd()```...
### Photo-realistic in Real-time
And the addon mentioned above has only few effects for photo-real purpose such like "casting shadow". So I decided to reproduce and regather PostProcesses focusing "photo-real" rendering and compatible in modern version.

now it contains
* Multiple point lights with attenuation
* Casting shadow with directional light
* Ambient occlusion (SSAO)
* HDR bloom
    * blooming when vertex colors are specified greater than 1.0
* Defocus blur
    * Imitating "depth of field" effect

## Support
* openFrameworks 0.10.0~ (using glm library)
* helper addon: [ofxDeferredParams]()

## Usage
* copy the ```shader/``` directory under your ```bin/data/```
    * so you can edit them
* specify modern version of OpenGL in main.cpp
```C++
int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 1); // now we use OpenGL 4.1
    settings.setSize(1024, 768);

    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
```
* declare instances in ofApp.h
    * it's safer to use `std::shared_ptr`    
```C++
// declare in ofApp.h
ofxDeferredProcessing deferred;
shared_ptr<ofxDeferred::SsaoPass> ssao;
shared_ptr<ofxDeferred::PointLightPass> points;
shared_ptr<ofxDeferred::ShadowLightPass> shadow;
shared_ptr<ofxDeferred::HdrBloomPass> bloom;
...
```
* init and create Passes in ofApp::setup(), and get reference of Passes
```C++
deferred.init(ofGetWidth(), ofGetHeight());
ssao = deferred.createPass<ofxDeferred::SsaoPass>();
points = deferred.createPass<ofxDeferred::PointLightPass>();
shadow = deferred.createPass<ofxDeferred::ShadowLightPass>();    
bloom = deferred.createPass<ofxDeferred::HdrBloomPass>();
```
* set parameters via reference
```C++
ssao->setOcculusionRadius(5.0);
ssao->setDarkness(1.0);
```
* draw objects between `begin()` and `end()`.
* If you want to HDR color, just specify vertex color more than 1.0 in `ofFloatColor`'s parameters.
* if you want to use cast shadow (`ShadowLightPass`), you need another draw-objects call.

    ```C++
    // for shadow map
    shadowLightPass->beginShadowMap();
        drawObjs(); // draw something
        lightPass->drawLights(); // draw light bulbs
    shadowLightPass->endShadowMap();

    // for rendering pass
    deferred.begin(cam);
        drawObjs(); // exactly the same call above
        lightPass->drawLights();
    deferred.end();
    ```
* Also, you can use own shader for mesh rendering. In this case, you need to write pixel data into color buffer properly.

```GLSL
#version 400

uniform int shadowFlag;

in vec4 vPos;
in vec4 vNormal;
in vec4 vColor;
in float vDepth;

layout (location = 0) out vec4 outputColor0; // Color
layout (location = 1) out vec4 outputColor1; // Position
layout (location = 2) out vec4 outputColor2; // Normal and Depth
layout (location = 3) out vec4 outputColor3; // HDR map

void main(){

    if (shadowFlag == 1) {
        outputColor0.r = vDepth;
        outputColor0.a = 1.;
    } else {
        outputColor0 = vColor;
        outputColor1 = vPos;
        outputColor2 = vec4(vNormal.xyz, vDepth);
        outputColor3 = any(greaterThan(vColor, vec4(1.))) ? vColor : vec4(0.,0.,0.,1.);
    }

}

```

## Reference
### Render Pass
* Basic architecture and "RenderPass" is quoted from [neilmendoza/ofxPostProcessing](https://github.com/neilmendoza/ofxPostProcessing).
* GBuffer design from [hanasaan/ofxDeferredProcessing](https://github.com/hanasaan/ofxDeferredProcessing/)

### Effects
* Ambient Occlusion from [Learn OpenGL](https://learnopengl.com/#!Advanced-Lighting/SSAO)
* Point light with attenuation from
 [jacres/of-DeferredRendering](https://github.com/jacres/of-DeferredRendering)
* Shadow Mapping from [Learn OpenGL](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)
* High Dynamic Range + Bloom from [Learn OpenGL](https://learnopengl.com/#!Advanced-Lighting/Bloom)
* Defocus Blur from http://pasteall.org/10779

## Author
[Ayumu Nagamatsu](http://ayumu-nagamatsu.com/)
