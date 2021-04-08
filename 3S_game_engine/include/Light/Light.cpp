#include "Light.h"

void PointLight::render(Shader _shader, int _idx)
{
	std::string name = "pointLights[" + std::to_string(_idx) + "]";

	_shader.setUniform(name + ".position", this->position);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);

	_shader.setUniform(name + ".k0", this->k0);
	_shader.setUniform(name + ".k1", this->k1);
	_shader.setUniform(name + ".k2", this->k2);
}

void DirLight::render(Shader _shader)
{
	std::string name = "dirLight";

	_shader.setUniform(name + ".direction", this->direction);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);
}

void SpotLight::render(Shader _shader, int _idx)
{
	std::string name = "spotLights[" + std::to_string(_idx) + "]";

	_shader.setUniform(name + ".position", this->position);
	_shader.setUniform(name + ".direction", this->direction);
	_shader.setUniform(name + ".cutOff", this->cutOff);
	_shader.setUniform(name + ".outerCutOff", this->cutOff);
	_shader.setUniform(name + ".ambient", this->ambient);
	_shader.setUniform(name + ".diffuse", this->diffuse);
	_shader.setUniform(name + ".specular", this->specular);

	_shader.setUniform(name + ".k0", this->k0);
	_shader.setUniform(name + ".k1", this->k1);
	_shader.setUniform(name + ".k2", this->k2);
}
