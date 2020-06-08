
#include <Render/Pipelines/DeferredRenderPipeline.h>
#include <Render/Pipelines/Deferred/GBufferPass.h>
#include <Render/Pipelines/Deferred/CombinePass.h>
#include <Render/Pipelines/Deferred/LightingPass.h>
#include <Render/Pipelines/Deferred/ForwardPass.h>
#include <Render/Pipelines/Deferred/RenderToScreenPass.h>
#include <Render/Pipelines/Deferred/TerrainPass.h>
#include <Render/Pipelines/Deferred/SkyboxPass.h>
#include <IO/Profiler.h>

void DeferredRenderPipeline::Initialize(int width, int height) {
    auto ri = m_renderPasses.begin();
    for(; ri != m_renderPasses.end(); ri++) {
        delete(*ri);
    }
    m_renderPasses.clear();
    
    // Set up render pipeline
    GBufferPass* gbuffer = new GBufferPass();
    gbuffer->Initialize(width, height);
    
    AddPass(gbuffer);
    
    TerrainPass* terrainPass = new TerrainPass();
    terrainPass->Initialize(width, height);
    terrainPass->SetDiffuseTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(0));
    terrainPass->SetPositionTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(1));
    terrainPass->SetNormalTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(2));
    terrainPass->SetAuxTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(3));
    terrainPass->SetDepthTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(4));
    
    AddPass(terrainPass);
    
    LightingPass* lighting = new LightingPass();
    lighting->Initialize(width, height);
    lighting->SetNormalTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(2));
    lighting->SetPositionTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(1));
    lighting->SetMaterialTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(3));
    
    AddPass(lighting);
    
    CombinePass* combine = new CombinePass();
    combine->Initialize(width, height);
    combine->SetDiffuseTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(0));
    combine->SetLightingTexture((Texture2D*)lighting->GetFramebuffer(0)->GetTexture(0));
    
    AddPass(combine);
    
    ForwardPass* forwardPass = new ForwardPass();
    forwardPass->Initialize(width, height);
    forwardPass->SetInputTexture((Texture2D*)combine->GetFramebuffer(0)->GetTexture(0));
    forwardPass->SetDepthTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(4));
    
    AddPass(forwardPass);
    
    SkyboxPass* skyboxPass = new SkyboxPass();
    skyboxPass->Initialize(width, height);
    skyboxPass->SetInputTexture((Texture2D*)forwardPass->GetFramebuffer(0)->GetTexture(0));
    skyboxPass->SetDepthTexture((Texture2D*)gbuffer->GetFramebuffer(0)->GetTexture(4));
    
    AddPass(skyboxPass);
    
    RenderToScreenPass* screenPass = new RenderToScreenPass();
    screenPass->Initialize(width, height);
    screenPass->SetInputTexture((Texture2D*)skyboxPass->GetFramebuffer(0)->GetTexture(0));
    
    AddPass(screenPass);
}

void DeferredRenderPipeline::Render(Scene* scene) {
    auto it = m_renderPasses.begin();
    for(; it != m_renderPasses.end(); it++) {
        PROFILE_START_AREA((*it)->GetGigaName());
        
        RenderPass* pass = (*it);
        pass->Render(scene);
        
        PROFILE_END_AREA((*it)->GetGigaName());
    }
}
