#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"


Scene::Scene()
{
	quad = NULL;
}

Scene::~Scene()
{
	if (quad != NULL)
		delete quad;
}


void Scene::init()
{
	initShaders();
	quad = Quad::createQuad(-64.f, -64.f, 128.f, 128.f, program);
	projection = glm::ortho(0.f, float(CAMERA_WIDTH - 1), float(CAMERA_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
}

void Scene::render()
{
	glm::mat4 modelview;

	// We can now, using matrices, draw four quads at different screen locations
	// using a single Quad object.
	program.use();
	program.setUniformMatrix4f("projection", projection);
	program.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	modelview = glm::translate(glm::mat4(1.0f), glm::vec3(320.f, 240.f, 0.f)); // despacament orbita
	modelview = glm::rotate(modelview, -currentTime / 1000.f, glm::vec3(0.0f, 0.0f, 1.0f)); // rotacio orbita
	modelview = glm::translate(modelview, glm::vec3(100.f, 0.f, 0.f)); // mou al centre de la pantalla
	modelview = glm::rotate(modelview, -currentTime / 1000.f, glm::vec3(0.0f, 0.0f, 1.0f)); // mov rotacio
	program.setUniformMatrix4f("modelview", modelview);
	quad->render();

	modelview = glm::rotate(modelview, -currentTime / 2000.f, glm::vec3(0.0f, 0.0f, 1.0f)); // mov rotacio
	modelview = glm::translate(modelview, glm::vec3(125.f, 0.f, 0.f)); // despacament orbita
	modelview = glm::scale(modelview, glm::vec3(0.5, 0.5, 0.5));
	program.setUniformMatrix4f("modelview", modelview);
	quad->render();

}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/simple.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/simple.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	program.init();
	program.addShader(vShader);
	program.addShader(fShader);
	program.link();
	if (!program.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << program.log() << endl << endl;
	}
	program.bindFragmentOutput("outColor");
}

