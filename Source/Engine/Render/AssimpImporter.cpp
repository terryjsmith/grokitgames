
#include <Render/AssimpImporter.h>
#include <Render/Bone.h>
#include <IO/ResourceSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/MaterialSystem.h>
#include <Render/Node.h>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct AssimpMaterial {
    std::string texture;
    Material* material;
};

static inline glm::mat4 mat4_convert(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }

Node* CreateNodeHeirarchy(const aiNode* pNode) {
    Node* n = new Node();
    n->name = pNode->mName.C_Str();
    n->transform = mat4_convert(pNode->mTransformation);
    
    for (uint32_t i = 0 ; i < pNode->mNumChildren ; i++) {
        Node* child = CreateNodeHeirarchy(pNode->mChildren[i]);
        child->parent = n;
        n->children.push_back(child);
    }
    
    return(n);
}

Node* GetNode(std::string name, Node* parent) {
    if(parent->name == name) {
        return(parent);
    }
    
    for(int i = 0; i < parent->children.size(); i++) {
        Node* n = GetNode(name, parent->children[i]);
        if(n) {
            return(n);
        }
    }
    
    return(0);
}

void AssimpImporter::LoadMesh(std::string filename, Mesh* mesh) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    MaterialSystem* materialSystem = GetSystem<MaterialSystem>();
    
    // Use Assimp to import our scene
    const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals);
    
    Array<AssimpMaterial*> materialList;
    
    // Load our textures and materials
    for(int i = 0; i < scene->mNumMaterials; i++) {
        AssimpMaterial* material = new AssimpMaterial();
        material->material = new Material;
        
        const aiMaterial* pMaterial = scene->mMaterials[i];
        
        // Diffuse
        aiColor3D diffuse (0.f,0.f,0.f);
        pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->material->diffuse = vector4(diffuse.r, diffuse.g, diffuse.b, 1.0f);
        
        aiColor3D ambient (0.f,0.f,0.f);
        pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->material->ambient = vector4(ambient.r, ambient.g, ambient.b, 1.0f);
        
        aiColor3D specular (0.f,0.f,0.f);
        pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->material->specular = vector4(specular.r, specular.g, specular.b, 1.0f);
        
        float shininess = 0.0f;
        pMaterial->Get(AI_MATKEY_SHININESS, shininess);
        material->material->shininess = shininess;
        
        // Try to get a diffuse texture
        unsigned int textureCount = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        if(textureCount > 0) {
            aiString Path;
            
            char* filename = 0;
            
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                // Is this an embedded texture or an actual path to a file?
                std::string firstChar = std::string(Path.C_Str()).substr(0, 1);
                if(firstChar == "*") {
                    // Embedded texture, skip?
                }
                else {
                    // Otherwise, it is a path, record the texture file
                    std::string fileStr = Path.C_Str();
                    if(fileStr.find_last_of("/") != std::string::npos) {
                        fileStr = fileStr.substr(fileStr.find_last_of("/") + 1);
                    }
                    if(fileStr.find_last_of("\\") != std::string::npos) {
                        fileStr = fileStr.substr(fileStr.find_last_of("\\") + 1);
                    }
                    filename = (char*)malloc(fileStr.size() + 1);
                    strcpy(filename, (char*)fileStr.c_str());
                }
            }
            
            if(filename != 0) {
                // Assign it to any sub-meshes using this material index
                material->texture = filename;
                
                free(filename);
            }
        }
        
        materialSystem->AddMaterial(material->material);
        materialList.push_back(material);
    }
    
    mesh->renderable = new Renderable();
    
    // Read node hierarchy
    mesh->nodes = CreateNodeHeirarchy(scene->mRootNode);
    
    // Create meshes and bones
    std::map<std::string, aiMatrix4x4> boneOffsets;
    for(int m = 0; m < scene->mNumMeshes; m++) {
        const struct aiMesh* paiMesh = scene->mMeshes[m];

        Renderable* ret = new Renderable();
        mesh->renderable->children.push_back(ret);
        
        // Save the name
        mesh->name = paiMesh->mName.data;
        
        // Load bones
        if(paiMesh->HasBones()) {
            for(int i = 0; i < paiMesh->mNumBones; i++) {
                char* boneName = paiMesh->mBones[i]->mName.data;
                auto bi = mesh->bones.find(boneName);
                
                if(bi == mesh->bones.end()) {
                    // Create a new bone
                    Bone* newBone = new Bone();
                    
                    // Save the name of the bone
                    newBone->name = paiMesh->mBones[i]->mName.data;
                    newBone->offsetMatrix = mat4_convert(paiMesh->mBones[i]->mOffsetMatrix);
                    mesh->bones[boneName] = newBone;
                    
                    boneOffsets[boneName] = paiMesh->mBones[i]->mOffsetMatrix;
                }
            }
        }
    }
    
    // Load mesh data
    for(int m = 0; m < scene->mNumMeshes; m++) {
        const struct aiMesh* paiMesh = scene->mMeshes[m];
        Renderable* ret = mesh->renderable->children[m];
        
        /* Initialize material data
        ret->material->diffuse = vector4(1.0, 1.0, 1.0, 1.0);
        ret->material->specular = vector4(0.0, 0.0, 0.0, 0.0);
        ret->material->ambient = vector4(1.0, 1.0, 1.0, 1.0);
        ret->material->shininess = 0.0;*/
        
        // Use the material index to set the data
        if (materialList[paiMesh->mMaterialIndex]->texture.length()) {
            ret->diffuseTexture = dynamic_cast<Texture2D*>(resourceSystem->LoadResource(materialList[paiMesh->mMaterialIndex]->texture, "Texture2D"));
        }
        ret->material = materialList[paiMesh->mMaterialIndex]->material;
        
        // Initialize our data
        Array<float> vertex_data;
        Array<unsigned int> index_data;
        
        // Calculate our vertex size
        ret->vertexBuffer = renderSystem->CreateVertexBuffer();
        VertexFormat* vf = renderSystem->CreateVertexFormat();
        vf->Initialize();

        // Figure out our vertex format
        int offset = 3;
        vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
        
        if(paiMesh->HasNormals()) {
            vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_NORMAL, 3, offset);
            offset += 3;
        }
        
        if(paiMesh->HasTextureCoords(0)) {
            vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, offset);
            offset += 2;
        }
        
        if(paiMesh->HasBones()) {
            vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_BONES, 4, offset);
            offset += 4;
            
            vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_BONEWEIGHTS, 4, offset);
            offset += 4;
        }
        
        int vertexSize = vf->GetVertexSize();
        
        // Prepare vertex buffer
        vertex_data.resize(paiMesh->mNumVertices * vertexSize);
        memset(&vertex_data[0], 0, sizeof(float) * paiMesh->mNumVertices * vertexSize);
        
        // Track min/max for AABB
        float minX, maxX, minY, maxY, minZ, maxZ;
        minX = minY = minZ = FLT_MAX;
        maxX = maxY = maxZ = -FLT_MAX;
        
        // Load our vertices into our internal format
        for(int i = 0; i < paiMesh->mNumVertices; i++) {
            unsigned int offset = i * vf->GetVertexSize();
            
            const aiVector3D* pPos = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : NULL;
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : NULL;

            vector4 position = vector4(pPos->x, pPos->y, pPos->z, 1.0);
            
            vertex_data[offset + 0] = position.x;
            vertex_data[offset + 1] = position.y;
            vertex_data[offset + 2] = position.z;
            
            minX = std::min(minX, position.x);
            maxX = std::max(maxX, position.x);
            minY = std::min(minY, position.y);
            maxY = std::max(maxY, position.y);
            minZ = std::min(minZ, position.z);
            maxZ = std::max(maxZ, position.z);
            
            offset += 3;
            
            if(paiMesh->HasNormals()) {
                vector4 normal = vector4(pNormal->x, pNormal->z, pNormal->y, 1.0);
                vertex_data[offset + 0] = normal.x;
                vertex_data[offset + 1] = normal.y;
                vertex_data[offset + 2] = normal.z;
                
                offset += 3;
            }
            
            if(paiMesh->HasTextureCoords(0)) {
                vertex_data[offset + 0] = pTexCoord->x;
                vertex_data[offset + 1] = pTexCoord->y;
                
                offset += 2;
            }
        }
        
        // We're going to write vertex data after any possible animation data has been added
        
        // Next, load our index data
        for(int i = 0 ; i < paiMesh->mNumFaces; i++) {
            const aiFace& face = paiMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            
            index_data.push_back(face.mIndices[0]);
            index_data.push_back(face.mIndices[1]);
            index_data.push_back(face.mIndices[2]);
        }
        
        ret->indexBuffer = renderSystem->CreateIndexBuffer();
        ret->indexBuffer->Create((int)index_data.size(), &index_data[0]);
        
        if(paiMesh->HasBones()) {
            for(int i = 0; i < paiMesh->mNumBones; i++) {
                // Get the bone ID
                char* boneName = paiMesh->mBones[i]->mName.data;
                auto bi = mesh->bones.find(boneName);
                int boneOffset = std::distance(mesh->bones.begin(), bi);
                
                // Save the vertex weights
                for(int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
                    unsigned int vertexID = paiMesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
                    
                    int offsetToBoneIDs = (vertexID * vertexSize) + vertexSize - 8;
                    int offsetToBoneWeights = (vertexID * vertexSize) + vertexSize - 8 + 4;
                    
                    bool foundSlot = false;
                    for(int k = 0; k < 4; k++) {
                        if(vertex_data[offsetToBoneWeights + k] == 0.0f) {
                            vertex_data[offsetToBoneIDs + k] = boneOffset;
                            vertex_data[offsetToBoneWeights + k] = weight;
                            
                            foundSlot = true;
                            break;
                        }
                    }
                    
                    if(foundSlot == false) {
                        // If we get here, more bones than slots, need to take highest one by replacing the one with the
                        // least weight
                        int index = 0;
                        float minFactor = 1.0f;
                        for(int k = 0; k < 4; k++) {
                            if(vertex_data[offsetToBoneWeights + k] < minFactor) {
                                index = k;
                                minFactor = vertex_data[offsetToBoneWeights + k];
                            }
                        }
                        
                        if(minFactor < weight) {
                            vertex_data[offsetToBoneIDs + index] = boneOffset;
                            vertex_data[offsetToBoneWeights + index] = weight;
                        }
                    }
                }
            }
            
            /* Make sure the vertex weights add up to 1.0
            int numVertices = vertex_data.size() / vf->GetVertexSize();
            for(int i  = 0; i < numVertices; i++) {
                int offsetToBoneWeights = (i * vertexSize) + vertexSize - 8 + 4;
                vector4 v = vector4(vertex_data[offsetToBoneWeights + 0], vertex_data[offsetToBoneWeights + 1],
                                    vertex_data[offsetToBoneWeights + 2], vertex_data[offsetToBoneWeights + 3]);
                float size = v.x + v.y + v.z + v.w;
                if(size < 1.0f) {
                    float factor = 1.0f / size;
                    v *= factor;
                    size = v.x + v.y + v.z + v.w;
                    
                    vertex_data[offsetToBoneWeights + 0] = v.x;
                    vertex_data[offsetToBoneWeights + 1] = v.y;
                    vertex_data[offsetToBoneWeights + 2] = v.z;
                    vertex_data[offsetToBoneWeights + 3] = v.w;
                }
            }*/
        }
        
        // Load vertex buffer
        ret->vertexBuffer->Create(vf, vertex_data.size() / vf->GetVertexSize(), &vertex_data[0], false);
        
        // Create bounding box
        ret->aabb = new AABB();
        ret->aabb->Create(vector3(minX, minY, minZ), vector3(maxX, maxY, maxZ));
    }
    
    // Save the global inverse matrix for later use
    mesh->globalInverseMatrix = glm::inverse(mat4_convert(scene->mRootNode->mTransformation));
    
    // Load animations
    if(scene->HasAnimations()) {
        for(int i = 0; i < scene->mNumAnimations; i++) {
            // Save the name
            std::string animationName = scene->mAnimations[i]->mName.data;
            
            // Create new animation
            Animation* animation = new Animation();
            animation->type = Animation::ANIMATIONTYPE_SKELETAL;
            animation->duration = scene->mAnimations[i]->mDuration;
            animation->framesPerSecond = scene->mAnimations[i]->mTicksPerSecond;
            
            mesh->animations[animationName] = animation;
            
            // Loop over Assimp's "channels" and figure out the discrete frame timestamps
            Array<double> frameTimes;
            for(int j = 0; j < scene->mAnimations[i]->mDuration; j++) {
                frameTimes.push_back(j);
            }
            
            // For each channel (bone) in the animation, get the keyframes
            for(int c = 0; c < scene->mAnimations[i]->mNumChannels; c++) {
                aiNodeAnim* nodeAnim = scene->mAnimations[i]->mChannels[c];
                std::string nodeName = nodeAnim->mNodeName.C_Str();
                
                AnimationTransforms* at = new AnimationTransforms();
                at->nodeName = nodeName;
                
                for(int s = 0; s < nodeAnim->mNumScalingKeys; s++) {
                    AnimationTransforms::Scaling* sc = new AnimationTransforms::Scaling();
                    sc->time = nodeAnim->mScalingKeys[s].mTime;
                    sc->scaling = vector3(nodeAnim->mScalingKeys[s].mValue.x,nodeAnim->mScalingKeys[s].mValue.y, nodeAnim->mScalingKeys[s].mValue.z);
                    
                    at->scaling.push_back(sc);
                }
                
                for(int s = 0; s < nodeAnim->mNumPositionKeys; s++) {
                    AnimationTransforms::Translation* tc = new AnimationTransforms::Translation();
                    tc->time = nodeAnim->mPositionKeys[s].mTime;
                    tc->translation = vector3(nodeAnim->mPositionKeys[s].mValue.x, nodeAnim->mPositionKeys[s].mValue.y, nodeAnim->mPositionKeys[s].mValue.z);
                    
                    at->translations.push_back(tc);
                }
                
                for(int s = 0; s < nodeAnim->mNumRotationKeys; s++) {
                    AnimationTransforms::Rotation* rc = new AnimationTransforms::Rotation();
                    rc->time = nodeAnim->mRotationKeys[s].mTime;
                    rc->rotation = quaternion(nodeAnim->mRotationKeys[s].mValue.w, nodeAnim->mRotationKeys[s].mValue.x, nodeAnim->mRotationKeys[s].mValue.y, nodeAnim->mRotationKeys[s].mValue.z);
                    
                    at->rotations.push_back(rc);
                }
                
                animation->transforms[nodeName] = at;
            }
        }
    }

    aiReleaseImport(scene);
    
    return(mesh);
}

