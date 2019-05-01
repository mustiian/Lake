#include "pgr.h"
#include "Project_render.h"

const char* WIN_TITLE = "Jezero project";
Camera camera;
State gameState;

Shader shaderProgram;
MeshGeometry * treeGeometry = NULL;
MeshGeometry * groundGeometry = NULL;
Objects objects;

bool initShaderProgram() {
	std::vector<GLuint> shaders;

	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER,"shaders/shader.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER,"shaders/shader.frag"));
	if (shaders.size() != 2) {
		std::cout << "Can''t load shaders files" << std::endl;
		return false;
	}

	shaderProgram.program = pgr::createProgram(shaders);
	if (shaderProgram.program == 0) {
		std::cout << "Can''t create shader program" << std::endl;
		return false;
	}
	// vertex attr
	shaderProgram.positionLocation = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.normalLocation = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");

	// vertex uniforms
	shaderProgram.PVMmatrixLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.MmatrixLocation = glGetUniformLocation(shaderProgram.program, "modelMatrix");
	shaderProgram.VmatrixLocation = glGetUniformLocation(shaderProgram.program, "viewMatrix");
	shaderProgram.normalMatrixLocation = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	shaderProgram.cameraPositionLocation = glGetUniformLocation(shaderProgram.program, "cameraPosition");
	shaderProgram.cameraDirectionLocation = glGetUniformLocation(shaderProgram.program, "cameraDirection");

	// material
	shaderProgram.ambientLocation = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.diffuseLocation = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.specularLocation = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.shininessLocation = glGetUniformLocation(shaderProgram.program, "material.shininess");

	// texture
	shaderProgram.texSamplerLocation = glGetUniformLocation(shaderProgram.program, "texSampler");
	shaderProgram.useTextureLocation = glGetUniformLocation(shaderProgram.program, "material.useTexture");

	shaderProgram.useFlashlightLocation = glGetUniformLocation(shaderProgram.program, "useFlashlight");

	if (shaderProgram.texSamplerLocation == -1 || shaderProgram.useTextureLocation == -1 || shaderProgram.texCoordLocation == -1) {
		std::cout << "Can''t get all uniforms" << std::endl;
		return false;
	}

	return true;
}

void initTree(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = 0;
	(*geometry)->texture = pgr::createTexture("meshes/tree.jpg");

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// Vertex buffer 
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, treeNVertices*treeNAttribsPerVertex * sizeof(float), treeVertices, GL_STATIC_DRAW);

	// Element buffer
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned)* treeNTriangles, treeTriangles, GL_STATIC_DRAW);

	// Get position location
	glEnableVertexAttribArray(shader.positionLocation);
	glVertexAttribPointer(shader.positionLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	CHECK_GL_ERROR();
	// Get normal location
	glEnableVertexAttribArray(shader.normalLocation);
	glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// Get texture location
	glEnableVertexAttribArray(shader.texCoordLocation);
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	(*geometry)->ambient = glm::vec3(0.4f, 0.1f, 0.0f);
	(*geometry)->diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	(*geometry)->specular = glm::vec3(0.6f, 0.3f, 0.3f);
	(*geometry)->shininess = 1.0f;
	(*geometry)->numTriangles = treeNTriangles;

	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void initGround(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = 0;
	(*geometry)->texture = pgr::createTexture("meshes/grass.png");


	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// Vertex buffer 
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, groundNVertices*groundNAttribsPerVertex * sizeof(float), groundVertices, GL_STATIC_DRAW);

	// Element buffer
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned)* groundNTriangles, groundTriangles, GL_STATIC_DRAW);

	// Get position location
	glEnableVertexAttribArray(shader.positionLocation);
	glVertexAttribPointer(shader.positionLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	// Get normal location
	glEnableVertexAttribArray(shader.normalLocation);
	glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	// Get texture location
	glEnableVertexAttribArray(shader.texCoordLocation);
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	(*geometry)->ambient = glm::vec3(0.5f, 0.6f, 0.2f);
	(*geometry)->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	(*geometry)->specular = glm::vec3(0.3f, 0.4f, 0.3f);
	(*geometry)->shininess = 0.5f;
	(*geometry)->numTriangles = groundNTriangles;
	
	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void drawTree(Object *tree, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tree->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tree->size, tree->size, tree->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	setTransform(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		treeGeometry->ambient,
		treeGeometry->diffuse,
		treeGeometry->specular,
		treeGeometry->shininess,
		treeGeometry->texture
	);

	glBindVertexArray(treeGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, treeNTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawGround(Object *ground, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), ground->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(ground->size, ground->size, ground->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	setTransform(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		groundGeometry->ambient,
		groundGeometry->diffuse,
		groundGeometry->specular,
		groundGeometry->shininess,
		groundGeometry->texture
	);

	glBindVertexArray(groundGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, groundNTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

void setMaterialUniforms(const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular, float shininess, GLuint texture) {
	// Set uniforms
	glUniform3fv(shaderProgram.diffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(shaderProgram.ambientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
	glUniform1f(shaderProgram.shininessLocation, shininess);
	glUniform1f(shaderProgram.useFlashlightLocation, gameState.useFlashlight);

	// Set texture
	if (texture != 0) {
		glUniform1i(shaderProgram.useTextureLocation, 1);
		glUniform1i(shaderProgram.texSamplerLocation, 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texture);
	} else {
		glUniform1i(shaderProgram.useTextureLocation, 0);
	}

	CHECK_GL_ERROR();
}

void setTransform(const glm::mat4 & modelMatrix, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	// Set transform attributes
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));

	glm::mat4 normMatrix = glm::transpose(glm::inverse(modelMatrix));
	glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, value_ptr(normMatrix));

	// Camera
	glUniform3fv(shaderProgram.cameraPositionLocation, 1, glm::value_ptr(camera.position));

	glm::vec3 cameraViewDirection = camera.direction;
	glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(camera.elevationAngle), rotationAxis);

	cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));
	glUniform3fv(shaderProgram.cameraDirectionLocation, 1, glm::value_ptr(cameraViewDirection));
}

void deleteGeometry(MeshGeometry *geometry) {
	glDeleteVertexArrays(1, &geometry->vertexArrayObject);
	glDeleteBuffers(1, &geometry->vertexBufferObject);
	glDeleteBuffers(1, &geometry->elementBufferObject);
}

void cleanMeshes() {
	deleteGeometry(treeGeometry);
	deleteGeometry(groundGeometry);
	pgr::deleteProgramAndShaders(shaderProgram.program);
}

void initModels() {
	initGround(shaderProgram, &groundGeometry);
	initTree(shaderProgram, &treeGeometry);
}