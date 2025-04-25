#include <types.h>
#include <gl/GL.h>

static volatile bool window_is_running = false;
static HANDLE render_thread_handle = NULL;
static HGLRC opengl_context = NULL;

static double rotation_angle = 0.0;
static LARGE_INTEGER last_time, current_time, frequency;
static const float ROTATION_SPEED = 180.0f;

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
    ReleaseDC(window, hdc);

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC*)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(1);
    } else {
        MessageBoxA(0, "Failed to load wglSwapIntervalEXT!", "Error", MB_ICONERROR);
    }
    
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&last_time);
}

void cleanup_opengl(HWND window) {
    if (opengl_context) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(opengl_context);
        opengl_context = NULL;
    }
}

DWORD WINAPI update_and_render(LPVOID param) {
    HWND window_handle = (HWND)param;
    HDC hdc = GetDC(window_handle);
    win32_opengl_init(window_handle, hdc);

    double current_time = os_query_performance_counter();
    double previous_time = current_time;
    
    while (window_is_running) {
        current_time = os_query_performance_counter();
        double delta_time = current_time - previous_time;
        
        rotation_angle += ROTATION_SPEED * delta_time / 1000.0f;
        if (rotation_angle >= 360.0) {
            rotation_angle -= 360.0;
        }
        
        RECT client_rect;
        GetClientRect(window_handle, &client_rect);
        int width = client_rect.right - client_rect.left;
        int height = client_rect.bottom - client_rect.top;
        
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glLoadIdentity();
        glRotatef((float)rotation_angle, 0, 0, 1);
        
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.6f, -0.75f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.6f, -0.75f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.0f, 0.75f);
        glEnd();
            
        hdc = GetDC(window_handle);
        SwapBuffers(hdc);
        ReleaseDC(window_handle, hdc);
   

		double seconds_per_frame = delta_time / 1000.0;
		u64 fps = (u64)(1.0 / seconds_per_frame);

        char buffer[1024] = {0};
		snprintf(buffer, ArrayCount(buffer), "%fms / FPS: %d\n", (float)delta_time, (u32)fps);
        OutputDebugStringA(buffer);

		previous_time = current_time;
    }
    
    cleanup_opengl(window_handle);
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    HWND window_handle = window_create(hInstance, 800, 600, "Testing Window");
    window_is_running = true;
    
    render_thread_handle = CreateThread(0, 0, update_and_render, (void*)window_handle, 0, 0);
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