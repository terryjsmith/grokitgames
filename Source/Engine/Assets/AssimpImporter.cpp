
#include <Assets/AssimpImporter.h>
#include <Render/Bone.h>
#include <IO/ResourceSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/MaterialSystem.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct AssimpMaterial {
    std::string texture;
    Material* material;
};

Mesh* AssimpImporter::LoadFromFile(std::string filename) {
    Mesh* mesh = new Mesh();
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    MaterialSystem* materialSystem = GetSystem<MaterialSystem>();
    
    // Use Assimp to import our scene
    const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate);
    
    std::vector<AssimpMaterial*> materialList;
    
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
    
    // Load mesh data (and bones)
    for(int i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh* paiMesh = scene->mMeshes[i];

        Renderable* ret = new Renderable();
        mesh->renderable->children.push_back(ret);
        
        // Save the name
        mesh->name = paiMesh->mName.data;
        
        /* Initialize material data
        ret->material->diffuse = vector4(1.0, 1.0, 1.0, 1.0);
        ret->material->specular = vector4(0.0, 0.0, 0.0, 0.0);
        ret->material->ambient = vector4(1.0, 1.0, 1.0, 1.0);
        ret->material->shininess = 0.0;*/
        
        // Use the material index to set the data
        ret->diffuseTexture = dynamic_cast<Texture2D*>(resourceSystem->LoadResource(materialList[paiMesh->mMaterialIndex]->texture, "Texture2D"));
        ret->material = materialList[paiMesh->mMaterialIndex]->material;
        
        // Initialize our data
        std::vector<float> vertex_data;
        std::vector<unsigned int> index_data;
        
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
            vf->AddVertexAttrib(VERTEXTYPE_ATTRIB_BONES, 8, offset);
            offset += 8;
        }
        
        int vertexSize = vf->GetVertexSize();
        
        // Prepare vertex buffer
        vertex_data.resize(paiMesh->mNumVertices * vertexSize);
        memset(&vertex_data[0], 0, sizeof(float) * paiMesh->mNumVertices * vertexSize);
        
        // Load our vertices into our internal format
        for(int i = 0; i < paiMesh->mNumVertices; i++) {
            unsigned int offset = i * vf->GetVertexSize();
            
            const aiVector3D* pPos = &(paiMesh->mVertices[i]);
            const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : NULL;
            const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : NULL;

            vector4 position = vector4(pPos->x, pPos->z, pPos->y, 1.0);
            
            vertex_data[offset + 0] = position.x;
            vertex_data[offset + 1] = position.y;
            vertex_data[offset + 2] = position.z;
            
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
        
        // Create our bone mappings
        if(paiMesh->HasBones()) {
            for(int i = 0; i < paiMesh->mNumBones; i++) {
                char* boneName = paiMesh->mBones[i]->mName.data;
                auto bi = mesh->bones.find(boneName);
                
                if(bi == mesh->bones.end()) {
                    // Create a new bone
                    Bone* newBone = new Bone();
                    
                    // Save the name of the bone
                    newBone->name = paiMesh->mBones[i]->mName.data;
                    mesh->bones[boneName] = newBone;
                }
            }
        }
        
        if(paiMesh->HasBones()) {
            for(int i = 0; i < paiMesh->mNumBones; i++) {
                // Get the bone ID
                char* boneName = paiMesh->mBones[i]->mName.data;
                int boneOffset = 0;
                auto bi = mesh->bones.begin();
                for(; bi != mesh->bones.end(); bi++) {
                    if(strcmp(bi->first.c_str(), boneName) == 0) {
                        break;
                    }
                    boneOffset++;
                }
                
                // Save the vertex weights
                for(int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
                    aiBone* bone = paiMesh->mBones[i];
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
                        // If we get here, more bones than slots
                        assert(0);
                    }
                }
            }
        }
        
        // Load vertex buffer
        ret->vertexBuffer->Create(vf, vertex_data.size() / vf->GetVertexSize(), &vertex_data[0], false);
    }

    // Load bones
    std::map<Bone*, aiMatrix4x4> boneOffsets;
    for(int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* paiMesh = scene->mMeshes[i];
        
        if(paiMesh->HasBones()) {
            for(int j = 0; j < paiMesh->mNumBones; j++) {
                // Check to see if we need a new bone
                auto it = mesh->bones.find(paiMesh->mBones[j]->mName.data);
                
                // Save the bone's offset matrix
                boneOffsets[it->second] = paiMesh->mBones[j]->mOffsetMatrix;
            }
        }
    }
    
    // Load animations
    if(scene->HasAnimations()) {
        // Save the global inverse matrix for later use
        aiMatrix4x4 globalInverseTransform = scene->mRootNode->mTransformation.Inverse();
        
        for(int i = 0; i < scene->mNumAnimations; i++) {
            // Save the name
            std::string animationName = scene->mAnimations[i]->mName.data;
            
            // Create new animation
            Animation* animation = new Animation();
            animation->duration = scene->mAnimations[i]->mDuration;
            animation->framesPerSecond = scene->mAnimations[i]->mTicksPerSecond;
            
            // Loop over Assimp's "channels" and figure out the discrete frame timestamps
            std::vector<double> frameTimes;
            for(int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
                for(int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++) {
                    double t = scene->mAnimations[i]->mChannels[j]->mPositionKeys->mTime;
                    std::vector<double>::iterator it = std::find(frameTimes.begin(), frameTimes.end(), t);
                    if(it == frameTimes.end()) {
                        frameTimes.push_back(t);
                    }
                }
            }
            
            // For each frame, go over all of Assimp's channels (bones) and get the transform data out
            int counter = 0;
            for(; counter < frameTimes.size(); counter++) {
                // Save the frame time
                AnimationFrame* frame = new AnimationFrame;
                frame->time = frameTimes[counter];
                
                double frameTime = frameTimes[counter];
                
                // For each channel in the animation, get the location at the time
                for(int c = 0; c < scene->mAnimations[i]->mNumChannels; c++) {
                    aiNodeAnim* nodeAnim = scene->mAnimations[i]->mChannels[c];
                    
                    // Look for a bone
                    std::map<std::string, Bone*>::iterator it = mesh->bones.find(std::string(nodeAnim->mNodeName.data));
                    if(it == mesh->bones.end()) {
                        // Non-skeletal animation, use node transform
                    }
                    
                    // Find the scaling vector
                    aiVector3D scaling;
                    if(nodeAnim->mNumScalingKeys > 1) {
                        unsigned int index = 0;
                        for(int s = 0; s < nodeAnim->mNumScalingKeys - 1; s++) {
                            if(frame->time <= nodeAnim->mScalingKeys[s + 1].mTime) {
                                index = s;
                                break;
                            }
                        }
                        
                        // Calculate the difference between this time and the next time
                        float delta = nodeAnim->mScalingKeys[index + 1].mTime - nodeAnim->mScalingKeys[index].mTime;
                        float factor = (frame->time - nodeAnim->mScalingKeys[index].mTime) / delta;
                        assert(factor >= 0.0f && factor <= 1.0f);
                        
                        const aiVector3D& start = nodeAnim->mScalingKeys[index].mValue;
                        const aiVector3D& end = nodeAnim->mScalingKeys[index + 1].mValue;
                        aiVector3D difference = end - start;
                        scaling = start + difference * factor;
                    }
                    else {
                        scaling = nodeAnim->mScalingKeys[0].mValue;
                    }
                    
                    // Do the same for transform
                    aiVector3D translate;
                    if(nodeAnim->mNumPositionKeys > 1) {
                        unsigned int index = 0;
                        for(int s = 0; s < nodeAnim->mNumPositionKeys - 1; s++) {
                            if(frame->time <= nodeAnim->mPositionKeys[s + 1].mTime) {
                                index = s;
                                break;
                            }
                        }
                        
                        // Calculate the difference between this time and the next time
                        float delta = nodeAnim->mPositionKeys[index + 1].mTime - nodeAnim->mPositionKeys[index].mTime;
                        float factor = (frame->time - nodeAnim->mPositionKeys[index].mTime) / delta;
                        assert(factor >= 0.0f && factor <= 1.0f);
                        
                        const aiVector3D& start = nodeAnim->mPositionKeys[index].mValue;
                        const aiVector3D& end = nodeAnim->mPositionKeys[index + 1].mValue;
                        aiVector3D difference = end - start;
                        translate = start + difference * factor;
                    }
                    else {
                        translate = nodeAnim->mPositionKeys[0].mValue;
                    }

                    // And finally rotation
                    aiQuaternion rotation;
                    if(nodeAnim->mNumRotationKeys > 1) {
                        unsigned int index = 0;
                        for(int s = 0; s < nodeAnim->mNumRotationKeys - 1; s++) {
                            if(frame->time <= nodeAnim->mRotationKeys[s + 1].mTime) {
                                index = s;
                                break;
                            }
                        }
                        
                        // Calculate the difference between this time and the next time
                        float delta = nodeAnim->mRotationKeys[index + 1].mTime - nodeAnim->mRotationKeys[index].mTime;
                        float factor = (frame->time - nodeAnim->mRotationKeys[index].mTime) / delta;
                        assert(factor >= 0.0f && factor <= 1.0f);
                        
                        const aiQuaternion& start = nodeAnim->mRotationKeys[index].mValue;
                        const aiQuaternion& end = nodeAnim->mRotationKeys[index + 1].mValue;
                        aiQuaternion::Interpolate(rotation, start, end, factor);
                    }
                    else {
                        rotation = nodeAnim->mRotationKeys[0].mValue;
                    }
                    
                    AnimationTransform* transform = new AnimationTransform();
                    transform->scaling = vector3(scaling.x, scaling.y, scaling.z);
                    transform->translation = vector3(translate.x, translate.y, translate.z);
                    transform->rotation = quaternion(rotation.w, rotation.x, rotation.y, rotation.z);
                    
                    std::map<std::string, Bone*>::iterator bi = mesh->bones.find(nodeAnim->mNodeName.data);
                    size_t offset = std::distance(mesh->bones.begin(), bi);
                    
                    transform->objectID = (uint32_t)offset;
                    
                    frame->transforms.push_back(transform);
                }
            }
        }
    }
    
    aiReleaseImport(scene);
    
    return(mesh);
}
