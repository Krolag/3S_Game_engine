#pragma once
#ifndef FRUSTUMCULLING_H
#define FRUSTUMCULLING_H

#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class FrustumCulling
{
public:
	/* Calculate the view frustum from the view-projection matrix */
	static void createViewFrustumFromMatrix(Camera* _camera)
	{
		float projection[16];
		float modelview[16];
		float clip[16];
		float t;

		/* Get current projection matrix from OpenGL */
		const float* modelviewSource = (const float*)glm::value_ptr(_camera->GetViewMatrix());
		for (int i = 0; i < 16; i++)
		{
			modelview[i] = modelviewSource[i];
		}
		/* Get current model-view matrix */
		const float* projectionSource = (const float*)glm::value_ptr(_camera->getProjection());
		for (int i = 0; i < 16; i++)
		{
			projection[i] = projectionSource[i];
		}

		/* Combine the two matrices (simply multiply projection by modelview) */
		clip[0] = (modelview[0] * projection[0]) + (modelview[1] * projection[4]) + (modelview[2] * projection[8]) + (modelview[3] * projection[12]);
		clip[1] = (modelview[0] * projection[1]) + (modelview[1] * projection[5]) + (modelview[2] * projection[9]) + (modelview[3] * projection[13]);
		clip[2] = (modelview[0] * projection[2]) + (modelview[1] * projection[6]) + (modelview[2] * projection[10]) + (modelview[3] * projection[14]);
		clip[3] = (modelview[0] * projection[3]) + (modelview[1] * projection[7]) + (modelview[2] * projection[11]) + (modelview[3] * projection[15]);
		clip[4] = (modelview[4] * projection[0]) + (modelview[5] * projection[4]) + (modelview[6] * projection[8]) + (modelview[7] * projection[12]);
		clip[5] = (modelview[4] * projection[1]) + (modelview[5] * projection[5]) + (modelview[6] * projection[9]) + (modelview[7] * projection[13]);
		clip[6] = (modelview[4] * projection[2]) + (modelview[5] * projection[6]) + (modelview[6] * projection[10]) + (modelview[7] * projection[14]);
		clip[7] = (modelview[4] * projection[3]) + (modelview[5] * projection[7]) + (modelview[6] * projection[11]) + (modelview[7] * projection[15]);
		clip[8] = (modelview[8] * projection[0]) + (modelview[9] * projection[4]) + (modelview[10] * projection[8]) + (modelview[11] * projection[12]);
		clip[9] = (modelview[8] * projection[1]) + (modelview[9] * projection[5]) + (modelview[10] * projection[9]) + (modelview[11] * projection[13]);
		clip[10] = (modelview[8] * projection[2]) + (modelview[9] * projection[6]) + (modelview[10] * projection[10]) + (modelview[11] * projection[14]);
		clip[11] = (modelview[8] * projection[3]) + (modelview[9] * projection[7]) + (modelview[10] * projection[11]) + (modelview[11] * projection[15]);
		clip[12] = (modelview[12] * projection[0]) + (modelview[13] * projection[4]) + (modelview[14] * projection[8]) + (modelview[15] * projection[12]);
		clip[13] = (modelview[12] * projection[1]) + (modelview[13] * projection[5]) + (modelview[14] * projection[9]) + (modelview[15] * projection[13]);
		clip[14] = (modelview[12] * projection[2]) + (modelview[13] * projection[6]) + (modelview[14] * projection[10]) + (modelview[15] * projection[14]);
		clip[15] = (modelview[12] * projection[3]) + (modelview[13] * projection[7]) + (modelview[14] * projection[11]) + (modelview[15] * projection[15]);

		/* Extract the numbers for the RIGHT plane */
		// Calculate 
		_camera->frustum[0][0] = clip[3] - clip[0];
		_camera->frustum[0][1] = clip[7] - clip[4];
		_camera->frustum[0][2] = clip[11] - clip[8];
		_camera->frustum[0][3] = clip[15] - clip[12];
		// Normalize
		t = sqrt((_camera->frustum[0][0] * _camera->frustum[0][0]) + (_camera->frustum[0][1] * _camera->frustum[0][1]) + (_camera->frustum[0][2] * _camera->frustum[0][2]));
		_camera->frustum[0][0] /= t;
		_camera->frustum[0][1] /= t;
		_camera->frustum[0][2] /= t;
		_camera->frustum[0][3] /= t;
		/* Extract the numbers for the LEFT plane */
		// Calculate 
		_camera->frustum[1][0] = clip[3] + clip[0];
		_camera->frustum[1][1] = clip[7] + clip[4];
		_camera->frustum[1][2] = clip[11] + clip[8];
		_camera->frustum[1][3] = clip[15] + clip[12];
		// Normalize
		t = sqrt((_camera->frustum[1][0] * _camera->frustum[1][0]) + (_camera->frustum[1][1] * _camera->frustum[1][1]) + (_camera->frustum[1][2] * _camera->frustum[1][2]));
		_camera->frustum[1][0] /= t;
		_camera->frustum[1][1] /= t;
		_camera->frustum[1][2] /= t;
		_camera->frustum[1][3] /= t;
		/* Extract the numbers for the BOTTOM plane */
		// Calculate 
		_camera->frustum[2][0] = clip[3] + clip[1];
		_camera->frustum[2][1] = clip[7] + clip[5];
		_camera->frustum[2][2] = clip[11] + clip[9];
		_camera->frustum[2][3] = clip[15] + clip[13];
		// Normalize
		t = sqrt((_camera->frustum[2][0] * _camera->frustum[2][0]) + (_camera->frustum[2][1] * _camera->frustum[2][1]) + (_camera->frustum[2][2] * _camera->frustum[2][2]));
		_camera->frustum[2][0] /= t;
		_camera->frustum[2][1] /= t;
		_camera->frustum[2][2] /= t;
		_camera->frustum[2][3] /= t;
		/* Extract the numbers for the TOP plane */
		// Calculate 
		_camera->frustum[3][0] = clip[3] - clip[1];
		_camera->frustum[3][1] = clip[7] - clip[5];
		_camera->frustum[3][2] = clip[11] - clip[9];
		_camera->frustum[3][3] = clip[15] - clip[13];
		// Normalize
		t = sqrt((_camera->frustum[3][0] * _camera->frustum[3][0]) + (_camera->frustum[3][1] * _camera->frustum[3][1]) + (_camera->frustum[3][2] * _camera->frustum[3][2]));
		_camera->frustum[3][0] /= t;
		_camera->frustum[3][1] /= t;
		_camera->frustum[3][2] /= t;
		_camera->frustum[3][3] /= t;
		/* Extract the numbers for the FAR plane */
		// Calculate 
		_camera->frustum[4][0] = clip[3] - clip[2];
		_camera->frustum[4][1] = clip[7] - clip[6];
		_camera->frustum[4][2] = clip[11] - clip[10];
		_camera->frustum[4][3] = clip[15] - clip[14];
		// Normalize
		t = sqrt((_camera->frustum[4][0] * _camera->frustum[4][0]) + (_camera->frustum[4][1] * _camera->frustum[4][1]) + (_camera->frustum[4][2] * _camera->frustum[4][2]));
		_camera->frustum[4][0] /= t;
		_camera->frustum[4][1] /= t;
		_camera->frustum[4][2] /= t;
		_camera->frustum[4][3] /= t;
		/* Extract the numbers for the NEAR plane */
		// Calculate 
		_camera->frustum[5][0] = clip[3] + clip[2];
		_camera->frustum[5][1] = clip[7] + clip[6];
		_camera->frustum[5][2] = clip[11] + clip[10];
		_camera->frustum[5][3] = clip[15] + clip[14];
		// Normalize
		t = sqrt((_camera->frustum[5][0] * _camera->frustum[5][0]) + (_camera->frustum[5][1] * _camera->frustum[5][1]) + (_camera->frustum[5][2] * _camera->frustum[5][2]));
		_camera->frustum[5][0] /= t;
		_camera->frustum[5][1] /= t;
		_camera->frustum[5][2] /= t;
		_camera->frustum[5][3] /= t;
	}

	static bool pointInFrustum(float _x, float _y, float _z, Camera* _camera)
	{
		for (unsigned int p = 0; p < 6; p++)
		{
			if ((_camera->frustum[p][0] * _x) + (_camera->frustum[p][1] * _y) + (_camera->frustum[p][2] * _z) + _camera->frustum[p][3] <= 0)
			{
				return false;
			}
		}

		return true;
	}

	static bool boxAABBInFrustum(float _x, float _y, float _z, float _size, Camera* _camera)
	{
		for (unsigned int p = 0; p < 6; p++)
		{
			if ((_camera->frustum[p][0] * (_x - _size)) + (_camera->frustum[p][1] * (_y - _size)) + (_camera->frustum[p][2] * (_z - _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x + _size)) + (_camera->frustum[p][1] * (_y - _size)) + (_camera->frustum[p][2] * (_z - _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x - _size)) + (_camera->frustum[p][1] * (_y + _size)) + (_camera->frustum[p][2] * (_z - _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x + _size)) + (_camera->frustum[p][1] * (_y + _size)) + (_camera->frustum[p][2] * (_z - _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x - _size)) + (_camera->frustum[p][1] * (_y - _size)) + (_camera->frustum[p][2] * (_z + _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x + _size)) + (_camera->frustum[p][1] * (_y - _size)) + (_camera->frustum[p][2] * (_z + _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x - _size)) + (_camera->frustum[p][1] * (_y + _size)) + (_camera->frustum[p][2] * (_z + _size)) + _camera->frustum[p][3] > 0)	continue;
			if ((_camera->frustum[p][0] * (_x + _size)) + (_camera->frustum[p][1] * (_y + _size)) + (_camera->frustum[p][2] * (_z + _size)) + _camera->frustum[p][3] > 0)	continue;
			return false;
		}

		return true;
	}
};

#endif //!FRUSTUMCULLING_H