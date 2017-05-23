#include "ofMain.h"

class GenCells {
public:
    void setup() {
        createMesh();
        
    };
    void draw() {
        mesh.draw();
    };
    void createMesh(){
        
        int n = 32;
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    
                    float noise = ofNoise(i * 0.03, j * 0.03, k * 0.03);
                    float rand = ofRandom(1.0);
                    
                    if (noise > 0.5 && rand > 0.7) {
                        ofMatrix4x4 mat;
                        mat.glScale(80, 80, 80);
                        mat.glTranslate(i - n/2, j-n/2, k-n/2);
                        mat.glScale(0.3 + noise, 0.3 + noise, 0.3 + noise);
                        createRoom(&mat);
                    }
                }
            }
        }
        
    };
    void createRoom(ofMatrix4x4 *mat) {
        
        if (ofRandom(1.0) < 0.4) createBox(mat, 0.2, 0.2, 0.2, true);
        
        for (int i = 0; i < 4; i++) {
            ofMatrix4x4 m = *mat;
            m.glRotate(90*i, 0, 1.0, 0);
            m.glTranslate(0, 0, 0.5);
            
            createWall(&m, 1.0, 1.0);
        }
        
        for (int i = 0; i < 2; i++) {
            ofMatrix4x4 m = *mat;
            m.glRotate(90*pow(-1.0, i), 1.0, 0, 0);
            m.glTranslate(0, 0, 0.5);
            
            createWall(&m, 1.0, 1.0);
        }
        
    }
    
    void createWall(ofMatrix4x4 *mat, float w, float h){
        
        float rand = ofRandom(1.0);
        if (rand < 0.2) {
            return;
        } else if (rand < 0.6) {
            createBox(mat, w, h, 0.05, false);
        } else {
            int columnNum = ceil(ofRandom(6.0));
            
            for (int i = 0; i < columnNum+1; i++) {
                ofMatrix4x4 m = *mat;
                m.glTranslate(0,0, w/columnNum * i - w/2);
                createBox(&m, 0.05, h, 0.05, false);
            }
            
            ofMatrix4x4 _m = *mat;
            _m.glTranslate(0, -h/2, 0);
            createBox(&_m, 0.05, 0.05, w, false);
            
            _m = *mat;
            _m.glTranslate(0, h/2, 0);
            createBox(&_m, 0.05, 0.05, w, false);
            
        }
        
    };
    
    void createBox(ofMatrix4x4 *mat, float w, float h, float d, bool light){
        ofMesh _mesh = ofMesh::box(w, h, d, 1, 1, 1);
        int iNum = mesh.getNumVertices();
        ofFloatColor c;
        if (light) c.set(1.1, 1.1, 1.0);
        else c.setHsb(0.5, 0.1, 0.2);
        
        for (int i = 0; i < _mesh.getNumVertices(); i++) {
            ofVec3f v = _mesh.getVertex(i);
            mesh.addVertex(v * *mat);
            mesh.addNormal( ((_mesh.getNormal(i) + v) * *mat - (v * *mat)).normalize());
            mesh.addColor(c);
        }
        
        for (int i = 0; i < _mesh.getNumIndices(); i++) {
            mesh.addIndex(_mesh.getIndex(i) + iNum);
        }
    };
    
private:
    ofVboMesh mesh;
    
    
};
