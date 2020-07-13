
/**
 * Structs
 */
struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 emissionColor;
    float emissionStrength;
    float shininess;
};

/**
 * Constants
 */
const int LIGHTTYPE_DIRECTIONAL = 1;
const int LIGHTTYPE_POINT = 2;
const int LIGHTTYPE_SPOTLIGHT = 3;

/**
 * In variables
 */
in vec2 frag_texcoord;

/**
 * Uniforms
 */

// Sample textures
uniform sampler2D texturePosition;
uniform sampler2D textureNormal;
uniform sampler2D textureMaterial;
uniform sampler2D textureMaterialLookup;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightAngle;
uniform vec3 lightColour;
uniform int lightType;
uniform sampler2D lightShadowMap[4];
uniform samplerCube lightShadowMap3D;
uniform mat4 lightSpaceMatrix[4];
uniform float cascadeEnd[4];
uniform float farPlane;
uniform float nearPlane;
uniform float materialID;

// Model view matrix
uniform mat4 worldviewInverseMatrix;

/**
 * Out variables
 */
layout (location = 0)out vec3 out_lighting;

/**
 * Functions
 */
vec3 ApplyLight(vec3 colour, vec3 normal, vec3 position, vec4 light_position, vec3 eye, Material material) {
    // Attenuation from light
    float attenuation = 1.0;
    
    // Surface to light
    vec3 surfaceToLight = normalize(light_position.xyz - position.xyz);
    
    // Directional light
    if(lightType == LIGHTTYPE_DIRECTIONAL) {
        light_position = -light_position;
        surfaceToLight = normalize(light_position.xyz);
    }
    else {
        // Non-directional lights
        float distanceToLight = length(light_position.xyz - position);
        attenuation = clamp(1.0 - distanceToLight*distanceToLight/(farPlane*farPlane), 0.0, 1.0);
    }
    
    if(lightType == LIGHTTYPE_SPOTLIGHT) {
        vec3 ld = normalize(surfaceToLight);
        vec3 sd = normalize(-lightDirection);
        
        // inside the cone?
        if (dot(sd,ld) < lightAngle) {
            attenuation = 0;
        }
    }
    
    // Calculate diffuse lighting factor
    float NdotL = max(dot(normalize(normal), surfaceToLight), 0.0);
    vec3 diffuse = NdotL * lightColour;
    
    // And specular
    vec3 viewDir = normalize(cameraPosition - position);
    vec3 incidenceVec = reflect(-surfaceToLight, normal);
    float spec = pow(max(dot(viewDir, incidenceVec), 0.0), material.shininess);
    vec3 specular = lightColour * (spec * material.specular);

    specular = clamp(specular, 0.0, 1.0);
    return(attenuation * (diffuse + specular));
}

float ApplyShadow2D(sampler2D tex, vec4 position) {
    float bias = 0.005;
    
    vec3 projcoords = position.xyz / position.w;
    projcoords = projcoords * 0.5 + 0.5;
    
    // Get the closest depth to the light at this position
    float shadow = 0.0;
    float closestDepth = texture(tex, projcoords.xy).r;
    
    // Get the current depth at this position
    float currentDepth = projcoords.z;//length(position.xyz);// / farPlane;
    
    // Are we in the shadow?
    shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    
    vec2 poissonDisk[4] = vec2[](
      vec2( -0.94201624, -0.39906216 ),
      vec2( 0.94558609, -0.76890725 ),
      vec2( -0.094184101, -0.92938870 ),
      vec2( 0.34495938, 0.29387760 )
    );
    
    shadow = 0;
    for (int i=0;i<4;i++){
      if ( texture( tex, projcoords.xy + poissonDisk[i]/700.0 ).r  <  projcoords.z-bias ){
        shadow+=0.2;
      }
    }
    
    // inside the cone?
    if (projcoords.x < 0 || projcoords.x > 1) {
        shadow = 0;
    }
    
    if (projcoords.y < 0 || projcoords.y > 1) {
        shadow = 0;
    }
    
    return(shadow);
}

