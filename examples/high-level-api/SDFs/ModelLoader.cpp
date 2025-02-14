#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"
#include <stdio.h>
#include "Linxc.h"
#include "ModelLoader.hpp"
#include "vector.hpp"
#include "hashmap.hpp"
#include "Maths/Point2.hpp"
#include "Maths/Util.hpp"
#include "RayTrace.hpp"
#include "lgfx-astral/Application.hpp"
#include "random.hpp"

void LoadFile(void *ctx, const char *filename, const int is_mtl, const char *obj_filename, char **buffer, size_t *len)
{
    long string_size = 0, read_size = 0;
    FILE *handler = fopen(filename, "r");

    if (handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        *buffer = (char *)malloc(sizeof(char) * (string_size + 1));
        read_size = fread(*buffer, sizeof(char), (size_t)string_size, handler);
        (*buffer)[string_size] = '\0';
        if (string_size != read_size)
        {
            free(*buffer);
            *buffer = NULL;
        }
        fclose(handler);
    }

    *len = read_size;
}
Maths::Vec3 Barycentric(Maths::Vec3 A, Maths::Vec3 B, Maths::Vec3 C, float amount1, float amount2)
{
    return A + (B - A) * amount1 + (C - A) * amount2;
}
Model LoadModel(text fileName, AstralCanvas::Shader *loadShader, LGFXShaderState loadShaderState)
{
    Maths::Vec3 minBounds = Maths::Vec3(FloatMax);
    Maths::Vec3 maxBounds = Maths::Vec3(FloatMin);

    tinyobj_attrib_t attribs;
    tinyobj_shape_t *shapes;
    tinyobj_material_t *materials;
    usize shapeCount;
    usize materialsCount;
    tinyobj_parse_obj(&attribs, &shapes, &shapeCount, &materials, &materialsCount, fileName, LoadFile, NULL, TINYOBJ_FLAG_TRIANGULATE);

    //collections::hashmap<Maths::Point2, u32> vertexMap = collections::hashmap<Maths::Point2, u32>(GetCAllocator(), &Maths::Point2Hash, &Maths::Point2Eql);
    collections::vector<Vertex> vertices = collections::vector<Vertex>(GetCAllocator());
    collections::vector<u32> indices = collections::vector<u32>(GetCAllocator());
    collections::denseset<u32> vertexMap = collections::denseset<u32>(GetCAllocator());

    for (usize i = 0; i < attribs.num_faces; i++)
    {
        tinyobj_vertex_index_t face = attribs.faces[i];
        //Maths::Point2 key = Maths::Point2(face.v_idx, face.vn_idx);
        u32 *index = vertexMap.Get(face.v_idx);
        if (index == NULL || *index == 0)
        {
            Vertex vertex;
            Maths::Vec3 vertexPos = Maths::Vec3(
                attribs.vertices[face.v_idx * 3 + 0],
                attribs.vertices[face.v_idx * 3 + 1],
                attribs.vertices[face.v_idx * 3 + 2]);
            Maths::Vec3 vertexNormal = Maths::Vec3(
                attribs.normals[face.vn_idx * 3 + 0],
                attribs.normals[face.vn_idx * 3 + 1],
                attribs.normals[face.vn_idx * 3 + 2]
            );
            vertexPos.Y *= -1.0f;
            vertexNormal *= -1.0f;
            vertex.position = Maths::Vec4(vertexPos, 0.0f);
            vertex.normal = Maths::Vec4(vertexNormal, 0.0f);
            //vertex.position.Y *= -1.0f;
            //vertex.normal.Y *= -1.0f;
            
            //vertexMap.Add(key, (u32)vertices.count);
            vertexMap.Insert(face.v_idx, (u32)vertices.count);
            vertices.Add(vertex);
            minBounds = Maths::Vec3::Min(minBounds, vertexPos);
            maxBounds = Maths::Vec3::Max(maxBounds, vertexPos);
        }
        indices.Add(*vertexMap.Get(face.v_idx));
    }

    Random random = Random::FromTime();

    Model model = {};
    model.vertices = vertices.ToOwnedArray();
    model.indices = indices.ToOwnedArray();

    /*u32 pcCount = 4000;
    model.vertices = collections::Array<Vertex>(GetCAllocator(), pcCount);

    float totalWeights = 0.0f;
    collections::Array<float> weights = collections::Array<float>(GetCAllocator(), model.indices.length / 3);
    for (u32 i = 0; i < model.indices.length / 3; i++)
    {
        Maths::Vec3 v0 = vertices.ptr[model.indices.data[i * 3 + 0]].position.ToVec3();
        Maths::Vec3 v1 = vertices.ptr[model.indices.data[i * 3 + 1]].position.ToVec3();
        Maths::Vec3 v2 = vertices.ptr[model.indices.data[i * 3 + 2]].position.ToVec3();

        float weight = 0.5f * Maths::Vec3::Cross(v1 - v0, v2 - v0).Length();
        totalWeights += weight;
        weights.data[i] = weight;
    }
    for (u32 i = 0; i < pcCount; i++)
    {
        u32 triangle = 0;
        float weightLeft = random.NextFloat(totalWeights); // i % (model.indices.length / 3);
        for (u32 j = 0; j < model.indices.length / 3; j++)
        {
            if (weightLeft <= weights.data[j])
            {
                triangle = j;
                break;
            }
            weightLeft -= weights.data[j];
        }

        float r1 = random.NextFloat(1.0f);
        float r2 = random.NextFloat(1.0f);

        Vertex point = {};
        point.position = Maths::Vec4(Barycentric(
            vertices.ptr[model.indices.data[triangle * 3 + 0]].position.ToVec3(),
            vertices.ptr[model.indices.data[triangle * 3 + 1]].position.ToVec3(),
            vertices.ptr[model.indices.data[triangle * 3 + 2]].position.ToVec3(),
            r1,
            r2
        ), 0.01f + weights.data[triangle] * 2.0f);
        model.vertices.data[i] = point;
    }
    weights.deinit();*/

    LGFXBufferCreateInfo bufferInfo = {};
    bufferInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferInfo.size = sizeof(Vertex) * model.vertices.length;
    bufferInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_StorageBuffer | LGFXBufferUsage_TransferDest);
    model.vertexBuffer = LGFXCreateBuffer(AstralCanvas::applicationInstance.device, &bufferInfo);

    bufferInfo = {};
    bufferInfo.memoryUsage = LGFXMemoryUsage_GPU_ONLY;
    bufferInfo.size = sizeof(u32) * model.indices.length;
    bufferInfo.bufferUsage = (LGFXBufferUsage)(LGFXBufferUsage_StorageBuffer | LGFXBufferUsage_TransferDest);
    model.indexBuffer = LGFXCreateBuffer(AstralCanvas::applicationInstance.device, &bufferInfo);

    LGFXSetBufferDataOptimizedData(model.vertexBuffer, NULL, (u8 *)model.vertices.data, model.vertices.length * sizeof(Vertex));
    LGFXSetBufferDataOptimizedData(model.indexBuffer, NULL, (u8 *)model.indices.data, model.indices.length * sizeof(u32));

    model.minBounds = minBounds;
    model.maxBounds = maxBounds;

    constexpr float resolution = 16.0f;
    constexpr float oneOverResolution = 1.0f / resolution;

    u32 iterationWidth = (u32)ceilf((model.maxBounds.X - model.minBounds.X) * resolution);
    u32 iterationHeight = (u32)ceilf((model.maxBounds.Y - model.minBounds.Y) * resolution);
    u32 iterationDepth = (u32)ceilf((model.maxBounds.Z - model.minBounds.Z) * resolution);

    LGFXTextureCreateInfo createInfo = {};
    createInfo.width = iterationWidth;
    createInfo.height = iterationHeight;
    createInfo.depth = iterationDepth;
    createInfo.mipLevels = 1;
    createInfo.sampleCount = 1;
    createInfo.usage = (LGFXTextureUsage)(LGFXTextureUsage_Sampled | LGFXTextureUsage_Storage);
    createInfo.format = LGFXTextureFormat_RGBA16Float;
    model.meshSDFTexture = LGFXCreateTexture(AstralCanvas::applicationInstance.device, &createInfo);

    LGFXFence fence = LGFXCreateFence(AstralCanvas::applicationInstance.device, false);
    LGFXCommandBuffer cmd = LGFXCreateCommandBuffer(AstralCanvas::applicationInstance.device, false);
    LGFXCommandBufferBegin(cmd, true);

    LGFXTextureTransitionLayout(AstralCanvas::applicationInstance.device, model.meshSDFTexture, LGFXTextureLayout_General, cmd, 0, 1);

    ShaderMeshMetadata metadata = {};
    metadata.iterationWidth = iterationWidth; //(u32)indices.count;
    metadata.iterationHeight = iterationHeight;
    metadata.iterationDepth = iterationDepth;
    metadata.indicesCount = (u32)model.indices.length;
    metadata.minExtents = Maths::Vec4(model.minBounds, 0.0f);
    metadata.maxExtents = Maths::Vec4(model.maxBounds, 0.0f);

    loadShader->SetShaderVariableComputeBuffer("Vertices", model.vertexBuffer);
    loadShader->SetShaderVariableComputeBuffer("Indices", model.indexBuffer);
    loadShader->SetShaderVariableTexture("writeTo", model.meshSDFTexture);
    loadShader->SetShaderVariable("MeshData", &metadata, sizeof(ShaderMeshMetadata));
    LGFXUseShaderState(cmd, loadShaderState);
    loadShader->SyncUniformsWithGPU(cmd);
    LGFXDispatchCompute(cmd, (u32)ceilf(metadata.iterationWidth / 8.0f), (u32)ceilf(metadata.iterationHeight / 8.0f), (u32)ceilf(metadata.iterationDepth / 8.0f));

    LGFXTextureTransitionLayout(AstralCanvas::applicationInstance.device, model.meshSDFTexture, LGFXTextureLayout_ShaderReadOptimal, cmd, 0, 1);

    LGFXCommandBufferEnd(cmd);
    LGFXCommandBufferExecute(cmd, fence, NULL, NULL);
    LGFXAwaitFence(fence);
    LGFXDestroyCommandBuffer(cmd);
    LGFXDestroyFence(fence);

    vertices.deinit();
    indices.deinit();
    vertexMap.deinit();
    tinyobj_attrib_free(&attribs);
    if (shapes != NULL) {
        tinyobj_shapes_free(shapes, shapeCount);
    }
    if (materials != NULL) {
        tinyobj_materials_free(materials, materialsCount);
    }

    return model;
}