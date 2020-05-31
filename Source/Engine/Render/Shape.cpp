
#include <Render/Shape.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>

Shape::Shape() {
    applyLighting = false;
}

void Shape::SetVertices(VertexFormat* fmt, float* data, int count) {
    renderable = new Renderable();
    
    if(renderable->vertexBuffer) {
        delete renderable->vertexBuffer;
    }
    
    renderable->vertexBuffer = GetSystem<RenderSystem>()->CreateVertexBuffer();
    renderable->vertexBuffer->Create(fmt, count, data, false);
}
