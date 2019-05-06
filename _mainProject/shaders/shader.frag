#version 140

struct Material {   
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;    
	float shininess;

	bool useTexture; 
	bool useSkybox;
	bool useReflector;
};

struct Light {
	vec3 direction;
    vec3 position; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float spotCosAngle;
	float spotExponent; 
};

uniform bool useFlashlight;
uniform bool useFog;
uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;

uniform sampler2D texSampler;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform Material material; 

uniform vec3 fireAmbient;
uniform vec3 fireDiffuse;
uniform vec3 fireSpecular;

smooth in vec2 texCoord_v; 
smooth in vec3 eyePos_v; 
smooth in vec3 normal_v; 

out vec4 colorFragment;      

Light sun;
Light spotlight;
Light fireLight;

void initSun(){
	sun.ambient = vec3(0.5f);
	sun.diffuse = vec3(1.0f, 0.5f, 0.5f);
	sun.specular = vec3(1.0f);
	sun.position = (viewMatrix * vec4(-100.0, 100.0, -100.0, 0.0)).xyz;
}

void initSpotlight(){
	spotlight.ambient = vec3(1.0f);
	spotlight.diffuse = vec3(0.5f, 0.5f, 0.5f);
	spotlight.specular = vec3(0.0);

	spotlight.position = (viewMatrix * vec4(cameraPosition, 1.0)).xyz;
	spotlight.direction = normalize((viewMatrix * vec4(cameraDirection, 0.0)).xyz);

	spotlight.spotCosAngle = 0.9f;
	spotlight.spotExponent = 5.0f;
}

void initFireLight(){
	fireLight.ambient = fireAmbient;
	fireLight.diffuse = fireDiffuse;
	fireLight.specular = fireSpecular;
	fireLight.position = (viewMatrix * vec4(80.0f, -1.0f, 90.0f, 1.0)).xyz;
}

vec4 spotlightLight(Light light, vec3 vertexPosition, vec3 vertexNormal){
	// ambient
	 vec3 ambient = light.ambient * material.ambient;

	 // diffuse
	 vec3 lightDir = normalize(light.position - vertexPosition);
	 float diff = max(dot(vertexNormal, lightDir), 0.0f);
	 vec3 diffuse = material.diffuse * light.diffuse * diff;

	 //specular
	 vec3 refl = reflect(-lightDir, vertexNormal);
	 vec3 toEyes = normalize(-vertexPosition);
	 float spec = max(dot(refl, toEyes), 0.0f);
	 vec3 specular = material.specular * light.specular * pow(spec, material.shininess);

	 vec3 result = ambient + diffuse + specular;

	 float spotCoef = max(dot(-lightDir, light.direction), 0.0f);

	 if (spotCoef < light.spotCosAngle){
		result *= 0.0f;
	 }
	 else{
		result *= pow(spotCoef, light.spotExponent);
	 }

	 return vec4(result, 1.0);
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

vec4 pointLight(Light light, vec3 vertexPosition, vec3 vertexNormal) {

	float distance = distance(light.position, vertexPosition);
	float attenuation = 1.0f / (1.5f * distance);
	return attenuation * directionLight(light, vertexPosition, vertexNormal);
}

void main() {
	initSun();
	initSpotlight();
	initFireLight();

	vec3 globalAmbientLight = vec3(0.4f);
	vec4 outColor = vec4(material.ambient * globalAmbientLight, 0.0);

	if (useFlashlight)
		outColor += spotlightLight(spotlight, eyePos_v, normal_v);

	outColor += directionLight(sun, eyePos_v, normal_v);
	outColor += 4.0 *pointLight(fireLight, eyePos_v, normal_v);
	
	if (material.useSkybox){
		vec2 UV = vec2(texCoord_v.x * 1, texCoord_v.y * 1);
		outColor = texture(texSampler, UV);
	}
	else if(material.useTexture){
		vec2 UV = vec2(texCoord_v.x * 1, texCoord_v.y * 1);
		outColor *= texture(texSampler, UV).rgb;
	}

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