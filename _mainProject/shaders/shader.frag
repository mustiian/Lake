#version 140

struct Material {   
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;    
	float shininess;

	bool  useTexture; 
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;

uniform sampler2D texSampler;
uniform Material material; 

smooth in vec2 texCoord_v; 
smooth in vec3 eyePos_v; 
smooth in vec3 normal_v; 

out vec4 colorFragment;      

Light sun;

void initSun(){
	sun.ambient = vec3(0.5f);
	sun.diffuse = vec3(1.0f, 1.0f, 0.5f);
	sun.specular = vec3(1.0f);
	sun.position = (viewMatrix * vec4(1.0, 10.0, 10.0, 0.0)).xyz;
}

vec4 sunLight(Light light, vec3 vertexPosition, vec3 vertexNormal){
	// ambient
	 vec3 ambient = light.ambient * material.ambient;

	 // diffuse
	 vec3 normal = normalize(normal_v);
	 vec3 lightPos = normalize(light.position);
	 float diff = max(dot(normal, lightPos), 0.0);
	 vec3 diffuse = light.diffuse * material.diffuse * diff;

	 // specular
	 vec3 viewPos = normalize(-vertexPosition);
	 vec3 refl = reflect(-lightPos, vertexNormal);
	 float spec = max(dot(refl, viewPos), 0.0);
	 vec3 specular = light.specular * material.specular * pow(spec, material.shininess);

	 vec3 result = ambient + diffuse + specular;
	 return vec4(result, 1.0);
}

void main() {
	initSun();

	vec3 globalAmbientLight = vec3(0.4f);
	vec4 outColor = vec4(material.ambient * globalAmbientLight, 0.0);

	outColor += sunLight(sun, eyePos_v, normal_v);

	if(material.useTexture)
		outColor *= texture(texSampler, texCoord_v);

	colorFragment = outColor;
}