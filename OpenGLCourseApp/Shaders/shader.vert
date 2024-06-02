#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;

out vec3 fPos;
out vec2 fTexCoord;
out vec3 fNormal;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	fPos = vec3(model * vec4(pos,1.0));
	fNormal = mat3(transpose(inverse(model))) * normal;
	fTexCoord = tex;
	gl_Position = projection * view * vec4(fPos, 1.0);
}