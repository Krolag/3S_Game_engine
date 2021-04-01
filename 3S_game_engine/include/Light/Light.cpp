#include "Light.h"

void PointLight::render(Shader _shader, int _idx)
{
	std::string name = "pointLights[" + std::to_string(_idx) + "]";

	_shader.setVec3(name + ".position", this->position);
	_shader.setVec3(name + ".ambient", this->ambient);
	_shader.setVec3(name + ".diffuse", this->diffuse);
	_shader.setVec3(name + ".specular", this->specular);

	_shader.setFloat(name + ".k0", this->k0);
	_shader.setFloat(name + ".k1", this->k1);
	_shader.setFloat(name + ".k2", this->k2);
}

void DirLight::render(Shader _shader)
{
	std::string name = "dirLight";

	_shader.setVec3(name + ".direction", this->direction);
	_shader.setVec3(name + ".ambient", this->ambient);
	_shader.setVec3(name + ".diffuse", this->diffuse);
	_shader.setVec3(name + ".specular", this->specular);
}

void SpotLight::render(Shader _shader, int _idx)
{
	std::string name = "spotLights[" + std::to_string(_idx) + "]"; ";

	_shader.setVec3(name + ".position", this->position);
	_shader.setVec3(name + ".direction", this->direction);
	_shader.setFloat(name + ".cutOff", this->cutOff);
	_shader.setFloat(name + ".outerCutOff", this->cutOff);
	_shader.setVec3(name + ".ambient", this->ambient);
	_shader.setVec3(name + ".diffuse", this->diffuse);
	_shader.setVec3(name + ".specular", this->specular);

	_shader.setFloat(name + ".k0", this->k0);
	_shader.setFloat(name + ".k1", this->k1);
	_shader.setFloat(name + ".k2", this->k2);
}
