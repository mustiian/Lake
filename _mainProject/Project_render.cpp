#include "pgr.h"
#include "Project_render.h"

const char* WIN_TITLE = "Jezero project";
Camera camera;
State gameState;

Shader shaderProgram;
Shader waterShaderProgram;
Shader fireShaderProgram;
MeshGeometry * treeGeometry = NULL;
MeshGeometry * groundGeometry = NULL;
MeshGeometry * skyboxGeometry = NULL;
MeshGeometry * waterGeometry = NULL;
MeshGeometry * fireGeometry = NULL;
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
	shaderProgram.useSkyboxLocation = glGetUniformLocation(shaderProgram.program, "material.useSkybox");
	shaderProgram.useFogLocation = glGetUniformLocation(shaderProgram.program, "useFog");

	shaderProgram.useFlashlightLocation = glGetUniformLocation(shaderProgram.program, "useFlashlight");

	if (shaderProgram.texSamplerLocation == -1 || shaderProgram.useTextureLocation == -1 || shaderProgram.texCoordLocation == -1) {
		std::cout << "Can't get all uniforms" << std::endl;
		return false;
	}

	if (!initDynamicObjectsShaderProgram(waterShaderProgram, "shaders/water.frag", "shaders/water.vert")) {
		std::cout << "Can't init water shader program" << std::endl;
		return false;
	}

	if (!initDynamicObjectsShaderProgram(fireShaderProgram, "shaders/fire.frag", "shaders/fire.vert")) {
		std::cout << "Can't init fire shader program" << std::endl;
		return false;
	}

	return true;
}
bool initDynamicObjectsShaderProgram(Shader & shaderPGR, const std::string & fragShader, const std::string & vertShader ) {
	std::vector<GLuint> shaders;

	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, vertShader));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, fragShader));
	if (shaders.size() != 2) {
		std::cout << "Can't load shaders files" << std::endl;
		return false;
	}

	shaderPGR.program = pgr::createProgram(shaders);
	if (shaderPGR.program == 0) {
		std::cout << "Can't create shader program" << std::endl;
		return false;
	}

	// vertex attr
	shaderPGR.positionLocation = glGetAttribLocation(shaderPGR.program, "position");
	shaderPGR.normalLocation = glGetAttribLocation(shaderPGR.program, "normal");
	shaderPGR.texCoordLocation = glGetAttribLocation(shaderPGR.program, "texCoord");

	// vertex uniforms
	shaderPGR.PVMmatrixLocation = glGetUniformLocation(shaderPGR.program, "PVMmatrix");
	shaderPGR.MmatrixLocation = glGetUniformLocation(shaderPGR.program, "modelMatrix");
	shaderPGR.VmatrixLocation = glGetUniformLocation(shaderPGR.program, "viewMatrix");
	shaderPGR.normalMatrixLocation = glGetUniformLocation(shaderPGR.program, "normalMatrix");

	// material
	shaderPGR.ambientLocation = glGetUniformLocation(shaderPGR.program, "material.ambient");
	shaderPGR.diffuseLocation = glGetUniformLocation(shaderPGR.program, "material.diffuse");
	shaderPGR.specularLocation = glGetUniformLocation(shaderPGR.program, "material.specular");
	shaderPGR.shininessLocation = glGetUniformLocation(shaderPGR.program, "material.shininess");

	// texture
	shaderPGR.texSamplerLocation = glGetUniformLocation(shaderPGR.program, "texSampler");
	shaderPGR.timeLocation = glGetUniformLocation(shaderPGR.program, "time");
	shaderPGR.useFogLocation = glGetUniformLocation(shaderPGR.program, "useFog");

	return true;
}

