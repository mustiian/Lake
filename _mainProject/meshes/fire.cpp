#include "fire.h"
const int fireNAttribsPerVertex = 8;
const int fireNVertices = 4;
const int fireNTriangles = 2;
const float fireVertices[] = {
  -1.0f,-1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
  1.0f,-1.0f,0.0f,0.0f,0.0f,1.0f,1.0f,0.0f,
  1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,
  -1.0f,1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,
}; // end fireVertices

const unsigned fireTriangles[] = {
  0, 1, 2,
  0, 2, 3,
}; // end fireTriangles

