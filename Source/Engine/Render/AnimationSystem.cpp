
#include <Render/AnimationSystem.h>
#include <Core/World.h>
#include <Render/MeshComponent.h>
#include <IO/Profiler.h>

void AnimationSystem::Initialize() {
    
}

void ProcessNodeHierarchy(int frame, Mesh* mesh, Node* node, AnimationInstance* instance, matrix4 parent);

void AnimationSystem::Update(float delta) {
    PROFILE_START_AREA("AnimationSystem");
    
    // Get all render components from scene
    World* world = World::GetInstance();
    std::vector<MeshComponent*> components = world->FindComponents<MeshComponent>();
    
    auto it = components.begin();
    for(; it != components.end(); it++) {
        if((*it)->mesh == 0) continue;
        
        if((*it)->mesh->bones.size()) {
            // Update animation instance
            AnimationInstance* instance = (*it)->animation;
            if(instance == 0) {
                instance = new AnimationInstance();
                instance->currentTime = 0;
                instance->looping = false;
                instance->animation = 0;
                
                // Initialize bones
                auto bi = (*it)->mesh->bones.begin();
                for(; bi != (*it)->mesh->bones.end(); bi++) {
                    instance->bones[bi->first]  = matrix4(1.0f);
                }
                
                (*it)->animation = instance;
            }
            
            // Update timestamp
            if(instance->animation) {
                instance->currentTime += delta;
                //instance->currentTime = 0;
                float totalTime = (float)instance->animation->duration / (float)instance->animation->framesPerSecond;
                if(instance->currentTime >= totalTime) {
                    if(instance->looping) {
                        instance->currentTime = fmod(instance->currentTime, totalTime);
                    }
                    else {
                        instance->animation = 0;
                    }
                }
            }
            
            if(instance->animation) {
                // Find current animation frame
                int currentFrame = (float)instance->currentTime * (float)instance->animation->framesPerSecond;
                
                matrix4 identity = matrix4(1.0f);
                ProcessNodeHierarchy(currentFrame, (*it)->mesh, (*it)->mesh->nodes, instance, identity);
                
                // Update bone matrices
                auto bi = (*it)->mesh->bones.begin();
                for(; bi != (*it)->mesh->bones.end(); bi++) {
                    // Assemble
                    matrix4 finalTransform = (*it)->mesh->globalInverseMatrix * bi->second->finalTransform * bi->second->offsetMatrix;
                    instance->bones[bi->first] = finalTransform;
                }
            }
        }
    }
    PROFILE_END_AREA("AnimationSystem");
}

void ProcessNodeHierarchy(int frame, Mesh* mesh, Node* node, AnimationInstance* instance, matrix4 parent) {
    // Start with the local transform
    matrix4 localTransform = node->transform;
    
    // Find node animation
    auto ni = instance->animation->transforms.find(node->name);
    if(ni != instance->animation->transforms.end()) {
        // Find translation
        vector3 translate;
        if(ni->second->translations.size() > 1) {
            int index = -1;
        
            for(size_t ti = 0; ti < ni->second->translations.size() - 1; ti++) {
                if(frame < ni->second->translations[ti + 1]->time) {
                    index = ti;
                    break;
                }
            }
        
            if(index >= 0) {
                // Compute delta
                float delta = ni->second->translations[index + 1]->time - ni->second->translations[index]->time;
                float factor = (instance->currentTime * instance->animation->framesPerSecond - ni->second->translations[index]->time) / delta;
                assert(factor >= 0.0f && factor <= 1.0f);

                vector3 startT = ni->second->translations[index]->translation;
                vector3 endT = ni->second->translations[index + 1]->translation;
                vector3 differenceT = endT - startT;
                translate = startT + differenceT * factor;
            }
            else {
                // last frame
                index = ni->second->translations.size() - 1;
                translate = ni->second->translations[index]->translation;
            }
        }
        else {
            translate = ni->second->translations[0]->translation;
        }
        
        // Scaling
        vector3 scaling;
        if(ni->second->scaling.size() > 1) {
            int index = -1;
            for(size_t ti = 0; ti < ni->second->scaling.size() - 1; ti++) {
                if(frame < ni->second->scaling[ti + 1]->time) {
                    index = ti;
                    break;
                }
            }
        
            if(index >= 0) {
                // Compute delta
                float delta = ni->second->scaling[index + 1]->time - ni->second->scaling[index]->time;
                float factor = (instance->currentTime * instance->animation->framesPerSecond - ni->second->scaling[index]->time) / delta;
                assert(factor >= 0.0f && factor <= 1.0f);

                vector3 startS = ni->second->scaling[index]->scaling;
                vector3 endS = ni->second->scaling[index + 1]->scaling;
                vector3 differenceS = endS - startS;
                scaling = startS + differenceS * factor;
            }
            else {
                // last frame
                index = ni->second->scaling.size() - 1;
                scaling = ni->second->scaling[index]->scaling;
            }
        }
        else {
            scaling = ni->second->scaling[0]->scaling;
        }
        
        // Rotation
        quaternion rotation;
        if(ni->second->rotations.size() > 1) {
            int index = -1;
        
            for(size_t ti = 0; ti < ni->second->rotations.size() - 1; ti++) {
                if(frame < ni->second->rotations[ti + 1]->time) {
                    index = ti;
                    break;
                }
            }

            if(index >= 0) {
                // Compute delta
                float delta = ni->second->rotations[index + 1]->time - ni->second->rotations[index]->time;
                float factor = (instance->currentTime * instance->animation->framesPerSecond - ni->second->rotations[index]->time) / delta;
                assert(factor >= 0.0f && factor <= 1.0f);
                
                quaternion startR = ni->second->rotations[index]->rotation;
                quaternion endR = ni->second->rotations[index + 1]->rotation;
                rotation = glm::slerp(startR, endR, factor);
                rotation = glm::normalize(rotation);
            }
            else {
                // last frame
                index = ni->second->scaling.size() - 1;
                rotation = ni->second->rotations[index]->rotation;
            }
        }
        else {
            rotation = ni->second->rotations[0]->rotation;
        }
            
        matrix4 transformM = glm::translate(matrix4(1.0f), translate);
        matrix4 scalingM = glm::scale(matrix4(1.0f), scaling);
        matrix4 rotationM = glm::toMat4(rotation);
        
        localTransform = transformM  * rotationM * scalingM;
    }
    
    // Do this in reverse order because we're working up the tree
    matrix4 finalTransform = parent * localTransform;
    
    auto bi = instance->bones.find(node->name);
    if(bi != instance->bones.end()) {
        mesh->bones[node->name]->finalTransform = finalTransform;
    }
    
    // Process children
    for(int i = 0; i < node->children.size(); i++) {
        ProcessNodeHierarchy(frame, mesh, node->children[i], instance, finalTransform);
    }
}

void AnimationSystem::StartAnimation(MeshComponent* mc, std::string animation, bool loop, float weight) {
    // Find the animation
    auto ai = mc->mesh->animations.find(animation);
    GIGA_ASSERT(ai != mc->mesh->animations.end(), "Animation not found.");
    
    if(mc->animation == 0) {
        mc->animation = new AnimationInstance();
    }
    
    AnimationInstance* instance = mc->animation;
    instance->looping = loop;
    instance->animation = ai->second;
    instance->currentTime = 0;
}

void AnimationSystem::StopAnimation(MeshComponent* mc, std::string animation) {
    mc->animation->animation = 0;
    
    auto bi = mc->animation->bones.begin();
    for(; bi != mc->animation->bones.end(); bi++) {
        bi->second = matrix4(1.0f);
    }
}
