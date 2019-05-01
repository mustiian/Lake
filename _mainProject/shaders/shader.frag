#version 140

struct Material {   
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;    
	float shininess;

	bool useTexture; 
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
uniform mat4 PVMmatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform sampler2D texSampler;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform Material material; 

smooth in vec2 texCoord_v; 
smooth in vec3 eyePos_v; 
smooth in vec3 normal_v; 

out vec4 colorFragment;      

Light sun;
Light spotlight;

void initSun(){
	sun.ambient = vec3(0.5f);
	sun.diffuse = vec3(1.0f, 1.0f, 0.5f);
	sun.specular = vec3(1.0f);
	sun.position = (viewMatrix * vec4(1.0, 1.0, 1.0, 0.0)).xyz;
	sun.direction = (viewMatrix * vec4(0.7, -0.6, 0.9, 0.0)).xyz;
}

void initSpotlight(){
	spotlight.ambient = vec3(1.0f);
	spotlight.diffuse = vec3(0.5f, 0.5f, 0.5f);
	spotlight.specular = vec3(1.0);

	spotlight.position = (viewMatrix * vec4(cameraPosition, 1.0)).xyz;
	spotlight.direction = normalize((viewMatrix * vec4(cameraDirection, 0.0)).xyz);

	spotlight.spotCosAngle = 0.9f;
	spotlight.spotExponent = 5.0f;
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
	 float spec = max(dot(refl, toEyes),0.0f);
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

	 // diffuse
	 vec3 normal = vertexNormal;
	 vec3 lightPos = normalize(light.position);
	 float diff = max(dot(normal, lightPos), 0.0);
	 vec3 diffuse = light.diffuse * material.diffuse * diff;

	 // specular
	 vec3 viewPos = normalize(-vertexPosition);
	 vec3 refl = reflect(-lightPos, normal);
	 float spec = max(dot(refl, viewPos), 0.0);
	 vec3 specular = light.specular * material.specular * pow(spec, material.shininess);

	 vec3 result = ambient + diffuse + specular;
	 return vec4(result, 1.0);
}

vec4 pointLight(Light light, vec3 vertexPosition, vec3 vertexNormal) {

	float distance = abs(light.position.x - vertexPosition.x) +
					 abs(light.position.y - vertexPosition.y) + 
					 abs(light.position.z - vertexPosition.z);
	float attenuation = 1.0f / (pow(distance, 0.5f));
	return attenuation * directionLight(light, vertexPosition, vertexNormal);
}

vec4 sunLight(Light light, vec3 vertexPosition, vec3 vertexNormal){

	vec3 ambient = material.ambient*light.ambient;
	float X = dot(vertexNormal, light.direction);

	vec3 diffuse = max(X, 0.0f)*material.diffuse*light.diffuse;

	vec3 sum = ambient + diffuse;
	
	return vec4(sum, 1.0f);
}

void main() {
	initSun();
	initSpotlight();

	vec3 globalAmbientLight = vec3(0.6f);
	vec4 outColor = vec4(material.ambient * globalAmbientLight, 0.0);

	if (useFlashlight)
		outColor += spotlightLight(spotlight, eyePos_v, normal_v);

	outColor += directionLight(sun, eyePos_v, normal_v);
	outColor += sunLight(sun, eyePos_v, normal_v);
	
	if(material.useTexture){
		vec2 UV = vec2(texCoord_v.x * 30, texCoord_v.y * 30);
		outColor *= texture(texSampler, UV).rgb;
	}

	colorFragment = outColor;
}