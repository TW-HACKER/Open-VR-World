#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "GLFW/glfw3.h"
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD 
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "GLFW/glfw3native.h"
#define WNDW_WIDTH 960
#define WNDW_HEIGHT 540
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
    while(!glfwWindowShouldClose(window)) 
    {
        bgfx::touch(0);
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
