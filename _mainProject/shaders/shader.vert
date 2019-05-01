#version 140

in vec3 position;  
in vec3 normal;
in vec2 texCoord;    

uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

smooth out vec2 texCoord_v; 
smooth out vec3 eyePos_v; 
smooth out vec3 normal_v; 

void main() {
	normal_v = normalize ((viewMatrix * modelMatrix * vec4(normal, 0.0f)).xyz);
	eyePos_v = (viewMatrix * modelMatrix * vec4(position, 0.0f)).xyz;
	gl_Position = PVMmatrix * vec4(position, 1);

	texCoord_v = texCoord;
}