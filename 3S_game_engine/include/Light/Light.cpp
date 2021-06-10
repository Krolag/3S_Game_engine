#include "Light.h"

void PointLight::render(Shader _shader, int _idx)
{
	std::string name = "pointLights[" + std::to_string(_idx) + "]";

	_shader.setUniform(name + ".position", this->position);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);

	_shader.setUniformFloat(name + ".k0", this->k0);
	_shader.setUniformFloat(name + ".k1", this->k1);
	_shader.setUniformFloat(name + ".k2", this->k2);
}

DirLight::DirLight(glm::vec3 _direction, glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular, BoundingRegion _br)
	: direction(_direction), ambient(_ambient), diffuse(_diffuse), specular(_specular), shadowFBO(1080, 1080, GL_DEPTH_BUFFER_BIT), br(_br)
{
	/* Setup FBO */
	shadowFBO.generate();
	shadowFBO.bind();
	shadowFBO.disableColorBuffer();
	shadowFBO.allocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT, true, GL_NEAREST, GL_NEAREST, true, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	updateMatrices();
}

void DirLight::updateMatrices()
{
	glm::mat4 proj = glm::ortho(br.min.x, br.max.x, br.min.y, br.max.y, br.min.z, br.max.z);
	glm::vec3 pos = -2.0f * direction;
	glm::mat4 lightView = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = proj * lightView;
}

void DirLight::render(Shader _shader, unsigned int _textureIdx)
{
	std::string name = "dirLight";

	_shader.setUniform(name + ".direction", this->direction);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);

	/* Set depth texture */
	glActiveTexture(GL_TEXTURE0 + _textureIdx);
	shadowFBO.textures[0].bind();
	_shader.setUniformInt("dirLight.depthBuffer", _textureIdx);

	/* Set light space matrix */
	_shader.setUniform("dirLight.lightSpaceMatrix", lightSpaceMatrix);
}

void SpotLight::render(Shader _shader, int _idx)
{
	std::string name = "spotLights[" + std::to_string(_idx) + "]";

	_shader.setUniform(name + ".position", this->position);
	_shader.setUniform(name + ".direction", this->direction);
	_shader.setUniformFloat(name + ".cutOff", this->cutOff);
	_shader.setUniformFloat(name + ".outerCutOff", this->cutOff);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);

	_shader.setUniformFloat(name + ".k0", this->k0);
	_shader.setUniformFloat(name + ".k1", this->k1);
	_shader.setUniformFloat(name + ".k2", this->k2);
}