void initTree(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture("meshes/tree_new.jpg");

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

	(*geometry)->ambient = glm::vec3(0.4f, 0.2f, 0.1f);
	(*geometry)->diffuse = glm::vec3(0.3f, 0.2f, 0.0f);
	(*geometry)->specular = glm::vec3(0.2f, 0.2f, 0.2f);
	(*geometry)->shininess =30.0f;
	(*geometry)->numTriangles = treeNTriangles;

	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void initGround(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = 0;
	(*geometry)->texture = pgr::createTexture("meshes/grass_new.jpg");

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

	(*geometry)->ambient = glm::vec3(0.6f, 0.5f, 0.5f);
	(*geometry)->diffuse = glm::vec3(0.8f, 0.5f, 0.5f);
	(*geometry)->specular = glm::vec3(0.1f);
	(*geometry)->shininess = 1.0f;
	(*geometry)->numTriangles = groundNTriangles;
	
	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void initSkybox(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = 0;
	(*geometry)->texture = pgr::createTexture("meshes/skybox.png");

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// Vertex buffer 
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER,skyboxNVertices*skyboxNAttribsPerVertex * sizeof(float), skyboxVertices, GL_STATIC_DRAW);

	// Element buffer
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned)* skyboxNTriangles, skyboxTriangles, GL_STATIC_DRAW);

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

	(*geometry)->ambient = glm::vec3(0.9f, 0.9f, 0.9f);
	(*geometry)->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	(*geometry)->specular = glm::vec3(0.3f, 0.3f, 0.3f);
	(*geometry)->shininess = 10.0f;
	(*geometry)->numTriangles = skyboxNTriangles;

	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void initWater(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture("meshes/water.jpg");

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// Vertex buffer 
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, waterNVertices * waterNAttribsPerVertex * sizeof(float), waterVertices, GL_STATIC_DRAW);

	// Element buffer
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * waterNTriangles, waterTriangles, GL_STATIC_DRAW);

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

	(*geometry)->ambient = glm::vec3(0.9f, 0.9f, 0.9f);
	(*geometry)->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	(*geometry)->specular = glm::vec3(0.3f, 0.3f, 0.3f);
	(*geometry)->shininess = 10.0f;
	(*geometry)->numTriangles = waterNTriangles;

	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void initFire(Shader &shader, MeshGeometry ** geometry) {
	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture("meshes/fire.png");

	glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	// Vertex buffer 
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, fireNVertices * fireNAttribsPerVertex * sizeof(float), fireVertices, GL_STATIC_DRAW);

	// Element buffer
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned)* fireNTriangles, fireTriangles, GL_STATIC_DRAW);

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

	(*geometry)->ambient = glm::vec3(0.9f, 0.9f, 0.9f);
	(*geometry)->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	(*geometry)->specular = glm::vec3(0.3f, 0.3f, 0.3f);
	(*geometry)->shininess = 10.0f;
	(*geometry)->numTriangles = fireNTriangles;

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
		treeGeometry->texture,
		false
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
		groundGeometry->texture,
		false
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindVertexArray(groundGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, groundGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawSkybox(Object *skybox, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), skybox->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(skybox->size, skybox->size, skybox->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	setTransform(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		skyboxGeometry->ambient,
		skyboxGeometry->diffuse,
		skyboxGeometry->specular,
		skyboxGeometry->shininess,
		skyboxGeometry->texture,
		true
	);

	glBindVertexArray(skyboxGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, skyboxNTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawWater(Object *water, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(waterShaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), water->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(water->size, water->size, water->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// Set transform uniforms
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(waterShaderProgram.PVMmatrixLocation, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(waterShaderProgram.VmatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
	glUniformMatrix4fv(waterShaderProgram.MmatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));

	glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));
	glUniformMatrix4fv(waterShaderProgram.normalMatrixLocation, 1, GL_FALSE, value_ptr(normalMatrix));

	// Set material uniforms
	glUniform3fv(waterShaderProgram.diffuseLocation, 1, glm::value_ptr(waterGeometry->diffuse));
	glUniform3fv(waterShaderProgram.ambientLocation, 1, glm::value_ptr(waterGeometry->ambient));
	glUniform3fv(waterShaderProgram.specularLocation, 1, glm::value_ptr(waterGeometry->specular));
	glUniform1f(waterShaderProgram.shininessLocation, waterGeometry->shininess);
	glUniform1i(waterShaderProgram.useFogLocation, gameState.useFog);

	// Set texture
	glUniform1i(waterShaderProgram.texSamplerLocation, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, waterGeometry->texture);

	// Set time
	glUniform1f(waterShaderProgram.timeLocation, gameState.currentTime);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindVertexArray(waterGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, waterNTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

void drawFire(Object *fire, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(fireShaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), fire->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(fire->size, fire->size, fire->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Set transform uniforms
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(fireShaderProgram.PVMmatrixLocation, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(fireShaderProgram.VmatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
	glUniformMatrix4fv(fireShaderProgram.MmatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));

	glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));
	glUniformMatrix4fv(fireShaderProgram.normalMatrixLocation, 1, GL_FALSE, value_ptr(normalMatrix));

	// Set material uniforms
	glUniform3fv(fireShaderProgram.diffuseLocation, 1, glm::value_ptr(fireGeometry->diffuse));
	glUniform3fv(fireShaderProgram.ambientLocation, 1, glm::value_ptr(fireGeometry->ambient));
	glUniform3fv(fireShaderProgram.specularLocation, 1, glm::value_ptr(fireGeometry->specular));
	glUniform1f(fireShaderProgram.shininessLocation, fireGeometry->shininess);
	glUniform1i(fireShaderProgram.useFogLocation, gameState.useFog);

	// Set texture
	glUniform1i(fireShaderProgram.texSamplerLocation, 0);

	// Set time
	glUniform1f(fireShaderProgram.timeLocation, gameState.currentTime);

	glBindTexture(GL_TEXTURE_2D, fireGeometry->texture);
	glBindVertexArray(fireGeometry->vertexArrayObject);

	glDrawElements(GL_TRIANGLES, fireGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(0);
}

void setMaterialUniforms(const glm::vec3 & ambient, const glm::vec3 & diffuse, const glm::vec3 & specular, float shininess, GLuint texture, bool useSkybox) {
	// Set uniforms
	glUniform3fv(shaderProgram.diffuseLocation, 1, glm::value_ptr(diffuse));
	glUniform3fv(shaderProgram.ambientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
	glUniform1f(shaderProgram.shininessLocation, shininess);
	glUniform1i(shaderProgram.useFlashlightLocation, gameState.useFlashlight);
	glUniform1i(shaderProgram.useSkyboxLocation, useSkybox);
	glUniform1i(shaderProgram.useFogLocation, gameState.useFog);

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

	 glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

	//glm::mat4 normMatrix = glm::transpose(glm::inverse(modelMatrix));
	glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, value_ptr(normalMatrix));

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
	deleteGeometry(skyboxGeometry);
	deleteGeometry(waterGeometry);
	deleteGeometry(fireGeometry);
	pgr::deleteProgramAndShaders(shaderProgram.program);
	pgr::deleteProgramAndShaders(waterShaderProgram.program);
	pgr::deleteProgramAndShaders(fireShaderProgram.program);
}

void initModels() {
	initTree(shaderProgram, &treeGeometry);
	initSkybox(shaderProgram, &skyboxGeometry);
	initGround(shaderProgram, &groundGeometry);
	initWater(waterShaderProgram, &waterGeometry);
	initFire(fireShaderProgram, &fireGeometry);
}