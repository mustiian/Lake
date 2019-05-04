#version 140

struct Material {   
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;    
	float shininess;
};

struct Light {
    vec3 position; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform bool useFog;
uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform float time;

uniform sampler2D texSampler;

uniform Material material; 

smooth in vec2 texCoord_v; 
smooth in vec3 eyePos_v; 
smooth in vec3 normal_v; 

out vec4 colorFragment;      

Light sun;

void initSun(){
	sun.ambient = vec3(0.5f);
	sun.diffuse = vec3(1.0f, 0.5f, 0.5f);
	sun.specular = vec3(2.0f);
	sun.position = (viewMatrix * vec4(10.0, -10.0, 0.0, 0.0)).xyz;
}

vec4 directionLight(Light light, vec3 vertexPosition, vec3 vertexNormal){
	// ambient
	 vec3 ambient = light.ambient * material.ambient;

	 vec3 normal = vertexNormal;

	 // diffuse
	 vec3 lightPos = normalize(light.position);
	 float diff = max(dot(normal, lightPos), 0.0);
	 vec3 diffuse = light.diffuse * material.diffuse * diff;

	 // specular
	 vec3 refl = reflect(-lightPos, normal);
	 vec3 viewPos = normalize(-vertexPosition);
	 float spec = max(dot(refl, viewPos), 0.0);
	 vec3 specular = light.specular * material.specular * pow(spec, material.shininess);

	 vec3 result = ambient + diffuse + specular;
	 return vec4(result, 1.0);
}

void main() {
	initSun();

	vec3 globalAmbientLight = vec3(0.6f);
	vec4 outColor = vec4(material.ambient * globalAmbientLight, 0.0);

	outColor += directionLight(sun, eyePos_v, normal_v);
	
	// Change position of texture 
	vec2 UV = vec2(texCoord_v.x, texCoord_v.y);
	float localTime = time * 0.05;
	float offsetX = UV.x + (localTime - floor(localTime)); 
	if(offsetX > 1.0f)
		offsetX = offsetX - 1.0f;

	UV = vec2(UV.x + offsetX, UV.y);

	outColor *= texture(texSampler, UV).rgb;

	if (useFog){
		float fog_end = 30.0;
		float fog_start = 0.01;
		vec4  fog_colour = vec4(0.7, 0.7, 0.7, 1.0);

		float dist = length(eyePos_v.xyz);
		float fog_factor = (fog_end - dist)/(fog_end - fog_start);
		fog_factor = clamp(fog_factor, 0.0, 1.0);

		outColor = mix(fog_colour, outColor, fog_factor);
	}

	colorFragment = outColor;
}