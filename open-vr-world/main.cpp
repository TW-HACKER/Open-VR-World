#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "bx/float4x4_t.h"
#include "GLFW/glfw3.h"
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD 
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "GLFW/glfw3native.h"
#define WNDW_WIDTH 960
#define WNDW_HEIGHT 540
#include <iostream>
#include <filesystem>
#include <fstream>


struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static PosColorVertex cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] =
{
	0, 1, 2,
	1, 3, 2,
	4, 6, 5,
	5, 6, 7,
	0, 2, 4,
	4, 2, 6,
	1, 5, 3,
	5, 7, 3,
	0, 4, 1,
	4, 5, 1,
	2, 3, 6,
	6, 3, 7,
};

bgfx::ShaderHandle loadShader(std::string_view path)
{
    if(bgfx::getRendererType() != bgfx::RendererType::Vulkan) {
        std::cerr << "Renderer must be Vulkan\n";
        abort();
    }
    std::filesystem::path base_path = "shaders/";
    std::string filePath = (base_path/path).generic_string();

    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    const auto fileSize = std::filesystem::file_size(filePath);
    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    in.read((char*)mem->data, fileSize);
    mem->data[mem->size - 1] = '\0';

    return bgfx::createShader(mem);
}


int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WNDW_WIDTH, WNDW_HEIGHT, "Open VR World", NULL, NULL);
    bgfx::PlatformData pd;
    
    #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD 
        #if ENTRY_CONFIG_USE_WAYLAND // examples entry options define
            pd.ndt      = glfwGetWaylandDisplay(); 
        #else 
            pd.ndt      = glfwGetX11Display(); 
            pd.nwh = (void*)glfwGetX11Window(window);
        #endif 
    #elif BX_PLATFORM_OSX 
        pd.ndt      = NULL; 
    #elif BX_PLATFORM_WINDOWS 
        pd.ndt      = NULL; 
        pd.nwh = glfwGetWin32Window(window);
    #endif // BX_PLATFORM_*
    bgfx::setPlatformData(pd);
    
    bgfx::Init bgfxInit;
    bgfxInit.type = bgfx::RendererType::Vulkan;
    bgfxInit.resolution.width = WNDW_WIDTH;
    bgfxInit.resolution.height = WNDW_HEIGHT;
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    
    bgfxInit.platformData = pd;
    bgfx::init(bgfxInit);
    unsigned int counter = 0;
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WNDW_WIDTH, WNDW_HEIGHT);

    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

    bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    bx::float4x4_t view;
    float proj[16];
    float mtx[16];
    const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
    bx::mtxLookAt((float*)&view, eye, at);
    bx::mtxProj(proj, 60.0f, float(WNDW_WIDTH) / float(WNDW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, (float*)&view, proj);
    while(!glfwWindowShouldClose(window)) 
    {
        bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
        bgfx::setTransform(mtx);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::submit(0, program);
        bgfx::frame();
        counter++;
        
        glfwPollEvents();
        if ( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
    bgfx::shutdown();       
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