void AssimpImporter::LoadAnimation(std::string filename, std::string animationName, Mesh* mesh) {
    // Use Assimp to import our scene
    const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    
    // Load animations
    if(scene->HasAnimations()) {
        for(int i = 0; i < scene->mNumAnimations; i++) {
            // Save the name
            // Create new animation
            Animation* animation = new Animation();
            animation->type = Animation::ANIMATIONTYPE_SKELETAL;
            animation->duration = scene->mAnimations[i]->mDuration;
            animation->framesPerSecond = scene->mAnimations[i]->mTicksPerSecond;
            
            mesh->animations[animationName] = animation;
            
            // Loop over Assimp's "channels" and figure out the discrete frame timestamps
            Array<double> frameTimes;
            for(int j = 0; j < scene->mAnimations[i]->mDuration; j++) {
                frameTimes.push_back(j);
            }
            
            // For each channel (bone) in the animation, get the keyframes
            for(int c = 0; c < scene->mAnimations[i]->mNumChannels; c++) {
                aiNodeAnim* nodeAnim = scene->mAnimations[i]->mChannels[c];
                std::string nodeName = nodeAnim->mNodeName.C_Str();
                
                AnimationTransforms* at = new AnimationTransforms();
                at->nodeName = nodeName;
                
                for(int s = 0; s < nodeAnim->mNumScalingKeys; s++) {
                    AnimationTransforms::Scaling* sc = new AnimationTransforms::Scaling();
                    sc->time = nodeAnim->mScalingKeys[s].mTime;
                    sc->scaling = vector3(nodeAnim->mScalingKeys[s].mValue.x,nodeAnim->mScalingKeys[s].mValue.y, nodeAnim->mScalingKeys[s].mValue.z);
                    
                    at->scaling.push_back(sc);
                }
                
                for(int s = 0; s < nodeAnim->mNumPositionKeys; s++) {
                    AnimationTransforms::Translation* tc = new AnimationTransforms::Translation();
                    tc->time = nodeAnim->mPositionKeys[s].mTime;
                    tc->translation = vector3(nodeAnim->mPositionKeys[s].mValue.x, nodeAnim->mPositionKeys[s].mValue.y, nodeAnim->mPositionKeys[s].mValue.z);
                    
                    at->translations.push_back(tc);
                }
                
                for(int s = 0; s < nodeAnim->mNumRotationKeys; s++) {
                    AnimationTransforms::Rotation* rc = new AnimationTransforms::Rotation();
                    rc->time = nodeAnim->mRotationKeys[s].mTime;
                    rc->rotation = quaternion(nodeAnim->mRotationKeys[s].mValue.w, nodeAnim->mRotationKeys[s].mValue.x, nodeAnim->mRotationKeys[s].mValue.y, nodeAnim->mRotationKeys[s].mValue.z);
                    
                    at->rotations.push_back(rc);
                }
                
                animation->transforms[nodeName] = at;
            }
        }
    }
    
    aiReleaseImport(scene);
}
