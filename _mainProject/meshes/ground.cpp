#include "ground.h"
const int groundNAttribsPerVertex = 8;
const int groundNVertices = 4;
const int groundNTriangles =  2;

const float groundVertices[] = {
  -8.0f,-8.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
  8.0f,-8.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,
  8.0f,8.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,
  -8.0f,8.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,
}; // end planeVertices

const unsigned groundTriangles[] = {
  0, 1, 2,
  0, 2, 3,
}; // end planeTriangles

