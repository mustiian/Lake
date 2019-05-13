//----------------------------------------------------------------------------------------
/**
* \file       Project_render.h
* \author     Ian Mustiats
* \date       07.05.2019
*/
//----------------------------------------------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

#pragma once
#include "meshes\test.h"
#include "meshes\ground.h"
#include "meshes\skybox.h"
#include "meshes\water.h"
#include "meshes\fire.h"
#include "meshes\boat.h"
#include "meshes\dock.h"
#include "meshes\greenTree.h"
#include "meshes\sticks.h"
#include "meshes\cactus.h"
#include "data.h"
#include "pgr.h"
#include <iostream>
#include <vector>

/// Name of the window
extern const char* WIN_TITLE;
/// Start color of the fire
extern const glm::vec3 fireColor;
/// Ambient color of the fire
extern glm::vec3 fireAmbient;
/// Diffuse color of the fire
extern glm::vec3 fireDiffuse;
/// Specualar color of the fire
extern glm::vec3 fireSpecular;

/** shader attrubutes/parametrs locations */
typedef struct Shader {
	GLuint program;					// shader program
	GLint positionLocation;			// position fo the vertex
	GLint normalLocation;			// normal of the vertex
	GLint texCoordLocation;			// texture coordinates of the vertex

	GLint PVMmatrixLocation;		// Projection View Model matrix
	GLint VmatrixLocation;			// view matrix
	GLint MmatrixLocation;			// model matrix
	GLint normalMatrixLocation;		// normal matrix

	GLint directionLightLocation;	// direction fo the light
	GLint ambientLocation;			// material ambient
	GLint diffuseLocation;			// material diffuse
	GLint specularLocation;			// material specular
	GLint shininessLocation;		// material shininess
	GLint cameraPositionLocation;	// camera position (for flashlight)
	GLint cameraDirectionLocation;	// camera direction (for flashlight)

	GLint firePositionLocation;		// position of the fire
	GLint fireAmbientLocation;		// material ambient fo the fire
	GLint fireDiffuseLocation;		// material diffuse of the fire
	GLint fireSpecularLocation;		// material specular of the fire

	GLint useTextureLocation;		// use texture
	GLint useSkyboxLocation;		// use skybox
	GLint useFogLocation;			// use fog
	GLint useFlashlightLocation;	// use flashlight
	GLint useFireLocation;			// use fire
	GLint texSamplerLocation;		// texture sampler

	GLint timeLocation;				// current time
}Shader;

/** mesh with buffers, materials and texture*/
typedef struct MeshGeometry {
	GLuint        vertexBufferObject;	// VBO
	GLuint        elementBufferObject;	// EBO
	GLuint        vertexArrayObject;	// VAO
	unsigned int  numTriangles;			// number of triangles

	glm::vec3     ambient;				// material ambient
	glm::vec3     diffuse;				// material diffuse
	glm::vec3     specular;				// material specular
	float         shininess;			// material shininess
	GLuint		  texture;				// texture
	GLuint		  alternativeTexture;	// Alternative texture
}MeshGeometry;

/** object parameters */
typedef struct Object {
	glm::vec3 position;		// position of object
	glm::vec3 direction;	// direction of object
	float size;				// size of object
	float angle;			// view angle of object
}object;

/** all objects of the scene*/
typedef struct Objects {
	Object * tree;
	Object * ground;
	Object * skybox;
	Object * water;
	Object * fire;
	Object * boat;
	Object * dock;
	Object * sticks;
	Object * cactus;
	std::vector <Object *> greenTrees;
}Objects;

/** state of the scene*/
typedef struct State {
	glm::mat4 projectionMatrix;			// projection transform matrix
	glm::mat4 viewMatrix;				// view transform matrix
	int windowWidth;					// window width
	int windowHeight;					// window height
	bool useFlashlight;					// use flashlight
	bool useFog;						// use fog
	bool useBoat;						// use boat
	bool useFIre;						// use fire
	bool useSticks;						// use sticks
	bool useSandTheme;					// use sand theme
	float elapsedTime;					// elapsed time
	float currentTime;					// current time
	float lastFireColorUpdate;			// last time of fire color update
	bool keyMap[KEYS_COUNT];			// key map
}State;