float VectorToDepth(vec3 Vec) {
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float f = farPlane;
    float n = nearPlane;
    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

float ApplyShadowCube(vec4 position, vec4 light_position, vec3 globalNormal) {
    vec3 fragToLight = vec3(position) - vec3(light_position);
    
    float bias = 0.005;
    
    // Get the closest depth to the light at this position
    // float closestDepth = texture(lightShadowMap3D, fragToLight).r;

    // Get the current depth at this position
    float currentDepth = VectorToDepth(fragToLight);
    
    // Are we in the shadow?
    //float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
    
    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    
    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(fragToLight);
    float diskRadius = 0.05;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(lightShadowMap3D, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        //closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    
    //return(closestDepth);
    return(shadow);
}

/**
 * Main
 */
void main()
{
    vec4 position = texture(texturePosition, frag_texcoord.xy);
    vec3 normal = texture(textureNormal, frag_texcoord.xy).xyz;
    
    // Re-construct our material from our material texture
    Material mat;
    /*float materialID = texture(textureMaterial, frag_texcoord.xy).y;*/
    float y = materialID / 1024.0;
    
    mat.diffuse = texture(textureMaterialLookup, vec2(0.0, y)).xyz;
    mat.specular = texture(textureMaterialLookup, vec2(0.25, y)).xyz;
    mat.emissionColor = texture(textureMaterialLookup, vec2(0.50, y)).xyz;
    vec3 extra = texture(textureMaterialLookup, vec2(0.75, y)).xyz;
    mat.emissionStrength = extra.x;
    mat.shininess = extra.y;
    
    // Iterate over our lights to calculate colours
    vec3 linearColor = vec3(0);
    float shadow = 0.0;
    vec4 globalPosition = worldviewInverseMatrix * position;
    vec3 globalNormal = mat3(worldviewInverseMatrix) * normal;
    
    //float vertexDepth = texture(textureMaterial, frag_texcoord.xy).z;
    float vertexDepth = length(cameraPosition - vec3(globalPosition));
    int cascadeIndex = 0;
    if(lightType == LIGHTTYPE_DIRECTIONAL) {
        if(vertexDepth > cascadeEnd[0]) {
            cascadeIndex = 1;
        }
        
        if(vertexDepth > cascadeEnd[1]) {
            cascadeIndex = 2;
        }
        
        if(vertexDepth > cascadeEnd[2]) {
            cascadeIndex = 3;
        }
    }
    
    // For point-lights, no transform
    vec4 lightSpacePosition = lightSpaceMatrix[cascadeIndex] * globalPosition;
    vec3 light_normal = mat3(lightSpaceMatrix[cascadeIndex]) * globalNormal;
    
    // Use the light type to determine passes for shadow maps
    if(lightType == LIGHTTYPE_DIRECTIONAL) {
        float normalOffset[5] = float[](0.00001,0.00003,0.0005,0.001, 0.001);
        
        lightSpacePosition = lightSpaceMatrix[cascadeIndex] * (globalPosition/* + (vec4(globalNormal, 1.0) * normalOffset[cascadeIndex])*/);
        shadow = ApplyShadow2D(lightShadowMap[cascadeIndex], lightSpacePosition);
        
        /*if(cascadeIndex == 0) {
            lightSpacePosition = lightSpaceMatrix[1] * (globalPosition * normalOffset[1]);
            shadow += ApplyShadow2D(lightShadowMap[1], lightSpacePosition);
        }*/
        
        /*lightSpacePosition = lightSpaceMatrix[1] * globalPosition;
        shadow += ApplyShadow2D(lightShadowMap[1], lightSpacePosition);
        
        lightSpacePosition = lightSpaceMatrix[2] * globalPosition;
        shadow += ApplyShadow2D(lightShadowMap[2], lightSpacePosition);*/
        shadow = clamp(shadow, 0.0, 1.0);
    }
    
    if(lightType == LIGHTTYPE_POINT) {
        shadow = ApplyShadowCube(globalPosition, vec4(lightPosition, 1.0), globalNormal);
    }
    
    if(lightType == LIGHTTYPE_SPOTLIGHT) {
        shadow = ApplyShadow2D(lightShadowMap[0], lightSpacePosition);
    }
    
    // Apply lighting
    linearColor = ApplyLight(vec3(1.0), globalNormal, globalPosition.xyz, vec4(lightPosition, 1.0), cameraPosition, mat);

    // Set shadow to max
    linearColor = clamp(linearColor, 0.0, 1.0);
    out_lighting = linearColor.xyz * vec3(1.0 - shadow);
    
    /*float r, g, b;
    r = g = b = 0;
    if(cascadeIndex == 0) {
        r = 1.0;
    }
    if(cascadeIndex == 1) {
        g = 1.0;
    }
    if(cascadeIndex == 2) {
        r = 1.0;
        b = 1.0;
    }
    if(cascadeIndex == 3) {
        b = 1.0;
    }
    
    out_lighting = linearColor.xyz * vec3(r, g, b) * vec3(1.0 - shadow);*/
}
