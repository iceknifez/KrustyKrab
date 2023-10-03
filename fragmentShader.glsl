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

	// 使用纹理
	if(useTexture > 0) {
		materialAmbient = vec3(texture(material.diffuse, TexCoords));
		materialDiffuse = vec3(texture(material.diffuse, TexCoords));
		alpha = texture(material.diffuse, TexCoords).w;
	}
	// 不使用纹理
	else
	{
		materialAmbient = material.ambientColor;
		materialDiffuse = material.diffuseColor;
		alpha = 1.0f;
	}

	// 环境光
	vec3 ambient = light.ambient * materialAmbient;

	// 漫反射
	vec3 norm = normalize(Normal);	// 片元的法向量
	vec3 lightDir = normalize(light.position - FragPos);	// 片元指向光源的方向
	float diff = max(dot(norm, lightDir), 0.0);	// 法向量与光照角度夹角的余弦值
	vec3 diffuse = light.diffuse * diff * materialAmbient;

	// 镜面反射
	// float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);	// 观察方向
    vec3 reflectDir = reflect(-lightDir, norm);		// 反射方向
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
	// 将三种光组合在一起
	FragColor = vec4(ambient + diffuse + specular, alpha);
}