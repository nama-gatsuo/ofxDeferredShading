#include "ofMain.h"

class GenCells {
public:
	void setup() {
		createMesh();

	};
	void draw() {
		mesh.draw();
	};
	void createMesh() {

		int n = 32;

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				for (int k = 0; k < n; k++) {

					float noise = ofNoise(i * 0.03, j * 0.03, k * 0.03);
					float rand = ofRandom(1.0);

					if (noise > 0.5 && rand > 0.7) {
						glm::mat4 mat = glm::scale(glm::vec3(80.f));
						mat = mat * glm::translate(glm::vec3(i - n / 2, j - n / 2, k - n / 2));
						mat = mat * glm::scale(glm::vec3(0.3 + noise));

						createRoom(mat);
					}
				}
			}
		}

	};
	void createRoom(const glm::mat4& mat) {

		if (ofRandom(1.0) < 0.4) createBox(mat, 0.2, 0.2, 0.2, true);

		for (int i = 0; i < 4; i++) {
			glm::mat4 m(mat);
			m = m * glm::rotate(float(HALF_PI * float(i)), glm::vec3(0.f, 1.f, 0.f));
			m = m * glm::translate(glm::vec3(0, 0, 0.5));

			createWall(m, 1.0, 1.0);
		}

		for (int i = 0; i < 2; i++) {
			glm::mat4 m(mat);
			m = m * glm::rotate(float(HALF_PI * pow(-1.0, i)), glm::vec3(1.f, 0.f, 0.f));
			m = m * glm::translate(glm::vec3(0, 0, 0.5));

			createWall(m, 1.0, 1.0);
		}

	}

	void createWall(const glm::mat4& mat, float w, float h) {

		float rand = ofRandom(1.0);
		if (rand < 0.2) {
			return;
		}
		else if (rand < 0.6) {
			createBox(mat, w, h, 0.05, false);
		}
		else {
			int columnNum = ceil(ofRandom(6.0));

			for (int i = 0; i < columnNum + 1; i++) {
				glm::mat4 m(mat);
				m = m * glm::translate(glm::vec3(0, 0, w / columnNum * i - w / 2));
				createBox(m, 0.05, h, 0.05, false);
			}

			glm::mat4 _m(mat);
			_m = _m * glm::translate(glm::vec3(0, -h / 2, 0));
			createBox(_m, 0.05, 0.05, w, false);

			_m = glm::mat4(mat);
			_m = _m * glm::translate(glm::vec3(0, h / 2, 0));
			createBox(_m, 0.05, 0.05, w, false);

		}

	};

	void createBox(const glm::mat4& mat, float w, float h, float d, bool light) {
		ofMesh _mesh = ofMesh::box(w, h, d, 1, 1, 1);

		ofFloatColor c;
		if (light) c.set(2.f);
		else c.setHsb(0.5, 0.2, 0.3);

		glm::mat4 normMat = glm::inverse(glm::transpose(mat));

		for (int i = 0; i < _mesh.getNumVertices(); i++) {
			_mesh.setVertex(i, mat * glm::vec4(_mesh.getVertex(i), 1.f));
			glm::vec3 n = normMat * glm::vec4(_mesh.getNormal(i), 0.f);
			_mesh.setNormal(i, glm::normalize(n));
			_mesh.addColor(c);
		}

		mesh.append(_mesh);
	};

private:
	ofVboMesh mesh;


};
