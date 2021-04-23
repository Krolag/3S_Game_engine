#pragma once
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Loader/Loader.h"
#include "Material/Material.h"

namespace GameLogic
{
	class MeshRenderer : public Component
	{
	public:
		Loader::Model* model = nullptr;
		Shader* shader;

		/* Constructors */
		MeshRenderer(ComponentType _type, Proctor* _proctor);

		/* Model methods */
		void setModel(Loader::Model* _model);
		void setShader(Shader* _shader);

		void update();
		void cleanup();

	private:
		Proctor* parent = nullptr;
	};
}

#endif // !MESHRENDERER_H