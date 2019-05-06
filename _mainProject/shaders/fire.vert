#version 140

in vec3 position;  
in vec3 normal;
in vec2 texCoord;    

uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

smooth out vec2 texCoord_v; 
smooth out vec3 eyePos_v; 

void main() {

	eyePos_v = (viewMatrix * modelMatrix * vec4(position, 1.0f)).xyz;
	gl_Position = PVMmatrix * vec4(position, 1);

	texCoord_v = texCoord;
}