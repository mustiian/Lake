#version 140

struct Material {   
	vec3 ambient;
};

uniform bool useFog;
uniform mat4 PVMmatrix;
uniform float time;

uniform sampler2D texSampler;

uniform Material material; 

smooth in vec2 texCoord_v; 
smooth in vec3 eyePos_v; 

out vec4 colorFragment;      

void main() {
	vec3 globalAmbientLight = vec3(0.6f);
	vec4 outColor = vec4(material.ambient * globalAmbientLight, 0.0);
	
	// Change position of texture 
	vec2 UV = vec2(texCoord_v.x, texCoord_v.y);
	float localTime = time * 1.5;
	float offsetX = floor( (localTime - floor(localTime)) * 34 ); 

	UV = vec2(UV.x / 34 + offsetX / 34, UV.y);

	outColor = texture(texSampler, UV);

	if (useFog){
		float fog_end = 50.0;
		float fog_start = 0.01;
		vec4  fog_colour = vec4(0.9, 0.4, 0.4, 0.0);

		float dist = length(eyePos_v.xyz);
		float fog_factor = (fog_end - dist)/(fog_end - fog_start);
		fog_factor = clamp(fog_factor, 0.0, 1.0);

		outColor = mix(fog_colour, outColor, fog_factor);
	}

	colorFragment = outColor;
}