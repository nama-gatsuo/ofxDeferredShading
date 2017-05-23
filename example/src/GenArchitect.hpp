#include "ofMain.h"

class GenArchitecture {
public:
    void setup() {
        createMesh();
    };
    void draw() {
        mesh.draw();
    };
    void createMesh(){
        
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 3; j++) {
                ofMatrix4x4 mat;
                mat.glTranslate(0, 100. * (i - 16.0), 0);
                
                createRoom(&mat, ofRandom(2.0, 5.0) * 80.);
            }
        }
        
    };
    void createRoom(ofMatrix4x4 *mat, float r) {
        int edgeNum = ceil(ofRandom(2.0, 4.0)) * 2;
        float wallWidth = ofRandom(0.2, 2.0) * r;
        float wallHeight = 100 * ofRandom(0.1, 4.0);
        for (int i = 0; i < edgeNum; i++) {
            
            ofMatrix4x4 m = *mat;
            m.glRotate(360 / edgeNum * i, 0, 1, 0);
            m.glTranslate(r, 0, 0);
            createWall(&m, wallWidth, wallHeight);
        }
        
    }
    
    void createWall(ofMatrix4x4 *mat, float w, float h){
        
        if (ofRandom(1.0) < 0.6) createBox(mat, 5, h, w);
        int columnNum = ceil(ofRandom(6.0));
        
        for (int i = 0; i < columnNum+1; i++) {
            ofMatrix4x4 m = *mat;
            m.glTranslate(0,0, w/columnNum * i - w/2);
            createBox(&m, 10, h, 10);
        }
        
        float t = 10 * ofRandom(1, 3);
        
        ofMatrix4x4 _m = *mat;
        _m.glTranslate(0, -h/2, 0);
        _m.glTranslate(-t/2, 0, 0);
        createBox(&_m, t, 10, w+10);
        
        _m = *mat;
        _m.glTranslate(0, h/2, 0);
        _m.glTranslate(-t/2, 0, 0);
        createBox(&_m, t, 10, w+10);
        
    };
    
    void createBox(ofMatrix4x4 *mat, float w, float h, float d){
        ofMesh _mesh = ofMesh::box(w, h, d, 1, 1, 1);
        int iNum = mesh.getNumVertices();
        
        for (int i = 0; i < _mesh.getNumVertices(); i++) {
            ofVec3f v = _mesh.getVertex(i);
            mesh.addVertex(v * *mat);
            mesh.addNormal( ((_mesh.getNormal(i) + v) * *mat - (v * *mat)).normalize());
            mesh.addColor(ofFloatColor(1.0, 0.0, 0.5));
        }
        
        for (int i = 0; i < _mesh.getNumIndices(); i++) {
            mesh.addIndex(_mesh.getIndex(i) + iNum);
        }
    };
    
private:
    ofVboMesh mesh;
    
};
