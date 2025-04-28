#include <types.h>
#include <gl/GL.h>

static volatile bool window_is_running = false;
static HANDLE render_thread_handle = NULL;
static HGLRC opengl_context = NULL;

typedef BOOL (WINAPI PFNWGLSWAPINTERVALEXTPROC)(int);
PFNWGLSWAPINTERVALEXTPROC* wglSwapIntervalEXT = NULL;

double os_query_performance_counter() {
    LARGE_INTEGER performance_counter_frequency;
    QueryPerformanceFrequency(&performance_counter_frequency);

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    double milliseconds_elapsed = ((double)(counter.QuadPart * 1000.0) / (double)performance_counter_frequency.QuadPart);

    return milliseconds_elapsed;
}

LRESULT CALLBACK custom_window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (message) {
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0);
            window_is_running = false;
        } break;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(handle, &paint);
            EndPaint(handle, &paint);
        } break;
       
        default: {
            result = DefWindowProcA(handle, message, wParam, lParam);
        } break;
    }
    return result;
}

HWND window_create(HINSTANCE hInstance, int width, int height, const char* name) {
    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    window_class.lpfnWndProc = custom_window_procedure;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = GetModuleHandle(NULL);
    // window_class.hIcon = icon_handle;
    // window_class.hCursor = cursor_handle;
    // window_class.hbrBackground = NULL;
    // window_class.lpszMenuName = NULL;
    window_class.lpszClassName = name;
    RegisterClassA(&window_class);
    // Date: May 04, 2024
    // TODO(Jovanni): Extended Window Styles (look into them you can do cool stuff)
    // WS_EX_ACCEPTFILES 0x00000010L (The window accepts drag-drop files.)
    DWORD dwStyle = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
    HWND handle = CreateWindowExA(0, name, name, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
    return handle;
}

void win32_opengl_init(HWND window, HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pf_index = ChoosePixelFormat(hdc, &pfd);
    DescribePixelFormat(hdc, pf_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    SetPixelFormat(hdc, pf_index, &pfd);
    opengl_context = wglCreateContext(hdc);
    if(!wglMakeCurrent(hdc, opengl_context)) {
        CRASH;
    }

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC*)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(1);
    } else {
        MessageBoxA(0, "Failed to load wglSwapIntervalEXT!", "Error", MB_ICONERROR);
    }

    ReleaseDC(window, hdc);
}

void cleanup_opengl(HWND window) {
    if (opengl_context) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(opengl_context);
        opengl_context = NULL;
    }
}

typedef void (UpdateRenderFunc)(float delta_time, int width, int height);

FILETIME get_last_write_time(const char* path) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
        return data.ftLastWriteTime;
    }
    FILETIME zero = {0};
    return zero;
}

BOOL filetime_changed(FILETIME a, FILETIME b) {
    return CompareFileTime(&a, &b) != 0;
}


DWORD WINAPI render_thread(LPVOID param) {
    HWND window_handle = (HWND)param;
    HDC hdc = GetDC(window_handle);
    win32_opengl_init(window_handle, hdc);

    const char* dll_name = "render.dll";
    const char* temp_dll = "render_temp.dll";

    FILETIME last_write_time = get_last_write_time(dll_name);
    if (!CopyFileA(dll_name, temp_dll, FALSE)) {
        CRASH;
    }

    HMODULE render_module = LoadLibraryA(temp_dll);
    UpdateRenderFunc* update_and_render = (UpdateRenderFunc*)GetProcAddress(render_module, "update_and_render");
    if (!update_and_render) {
        CRASH;
    }

    double current_time = os_query_performance_counter();
    double previous_time = current_time;

    while (window_is_running) {
        FILETIME new_time = get_last_write_time(dll_name);
        if (filetime_changed(new_time, last_write_time)) {
            last_write_time = new_time;
            if (render_module) {
                update_and_render = NULL;
                FreeLibrary(render_module);
                render_module = NULL;
            }

            Sleep(10);
        
            if (CopyFileA(dll_name, temp_dll, FALSE)) {
                render_module = LoadLibraryA(temp_dll);
                if (render_module) {
                    update_and_render = (UpdateRenderFunc*)GetProcAddress(render_module, "update_and_render");
                    if (!update_and_render) {
                        OutputDebugStringA("Invalid update and render pointer\n");
                        CRASH;
                    }
                } else {
                    OutputDebugStringA("Invalid load library\n");
                    CRASH;
                }
            } else {
                OutputDebugStringA("Failed to copy file\n");
                CRASH;
            }
        }

        current_time = os_query_performance_counter();
        double delta_time = current_time - previous_time;
        double delta_time_seconds = delta_time / 1000.0f;

        hdc = GetDC(window_handle);
        RECT client_rect;
        GetClientRect(window_handle, &client_rect);
        int width = client_rect.right - client_rect.left;
        int height = client_rect.bottom - client_rect.top;

        if (update_and_render) {
            update_and_render((float)delta_time, width, height);
        }

        SwapBuffers(hdc);
        ReleaseDC(window_handle, hdc);

        u64 fps = (u64)(1.0 / delta_time_seconds);
        char buffer[1024] = {0};
        snprintf(buffer, sizeof(buffer), "%fms / FPS: %d\n", (float)delta_time, (u32)fps);
        OutputDebugStringA(buffer);

        previous_time = current_time;
    }

    FreeLibrary(render_module);
    cleanup_opengl(window_handle);
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    HWND window_handle = window_create(hInstance, 800, 600, "Testing Window");
    window_is_running = true;
    
    render_thread_handle = CreateThread(0, 0, render_thread, (void*)window_handle, 0, 0);
    CloseHandle(render_thread_handle);

    while (window_is_running) {
        MSG msg;
        if(GetMessage(&msg, 0, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            window_is_running = false;
        }
    }
    
    return 0;
}