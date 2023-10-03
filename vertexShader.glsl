#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
	// 将齐次坐标顶点转化为屏幕坐标顶点
	gl_Position = projection * modelview * vec4(position, 1.0f);
	// 计算片元在世界空间中的顶点坐标 以便之后进行漫反射相关的计算
	FragPos = vec3(modelview * vec4(position, 1.0f));
	// 计算法向量的世界空间坐标 以便之后进行镜面反射相关的计算
	Normal = mat3(transpose(inverse(modelview))) * normal;
	TexCoords = texCoords;
}