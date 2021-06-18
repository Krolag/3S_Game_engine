#pragma once
#include <sstream>
#include <glad/glad.h>
#include "Shader/Shader.h"
#include <glm/gtx/string_cast.hpp>
#include "Model3D.h"

struct VertexStruct
{
	glm::vec3 position;	// Vertex position 
	glm::vec3 normal;	// Vertex normal
	glm::vec2 uvs;		// Vertex uv's
};

struct VertexBoneData
{
	unsigned int IDs[4];	// An array of 4 bone IDs that influence a single vertex
	float weights[4];		// An array of the weight influence per bone

	VertexBoneData()
	{
		/* 0's out the arrays */
		Reset();
	}

	void Reset()
	{
		memset(IDs, 0, 4 * sizeof(IDs[0]));
		memset(weights, 0, 4 * sizeof(weights[0]));
	}

	void AddBoneData(unsigned int _boneID, float _weight)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			/* Check to see if there are any empty weight values */
			if (weights[i] == 0.0f)
			{
				/* Add ID of bone */
				IDs[i] = _boneID;

				/* Set the vertex weight influence for this bone ID */
				weights[i] = _weight;
				return;
			}
		}

		/* Should never get here - more bones than we have space for */
		assert(0);
	}
};

struct BoneInfoRework
{
	Matrix4f FinalTransformation;	// Final transformation to apply to vertices
	Matrix4f BoneOffset;			// Initial offset from local to bone space

	BoneInfoRework()
	{
		BoneOffset.SetZero();
		FinalTransformation.SetZero();
	}
};

#define INVALID_MATERIAL 0xFFFFFFFF
struct MeshEntry 
{
	unsigned int BaseVertex;	// Total number of mesh indices. 
	unsigned int BaseIndex;		// The base vertex of this mesh in the vertices array for the entire model.
	unsigned int NumIndices;	// The base index of this mesh in the indices array for the entire model. 
	unsigned int MaterialIndex;
	
	MeshEntry()
	{

		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;
	}

	~MeshEntry() {}
};

class SkeletalModel
{
public:
	Shader* shader;
	GLuint m_VAO;
	GLuint vbo;
	GLuint ebo;
	GLuint boneBo;
	const aiScene* pScene;
	unsigned int m_NumBones;
	Assimp::Importer Importer;
	Matrix4f m_GlobalInverseTransform;
	Matrix4f GlobalTransformation;
	std::vector<MeshEntry> m_Entries;
	std::map<std::string, unsigned int> m_BoneMapping;
	std::vector<BoneInfoRework> m_BoneInfo;

	SkeletalModel(Shader* _shader)
	{
		m_VAO = 0;
		pScene = NULL;
		m_NumBones = 0;
		shader = _shader;
	}

	~SkeletalModel()
	{
		clear();
	}

