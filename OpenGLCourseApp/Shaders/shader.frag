#version 330

in vec3 fPos;
in vec2 fTexCoord;
in vec3 fNormal;

out vec4 colour;

struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	vec3 direction;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform vec3 cameraPos;

void main()
{
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(fNormal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, fNormal);
	float spec = pow(max(dot(cameraPos, reflectDir), 0.0), 32);

	vec3 ambient = directionalLight.colour * directionalLight.ambientIntensity * texture(theTexture, fTexCoord).rgb;
	vec3 diffuse = directionalLight.diffuseIntensity * diff * texture(theTexture, fTexCoord).rgb;
    vec3 specular = directionalLight.specularIntensity * spec * texture(theTexture, fTexCoord).rgb;

	colour = vec4(ambient + diffuse + specular, 1.0);
	
	// colour = vec4(0.0, 0.0, 1.0, 1.0);
}