/** state of the camera*/
typedef struct Camera {
	bool firstView;						// use first static view
	bool secondView;					// use second static view 
	bool freeCamera;					// use free camera
	bool collision;						// check collision with objects
	glm::vec3 position;					// position of the camera
	glm::vec3 oldPosition;				// Previous position of the camera
	glm::vec3 direction;				// direction of the camera
	glm::vec3 center;					// center of the camera
	glm::vec3 up;						// up of the camera
	float speed;						// speed of the camera
	float elevationAngle;				// elevation angle 
	float viewAngle;					// view angle
}Camera;

/**
 Initializes all parametrs of the main shader
*/
extern bool initShaderProgram();
/**
 Initializes all parametrs of the shader with dynamic texture

 \param[out] shaderProgram		Shader object
 \param[in] fragShader			Path to the fragment shader file
 \param[in] vertShader			Path to the vertex shader file

 \return true if the shader was initialized
*/
extern bool initDynamicObjectsShaderProgram(Shader & shaderProgram, const std::string & fragShader, const std::string & vertShader);

/**
 Initializes all parametrs of the fire shader

 \param[out] shaderProgram		Shader object
 \param[in] fragShader			Path to the fragment shader file
 \param[in] vertShader			Path to the vertex shader file

 \return true if the shader was initialized
*/
extern bool initFireShaderProgram(Shader & shaderPRG, const std::string & fragShader, const std::string & vertShader);

/**
 Initializes the geometry of the tree

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initTree(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the tree

 \param[in] tree			 Tree object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawTree(Object *tree, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the ground

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initGround(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the ground

 \param[in] ground			 Ground object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawGround(Object *ground, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the skybox

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initSkybox(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the skybox

 \param[in] skybox			 Skybox object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawSkybox(Object *skybox, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the water

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initWater(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the water

 \param[in] tree			 Water object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawWater(Object *water, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the fire

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initFire(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the fire

 \param[in] tree			 Fire object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawFire(Object *fire, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the boat

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initBoat(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the boat

 \param[in] tree			 Boat object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawBoat(Object *boat, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the dock

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initDock(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the dock

 \param[in] tree			 Dock object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawDock(Object *dock, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the green tree

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initGreenTree(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the green tree

 \param[in] tree			 Green tree object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawGreenTree(Object *dock, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the sticks for the fire

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initSticks(Shader &shader, MeshGeometry ** geometry);

/**
 Draw sticks for the fire

 \param[in] tree			 Sticks object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawSticks(Object *sticks, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Initializes the geometry of the cactus

 \param[in] shader			Shader object
 \param[out] geometry		Geometry that contains all information about object
*/
extern void initCactus(Shader &shader, MeshGeometry ** geometry);

/**
 Draw the cactus

 \param[in] cactus			 Cactus object
 \param[in] viewMatrix		 View matrix
 \param[in] projectionMatrix Projection matrix
*/
extern void drawCactus(Object *cactus, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Sets the transforms uniforms to the shader

\param[in] modelMatrix			Model matrix
\param[in] viewMatrix			View matrix
\param[in] projectionMatrix		Projection matrix
*/
extern void setTransform(const glm::mat4 & modelMatrix, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

/**
 Sets the material uniforms to the shader

\param[in] ambient			Model matrix
\param[in] diffuse			View matrix
\param[in] specular			Material specular
\param[in] shininess		Material shininess
\param[in] texture			Texture location
\param[in] useSkybox		Use skybox
*/
extern void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture, bool useSkybox);

/**
 Deletes geometry

 \param[in] geometry	Geometry object	
*/
extern void deleteGeometry(MeshGeometry *geometry);

/**
 Deletes all meshes in the scene
*/
extern void cleanMeshes();

/**
 Initializes all models in the scene
*/
extern void initModels();

extern MeshGeometry * treeGeometry;
extern MeshGeometry * groundGeometry;
extern MeshGeometry * skyboxGeometry;
extern MeshGeometry * waterGeometry;
extern MeshGeometry * fireGeometry;
extern MeshGeometry * boatGeometry;
extern MeshGeometry * dockGeometry;
extern MeshGeometry * greenTreeGeometry;
extern MeshGeometry * sticksGeometry;
extern MeshGeometry * cactusGeometry;

/// Main shader
extern Shader shaderProgram;

/// Water shader
extern Shader waterShaderProgram;

/// Fire shader
extern Shader fireShaderProgram;

/// All objects
extern Objects objects;

/// Main camera
extern Camera camera;

/// State object
extern State gameState;

#endif