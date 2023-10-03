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
	// ��������궥��ת��Ϊ��Ļ���궥��
	gl_Position = projection * modelview * vec4(position, 1.0f);
	// ����ƬԪ������ռ��еĶ������� �Ա�֮�������������صļ���
	FragPos = vec3(modelview * vec4(position, 1.0f));
	// ���㷨����������ռ����� �Ա�֮����о��淴����صļ���
	Normal = mat3(transpose(inverse(modelview))) * normal;
	TexCoords = texCoords;
}