#include "Project_render.h"
#include "pgr.h"

void setCamera() {
	glm::vec3 cameraPosition = camera.position;
	glm::vec3 cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraCenter;
	glm::vec3 cameraViewDirection = camera.direction;

	glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(camera.elevationAngle), rotationAxis);

	cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
	cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));

	cameraCenter = cameraPosition + cameraViewDirection;

	gameState.viewMatrix = lookAt(
		cameraPosition,
		cameraCenter,
		cameraUpVector
	);

	gameState.projectionMatrix = glm::perspective(glm::radians (60.0f), float(gameState.windowWidth) / float(gameState.windowHeight), 0.1f, 300.0f);
}

void DisplayFunc (void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setCamera();

	drawSkybox(objects.skybox, gameState.viewMatrix, gameState.projectionMatrix);
	drawTree(objects.tree, gameState.viewMatrix, gameState.projectionMatrix);
	drawGround(objects.ground, gameState.viewMatrix, gameState.projectionMatrix);

	CHECK_GL_ERROR();
	glutSwapBuffers();
}

void Reshape(int width, int height) {
	gameState.windowWidth = width;
	gameState.windowHeight = height;
	glViewport(0, 0, gameState.windowWidth, gameState.windowHeight);
	gameState.projectionMatrix = glm::perspective(glm::radians(60.0f), float(gameState.windowWidth) / float(gameState.windowHeight), 1.0f, 300.0f);
}

void increaseCameraSpeed(float deltaSpeed) {

	camera.speed += deltaSpeed;
	if (camera.speed > 1.0f) {
		camera.speed = 1.0f;
	}
}

void decreaseCameraSpeed(float deltaSpeed) {

	camera.speed -= deltaSpeed;
	if (camera.speed < -1.0f) {
		camera.speed = -1.0f;
	}
}

void turnCameraRight(float deltaAngle) {

	camera.viewAngle += deltaAngle;

	if (camera.viewAngle > 360.0f) {
		camera.viewAngle -= 360.0f;
	}

	camera.direction.x = cos(glm::radians(camera.viewAngle));
	camera.direction.z = sin(glm::radians(camera.viewAngle));
}

void turnCameraLeft(float deltaAngle) {
	camera.viewAngle -= deltaAngle;

	if (camera.viewAngle < 0.0f) {
		camera.viewAngle += 360.0f;
	}
	camera.direction.x = cos(glm::radians(camera.viewAngle));
	camera.direction.z = sin(glm::radians(camera.viewAngle));
}

void PassiveMouse(int xpos, int ypos) {

	if (ypos != gameState.windowHeight / 2) {
		float cameraEAngleDelta = 0.5f * (ypos - gameState.windowHeight / 2);

		if (fabs(camera.elevationAngle - cameraEAngleDelta) < CAMERA_ELEVATION_MAX) {
			camera.elevationAngle -= cameraEAngleDelta;
		}
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

		glutPostRedisplay();
	}

	if (xpos != gameState.windowWidth / 2) {
		if (xpos < gameState.windowWidth / 2) {
			turnCameraLeft(1.0f);
		}
		else {
			turnCameraRight(1.0f);
		}
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

		glutPostRedisplay();
	}
}

void Mouse(int button, int status, int x, int y) {
	gameState.lastMouseX = x;
	gameState.lastMouseY = y;
}

void clean(void) {
	cleanMeshes();
	delete objects.tree;
	delete objects.skybox;
	delete objects.ground;
}

void Keyboard(unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case 27:
		clean();
		glutLeaveMainLoop();
		break;
	case 'f':
		std::cout << "Flashlight" << std::endl;
		gameState.useFlashlight = !gameState.useFlashlight;
		break;
	case 'g':
		std::cout << "Fog" << std::endl;
		gameState.useFog = !gameState.useFog;
		break;
	case '1':
		std::cout << "Static look" << std::endl;
		camera.staticView = !camera.staticView;
		if (camera.freeCamera) {
			camera.freeCamera = false;
			glutPassiveMotionFunc(NULL);
		}
		break;
	case '2':
		std::cout << "Dynamic look" << std::endl;
		camera.freeCamera = !camera.freeCamera;
		camera.staticView = false;
		if (camera.freeCamera) {
			glutPassiveMotionFunc(PassiveMouse);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		}
		else
			glutPassiveMotionFunc(NULL);
		break;
	}
}

void SpecialKeyboard(int key, int mouseX, int mouseY) {
	if (camera.staticView)
		return;

	switch (key) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	}
}

void SpecialKeyboardUp(int key, int mouseX, int mouseY) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	}
}

void Timer(int value) {
	glutTimerFunc(refreshTime, Timer, 0);

	if (gameState.keyMap[KEY_RIGHT_ARROW]) {
		turnCameraRight(2.0f);
	}
	if (gameState.keyMap[KEY_LEFT_ARROW]) {
		turnCameraLeft(2.0f);
	}
	if (gameState.keyMap[KEY_UP_ARROW]) {
		increaseCameraSpeed(0.1f);
	}
	if (gameState.keyMap[KEY_DOWN_ARROW]) {
		decreaseCameraSpeed(0.1f);
	}
	if (!gameState.keyMap[KEY_DOWN_ARROW] && !gameState.keyMap[KEY_UP_ARROW]) {
		if (camera.speed > 0 || camera.speed < 0) {
			camera.speed = 0.0f;
		}
	}
	camera.position += camera.speed * camera.direction;

	glutPostRedisplay();
}


void setAttr() {
	objects.tree = new Object();
	objects.tree->position = glm::vec3(10.0f, -3.f, -10.0f);
	objects.tree->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	objects.tree->size = 6.0f;
	objects.tree->angle = 0.0f;

	objects.ground = new Object();
	objects.ground->position = glm::vec3(5.0f, -1.0f, -0.55f);
	objects.ground->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	objects.ground->size = 6.0f;
	objects.ground->angle = 0.0f;

	objects.skybox = new Object();
	objects.skybox->position = glm::vec3(0.0f, 20.0f, 0.0f);
	objects.skybox->direction = glm::vec3(0.0f, 0.0f, 0.0f);
	objects.skybox->size = 100.0f;
	objects.skybox->angle = 0.0f;

	gameState.windowWidth = WIN_WIDTH;
	gameState.windowHeight = WIN_HEIGHT;

	camera.position = glm::vec3(0.0f, 1.0f, 0.0f);
	camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
	camera.direction = glm::vec3(0.0f, 0.0f, -1.0f);
	camera.speed = 0.05f;
	camera.viewAngle = 270.0f;

	gameState.useFog = false;
	gameState.useFlashlight = false;
	camera.staticView = false;
}

bool init(void) {

	glEnable(GL_DEPTH_TEST);

	if (!initShaderProgram())
		return false;

	initModels();

	setAttr();

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	CHECK_GL_ERROR();
	return true;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutCreateWindow(WIN_TITLE);

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(Reshape);

	//glutPassiveMotionFunc(PassiveMouseMotion);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutSpecialUpFunc(SpecialKeyboardUp);

	glutTimerFunc(refreshTime, Timer, 0);

	// initialize pgr framework
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	if (!init())
		pgr::dieWithError("init failed, cannot continue");

	glutMainLoop();

	return 1;
}