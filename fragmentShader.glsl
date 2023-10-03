#version 330 core

in vec3 FragPos;
in vec3 Normal;

in vec2 TexCoords;

out vec4 FragColor;

// uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform int useTexture;

struct Material
{
	vec3 ambientColor;
    vec3 diffuseColor;
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
	vec3 materialAmbient;
	vec3 materialDiffuse;
	float alpha;

	// ʹ������
	if(useTexture > 0) {
		materialAmbient = vec3(texture(material.diffuse, TexCoords));
		materialDiffuse = vec3(texture(material.diffuse, TexCoords));
		alpha = texture(material.diffuse, TexCoords).w;
	}
	// ��ʹ������
	else
	{
		materialAmbient = material.ambientColor;
		materialDiffuse = material.diffuseColor;
		alpha = 1.0f;
	}

	// ������
	vec3 ambient = light.ambient * materialAmbient;

	// ������
	vec3 norm = normalize(Normal);	// ƬԪ�ķ�����
	vec3 lightDir = normalize(light.position - FragPos);	// ƬԪָ���Դ�ķ���
	float diff = max(dot(norm, lightDir), 0.0);	// ����������սǶȼнǵ�����ֵ
	vec3 diffuse = light.diffuse * diff * materialAmbient;

	// ���淴��
	// float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);	// �۲췽��
    vec3 reflectDir = reflect(-lightDir, norm);		// ���䷽��
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
	// �����ֹ������һ��
	FragColor = vec4(ambient + diffuse + specular, alpha);
}