	// TODO: @Dawid what a cool looking clearing method
	void clear()
	{
		if (m_VAO != 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}

	void loadMesh(const std::string& _filename)
	{
		/* Release the previously loaded mesh (if it exist) */
		clear();

		/* Create the VAO */
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		/* Generate the buffers for the vertices attributes */
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenBuffers(1, &boneBo);

		/* Load assimp scene */
		pScene = Importer.ReadFile(_filename.c_str(), 
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_LimitBoneWeights
		);

		if (pScene)
		{
			m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
			m_GlobalInverseTransform.Inverse();

			InitFromScene(pScene, _filename);
		}
		else
			printf("Error parsing '%s': '%s'\n", _filename.c_str(), Importer.GetErrorString());

		glBindVertexArray(0);
	}

	void InitFromScene(const aiScene* _pScene, const std::string& _filename)
	{
		m_Entries.resize(_pScene->mNumMeshes);

		/* Prepare vectors for vertex attributes and indices */
		std::vector<VertexStruct> vertices;
		std::vector<VertexBoneData> bones;
		std::vector<unsigned int> indices;

		unsigned int numVertices = 0;
		unsigned int numIndices = 0;

		for (unsigned int i = 0; i < m_Entries.size(); i++)
		{
			m_Entries[i].MaterialIndex = _pScene->mMeshes[i]->mMaterialIndex;

			/* Total mesh indices */
			m_Entries[i].NumIndices = _pScene->mMeshes[i]->mNumFaces * 3;

			/* Set the base vertex of this mesh (initial vertex for this mesh within the vertices array) to the current total vertices */
			m_Entries[i].BaseVertex = numVertices;

			/* Set the base index of this mesh (initial indeex for this mesh within the indices array) to the current total indices */
			m_Entries[i].BaseIndex = numIndices;

			/* Increment total vertices and indices */
			numVertices += _pScene->mMeshes[i]->mNumVertices;
			numIndices += m_Entries[i].NumIndices;
		}

		/* Reserve space in the vetors for the vertex attributes and indices */
		vertices.reserve(numVertices);
		bones.resize(numVertices);
		indices.reserve(numIndices);

		/* Initialize the meshes in the scene one by one */
		for (unsigned int i = 0; i < m_Entries.size(); i++)
		{
			const aiMesh* paiMesh = _pScene->mMeshes[i];
			InitMesh(i, paiMesh, vertices, indices, bones);
		}

		/* Generate and populate the buffers with vertex attributes and the indices */
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexStruct), &vertices[0], GL_STATIC_DRAW);

