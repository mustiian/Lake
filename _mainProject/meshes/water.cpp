#include "water.h"
const int waterNAttribsPerVertex = 8;
const int waterNVertices = 4;
const int waterNTriangles = 2;
const float waterVertices[] = {
  -7.99133f,-7.99133f,0.0f,0.0f,0.0f,1.0f,0.0f,10.0f,
  7.99133f,-7.99133f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
  7.99133f,7.99133f,0.0f,0.0f,0.0f,1.0f,10.0f,0.0f,
  -7.99133f,7.99133f,0.0f,0.0f,0.0f,1.0f,10.0f,10.0f,
}; // end waterVertices

const unsigned waterTriangles[] = {
  0, 1, 2,
  0, 2, 3,
}; // end waterTriangles