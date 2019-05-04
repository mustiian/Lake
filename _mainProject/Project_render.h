#ifndef RENDER_H
#define RENDER_H

#pragma once
#include "meshes\test.h"
#include "meshes\ground.h"
#include "meshes\skybox.h"
#include "data.h"
#include <iostream>
#include <vector>

extern const char* WIN_TITLE;

//Shader attrubutes/parametrs locations input
struct Shader {
	GLuint program;
	GLint positionLocation;
	GLint normalLocation;
	GLint texCoordLocation;

	GLint PVMmatrixLocation;
	GLint VmatrixLocation;
	GLint MmatrixLocation;
	GLint normalMatrixLocation;

	GLint directionLightLocation;
	GLint ambientLocation;
	GLint diffuseLocation;
	GLint specularLocation;
	GLint shininessLocation;
	GLint cameraPositionLocation; // (for flashlight)
	GLint cameraDirectionLocation; // (for flashlight)

	GLint useTextureLocation;
	GLint useSkyboxLocation;
	GLint useFlashlightLocation;
	GLint texSamplerLocation;
};

struct MeshGeometry {
	GLuint        vertexBufferObject;	// VBO
	GLuint        elementBufferObject;	// EBO
	GLuint        vertexArrayObject;	// VAO
	unsigned int  numTriangles;

	glm::vec3     ambient;
	glm::vec3     diffuse;
	glm::vec3     specular;
	float         shininess;
	GLuint		  texture;
};

struct Object {
	glm::vec3 position;		// position of object
	glm::vec3 direction;	// direction of object
	float size;				// size of object
	float angle;			// view angle of object
};

struct Objects {
	Object * tree;
	Object * ground;
	Object * skybox;
};

extern bool initShaderProgram();

extern void initTree(Shader &shader, MeshGeometry ** geometry);
extern void drawTree(Object *tree, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
extern void initGround(Shader &shader, MeshGeometry ** geometry);
extern void drawGround(Object *ground, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
extern void initSkybox(Shader &shader, MeshGeometry ** geometry);
extern void drawSkybox(Object *ground, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

extern void setTransform(const glm::mat4 & modelMatrix, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
extern void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture, bool useSkybox);
extern void deleteGeometry(MeshGeometry *geometry);
extern void cleanMeshes();
extern void initModels();

struct State {
	glm::mat4 projectionMatrix;			// projection transform matrix
	glm::mat4 viewMatrix;					// view transform matrix
	int lastMouseX, lastMouseY;
	int windowWidth;
	int windowHeight;
	bool firstInputMouse;
	bool useFlashlight;
	bool keyMap[KEYS_COUNT];
};

struct Camera {
	bool staticView;
	bool freeCamera;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 center;
	glm::vec3 up;
	float speed;
	float elevationAngle;
	float viewAngle;
};

extern MeshGeometry * treeGeometry;
extern MeshGeometry * groundGeometry;
extern MeshGeometry * skyboxGeometry;

extern Shader shaderProgram;
extern Objects objects;
extern Camera camera;
extern State gameState;

#endif