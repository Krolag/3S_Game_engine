#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"
#include "Bone.h"

class Animator {
private:
    std::vector<glm::mat4> m_Transforms;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
public:
    Animator() = default;
    Animator(Animation* current);
    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetPoseTransforms();
};


#endif // !ANIMATOR_H