		/* Vertex position */
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)0);

		/* Vertex normals */
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, normal));

		/* Vertex texture coords */
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, uvs));

		/* Bind the bone data buffer object */
		glBindBuffer(GL_ARRAY_BUFFER, boneBo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		vertices.clear();
		indices.clear();
		bones.clear();
	}

	void InitMesh(unsigned int _index, const aiMesh* _paiMesh, std::vector<VertexStruct>& _vertices, std::vector<GLuint>& _indices,
		std::vector<VertexBoneData>& _bones)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		/* Populate the vertex attribute vectors */
		for (unsigned int i = 0; i < _paiMesh->mNumVertices; i++)
		{
			const aiVector3D* pPos = &(_paiMesh->mVertices[i]);
			const aiVector3D* pNormal = &(_paiMesh->mNormals[i]);
			const aiVector3D* pTexCoord = _paiMesh->HasTextureCoords(0) ? &(_paiMesh->mTextureCoords[0][i]) : &Zero3D;

			glm::vec3 glmTempPos = glm::vec3(pPos->x, pPos->y, pPos->z);
			glm::vec3 glmTempNormal = glm::vec3(pNormal->x, pNormal->y, pNormal->z);
			glm::vec2 glmTempUV = glm::vec2(pTexCoord->x, pTexCoord->y);

			VertexStruct v;
			v.position = glmTempPos;
			v.normal = glmTempNormal;
			v.uvs = glmTempUV;

			_vertices.push_back(v);
		}

		/* Load the mesh's bone */
		if (_paiMesh->HasBones())
			LoadBones(_index, _paiMesh, _bones);

		/* Populate the indices array */
		for (unsigned int i = 0; i < _paiMesh->mNumFaces; i++)
		{
			const aiFace& Face = _paiMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			_indices.push_back(Face.mIndices[0]);
			_indices.push_back(Face.mIndices[1]);
			_indices.push_back(Face.mIndices[2]);
		}
	}

	void LoadBones(unsigned int _meshIndex, const aiMesh* _pMesh, std::vector<VertexBoneData>& _bones)
	{
		/* Loop through all bones int the assimp mesh */
		for (unsigned int i = 0; i < _pMesh->mNumBones; i++)
		{
			unsigned int BoneIndex = 0;

			/* Obtain the bone name */
			std::string BoneName(_pMesh->mBones[i]->mName.data);

			/* If bone isn't already in the map */
			if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
			{
				/* Set the boneID to the current total number of bones */
				BoneIndex = m_NumBones;

				/* Increment total bones */
				m_NumBones++;

				/* Push new bone info int bones vector */
				BoneInfoRework bi;
				m_BoneInfo.push_back(bi);
			}
			else
			{
				/* Bone ID is already in map */
				BoneIndex = m_BoneMapping[BoneName];
			}

			m_BoneMapping[BoneName] = BoneIndex;

			/* Obtains the offset matrix which transforms the bone from mesh space into bone space */
			m_BoneInfo[BoneIndex].BoneOffset = _pMesh->mBones[i]->mOffsetMatrix;

			/* Iterate over all the affected vertices by this bone i.e. weights */
			for (unsigned int j = 0; j < _pMesh->mBones[i]->mNumWeights; j++)
			{
				/* Obtain an index to the affected vertex within the array of vertices */
				unsigned int VertexID = m_Entries[_meshIndex].BaseVertex + _pMesh->mBones[i]->mWeights[j].mVertexId;
				/* The value of how much this bone influences the vertex */
				float weight = _pMesh->mBones[i]->mWeights[j].mWeight;
				/* Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex */
				_bones[VertexID].AddBoneData(BoneIndex, weight);
			}
		}
	}

	void BoneTransform(float _timeInSeconds, std::vector<Matrix4f>& _transforms)
	{
		Matrix4f identity;
		identity.InitIdentity();

		float TicksPerSecond = pScene->mAnimations[0]->mTicksPerSecond;
		float TimeInTicks = _timeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, pScene->mAnimations[0]->mDuration);

		ReadNodeHierarchy(AnimationTime, pScene->mRootNode, identity);

		_transforms.resize(m_NumBones);

		/* Populate transforms vector with new bone transformation matrices */
		for (unsigned int i = 0; i < m_NumBones; i++)
			_transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

	void ReadNodeHierarchy(float _animationTime, const aiNode* _pNode, const Matrix4f& _parentTransform)
	{
		Matrix4f IdentityTest;
		IdentityTest.InitIdentity();

		/* Obtain the name of the current node */
		std::string NodeName(_pNode->mName.data);

		/* Use the first animation */
		const aiAnimation* pAnimation = pScene->mAnimations[0];

		/* Obtain transformation relative to node's parent */
		Matrix4f NodeTransformation(_pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = NULL;

		/* Find the animation channel of the current node */
		for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
		{
			const aiNodeAnim* pNodeAnimIndex = pAnimation->mChannels[i];

			/* If there is a match for a channel with the current node's name, then we've found the animation channel */
			if (std::string(pNodeAnimIndex->mNodeName.data) == NodeName)
				pNodeAnim = pNodeAnimIndex;
		}

		if (pNodeAnim)
		{
			/* Interpolate scaling and generate scaling transformation matrix */
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, _animationTime, pNodeAnim);
			Matrix4f ScalingM;
			ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

			/* Interpolate rotation and generate rotation transformation matrix */
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, _animationTime, pNodeAnim);
			Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

			/* Interpolate translation and generate translation transformation matrix */
			aiVector3D Translation;
			CalcInterpolatedTranslation(Translation, _animationTime, pNodeAnim);
			Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			/* Combine the above transformations */
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}

		Matrix4f GlobalTransformation = _parentTransform * NodeTransformation;

		/* Apply the final transformation to the indexed bone in the array */
		if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
		{
			unsigned int BoneIndex = m_BoneMapping[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		}

		/* Do the same for all the node's children */
		for (unsigned int i = 0; i < _pNode->mNumChildren; i++)
			ReadNodeHierarchy(_animationTime, _pNode->mChildren[i], GlobalTransformation);
	}

	unsigned int FindScale(float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		assert(_pNodeAnim->mNumScalingKeys > 0);

		/* Find the scaling key just before the current animation time and return the index */
		for (unsigned int i = 0; i < _pNodeAnim->mNumScalingKeys - 1; i++)
		{
			if (_animationTime < (float)_pNodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}
		assert(0);

		return 0;
	}

	void CalcInterpolatedScaling(aiVector3D& _out, float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		/* We need at least two values to interpolate */
		if (_pNodeAnim->mNumScalingKeys == 1)
		{
			_out = _pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		unsigned int ScalingIndex = FindScale(_animationTime, _pNodeAnim);
		unsigned int NextScalingIndex = (ScalingIndex + 1);

		assert(NextScalingIndex < _pNodeAnim->mNumScalingKeys);

		float DeltaTime = _pNodeAnim->mScalingKeys[NextScalingIndex].mTime - _pNodeAnim->mScalingKeys[ScalingIndex].mTime;
		float Factor = (_animationTime - (float)_pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

		const aiVector3D& Start = _pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = _pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		
		aiVector3D Delta = End - Start;
		_out = Start + Factor * Delta;
	}

	void CalcInterpolatedRotation(aiQuaternion& _out, float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		/* We need at least two values to interpolate */
		if (_pNodeAnim->mNumRotationKeys == 1)
		{
			_out = _pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		/* Obtain the current rotation keyframe */
		unsigned int RotationIndex = FindRotation(_animationTime, _pNodeAnim);

		/* Calculate the next rotation keyframe and check bounds */
		unsigned int NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < _pNodeAnim->mNumRotationKeys);

		/* Calculate delta time, i.e. time between the two keyframes */
		float DeltaTime = _pNodeAnim->mRotationKeys[NextRotationIndex].mTime - _pNodeAnim->mRotationKeys[RotationIndex].mTime;

		/* Calculate the lepased time within the delta time */
		float Factor = (_animationTime - (float)_pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

		/* Obtain the quaternions values for the current and next keyframe */
		const aiQuaternion& StartRotationQ = _pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = _pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

		/* Interpolate between them using the Factor */
		aiQuaternion::Interpolate(_out, StartRotationQ, EndRotationQ, Factor);

		/* Normalize and set the refernece */
		_out = _out.Normalize();
	}

	unsigned int FindRotation(float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		/* Check if there are rotation keyframes */
		assert(_pNodeAnim->mNumRotationKeys > 0);

		/* Find the rotation key just before the current animation time and return the index */
		for (unsigned int i = 0; i < _pNodeAnim->mNumRotationKeys - 1; i++)
		{
			if (_animationTime < (float)_pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		assert(0);

		return 0;
	}

	void CalcInterpolatedTranslation(aiVector3D& _out, float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		/* We need at least two values to interpolate */
		if (_pNodeAnim->mNumPositionKeys == 1)
		{
			_out = _pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		unsigned int PositionIndex = FindTranslation(_animationTime, _pNodeAnim);
		unsigned int NextPositionIndex = (PositionIndex + 1);

		assert(NextPositionIndex < _pNodeAnim->mNumPositionKeys);

		float DeltaTime = _pNodeAnim->mPositionKeys[NextPositionIndex].mTime - _pNodeAnim->mPositionKeys[PositionIndex].mTime;
		float Factor = (_animationTime - (float)_pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

		const aiVector3D& Start = _pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = _pNodeAnim->mPositionKeys[NextPositionIndex].mValue;

		aiVector3D Delta = End - Start;
		_out = Start + Factor * Delta;
	}

	unsigned int FindTranslation(float _animationTime, const aiNodeAnim* _pNodeAnim)
	{
		assert(_pNodeAnim->mNumPositionKeys > 0);

		/* Find the translation key just before the current animation time and return the index */
		for (unsigned int i = 0; i < _pNodeAnim->mNumPositionKeys - 1; i++)
		{
			if (_animationTime < (float)_pNodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		assert(0);

		return 0;
	}

	void render(Shader& shader)
	{
		glBindVertexArray(m_VAO);

		/* Render all the model's meshes */
		for (unsigned int i = 0; i < m_Entries.size(); i++)
		{
			const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

			glDrawElementsBaseVertex(
				GL_TRIANGLES, 
				m_Entries[i].NumIndices, 
				GL_UNSIGNED_INT, 
				(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex), 
				m_Entries[i].BaseVertex
			);
		}

		glBindVertexArray(0);
	}
};

class AnimationSystem
{
public:
	static const unsigned int ui_BoneArraySize = 70;
	// TODO: @Dawid - will it be useful?
	GLint m_boneLocation[ui_BoneArraySize]; //!< Bone uniform locations 
	SkeletalModel* m_AnimatedModel;
	Shader* shader;
	int handle;
	long long startTime;

	glm::mat4 model;

	AnimationSystem(std::string _modelPath)
	{
		Shader shaderTMP("./assets/shaders/diffuse.vert", "./assets/shaders/diffuse.frag");
		shader = &shaderTMP;

		initializeBoneUniforms();

		m_AnimatedModel = new SkeletalModel(shader);
		m_AnimatedModel->loadMesh(_modelPath);
	}

	// TODO: Check when to set start time
	void setStartTime(long long _startTime)
	{
		startTime = _startTime;
	}

	void update(float _deltaTime)
	{
		/* Vector of bone transformation vertics */
		std::vector<Matrix4f> Transforms;

		// TODO: @Dawid set dTime
		/* Obtains newly transformed matrices from the bone hierarchy at the given time */
		m_AnimatedModel->BoneTransform(_deltaTime, Transforms);
		
		std::vector<glm::mat4> NewTransforms = CalculateTransformsToGLM(Transforms);
		std::cout << glm::to_string(NewTransforms[0]) << std::endl;
		
		/* Passes each new bone transformation into the shader */
		for (unsigned int i = 0; i < NewTransforms.size(); i++)
		{
			shader->use();
			shader->setUniform("gBones[" + std::to_string(i) + "]", NewTransforms[i]);
		}
	}

	void render(glm::mat4 view, glm::mat4 proj)
	{
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.2f));
		
		glm::mat4 ModelViewMatrix = view * model;
		glm::mat4 MVP = proj * ModelViewMatrix;
		glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(model)));

		shader->use();
		shader->setUniform("ModelViewMatrix", ModelViewMatrix);
		shader->setUniform("MVP", MVP);
		shader->setUniform("M", model);
		shader->setUniform("NormalMatrix", normMat);
		shader->setUniform("V", view);
		shader->setUniform("P", proj);
		shader->setUniform("Ka", glm::vec3(0.225f, 0.125f, 0.0f));
		shader->setUniform("Kd", glm::vec3(1.0f, 0.6f, 0.0f));
		shader->setUniform("Ks", glm::vec3(1.0f, 1.0f, 1.0f));
		shader->setUniformFloat("specularShininess", 1.0f);

		m_AnimatedModel->render(*shader);
	}

	std::vector<glm::mat4> CalculateTransformsToGLM(std::vector<Matrix4f> _matrix)
	{
		std::vector<glm::mat4> TMP;

		for (unsigned int m = 0; m < _matrix.size(); m++)
		{
			glm::mat4 matTMP;

			for (unsigned int i = 0; i < 4; i++)
			{
				for (unsigned int j = 0; j < 4; j++)
					matTMP[i][j] = _matrix[m].m[i][j];
			}

			TMP.push_back(matTMP);
		}

		return TMP;
	}

private:
	void initializeBoneUniforms()
	{
		for (unsigned int i = 0; i < ui_BoneArraySize; i++)
		{
			char name[128];
			memset(name, 0, sizeof(name));
			_snprintf_s(name, sizeof(name), "gBones[%d]", i);
			m_boneLocation[i] = glGetUniformLocation(handle, name);
		}
	}
};

