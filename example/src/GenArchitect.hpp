#include "ofMain.h"

class GenArchitecture {
public:
	void setup() {
		createMesh();
	};
	void draw() {
		mesh.draw();
	};
	void createMesh() {

		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 3; j++) {

				glm::mat4 mat = glm::translate(glm::vec3(0, 100.f * ((float)i - 16.f), 0));
				createRoom(mat, ofRandom(2.0, 5.0) * 80.);
			}
		}

	};
	void createRoom(const glm::mat4& mat, float r) {
		int edgeNum = ceil(ofRandom(2.0, 4.0)) * 2;
		float wallWidth = ofRandom(0.2, 2.0) * r;
		float wallHeight = 100 * ofRandom(0.1, 4.0);
		for (int i = 0; i < edgeNum; i++) {

			glm::mat4 m(mat);
			m = m * glm::rotate(float(TWO_PI / edgeNum * i), glm::vec3(0.f, 1.f, 0.f));
			m = m * glm::translate(glm::vec3(r, 0, 0));

			createWall(m, wallWidth, wallHeight);
		}

	}

	void createWall(const glm::mat4& mat, float w, float h) {

		if (ofRandom(1.0) < 0.6) createBox(mat, 5, h, w);
		int columnNum = ceil(ofRandom(6.0));

		for (int i = 0; i < columnNum + 1; i++) {
			glm::mat4 m(mat);
			m = m * glm::translate(glm::vec3(0, 0, w / columnNum * i - w / 2));
			createBox(m, 10, h, 10);
		}

		float t = 10.f * ofRandom(1, 3);

		glm::mat4 _m(mat);
		_m = _m * glm::translate(glm::vec3(-t / 2.f, -h / 2.f, 0));
		createBox(_m, t, 10.f, w + 10.f);

		_m = glm::mat4(mat);
		_m = _m * glm::translate(glm::vec3(-t / 2.f, h / 2.f, 0));
		createBox(_m, t, 10, w + 10);

	};

	void createBox(const glm::mat4& mat, float w, float h, float d) {
		ofMesh _mesh = ofMesh::box(w, h, d, 1, 1, 1);
		glm::mat4 normalMatrix = glm::inverse(glm::transpose(mat));

		for (int i = 0; i < _mesh.getNumVertices(); i++) {
			_mesh.setVertex(i, mat * glm::vec4(_mesh.getVertex(i), 1.f));
			_mesh.setNormal(i, normalMatrix * glm::vec4(_mesh.getNormal(i), 1.f));
			_mesh.addColor(ofFloatColor(.9f, .8f, 0.8f));
		}

		mesh.append(_mesh);
	}

private:
	ofVboMesh mesh;

};
