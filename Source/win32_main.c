#include <types.h>
#include <glad/glad.h>
#include <gl/GL.h>

typedef struct Bitmap {
    BITMAPINFO info;
    int width;
    int height;
    int bytes_per_pixel;
    int memory_size;
    void* memory;
} Bitmap;

static volatile bool window_is_running = false;

void win32_draw_bitmap(HWND window_handle, Bitmap* bitmap, int x, int y) {
    HDC hdc = GetDC(window_handle);
    RECT client_rect;
    GetClientRect(window_handle, &client_rect);


    u32 client_width = client_rect.right - client_rect.left;
    u32 client_height = client_rect.bottom - client_rect.top;

    StretchDIBits(hdc, x, y, client_width, client_height, 
                    0, 0, bitmap->width, bitmap->height, 
                    bitmap->memory, &bitmap->info,
                    DIB_RGB_COLORS, SRCCOPY);
}

void win32_resize_bitmap(Bitmap* bitmap, u32 width, u32 height) {
    bitmap->width = width;
    bitmap->height = height;
    bitmap->bytes_per_pixel = 4;

    if (bitmap->memory) {
        VirtualFree(bitmap->memory, 0, MEM_RELEASE);
    }

    bitmap->info.bmiHeader.biSize = sizeof(bitmap->info.bmiHeader);
    bitmap->info.bmiHeader.biWidth = bitmap->width;
    bitmap->info.bmiHeader.biHeight = -bitmap->height;
    bitmap->info.bmiHeader.biPlanes = 1;
    bitmap->info.bmiHeader.biBitCount = 32;
    bitmap->info.bmiHeader.biCompression = BI_RGB;
    bitmap->info.bmiHeader.biSizeImage = 0;
    bitmap->info.bmiHeader.biXPelsPerMeter = 0;
    bitmap->info.bmiHeader.biYPelsPerMeter = 0;
    bitmap->info.bmiHeader.biClrUsed = 0;
    bitmap->info.bmiHeader.biClrImportant = 0;

    bitmap->memory_size = bitmap->bytes_per_pixel *  bitmap->width * bitmap->height;
    bitmap->memory = VirtualAlloc(0, bitmap->memory_size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
}

void set_bitmap_gradient(Bitmap *bitmap, u32 x_offset, u32 y_offset) {
    int stride = bitmap->width * bitmap->bytes_per_pixel;

    u8* row = (u8*)bitmap->memory;    
    for(u32 y = 0; y < (u32)bitmap->height; y++) {
        u32* pixel = (u32*)row;
        for(u32 x = 0; x < (u32)bitmap->width; x++) {
            const u32 red = ((100 + x + x_offset) << 16);
            const u32 green = (10  << 8);
            const u32 blue = ((100 + y + y_offset) << 0);
            
            const u32 rgb = red|green|blue;

            *pixel++ = rgb;
        }
        row += stride;
    }
}

LRESULT CALLBACK custom_window_procedure(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (message) {
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0); 
            window_is_running = false;
        } break;

        case WM_PAINT: { // Repaint window when its dirty
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
    window_class.style = CS_HREDRAW|CS_VREDRAW;
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

// Next thing:
// - CreateThread() for software renderer
// - wglMakeCurrent()
/* 
BOOL SetPropA(
  [in]           HWND   hWnd,
  [in]           LPCSTR lpString,
  [in, optional] HANDLE hData
);
*/

void win32_opengl_init(HWND window) {
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC hdc = GetDC(window);
    int pf_index = ChoosePixelFormat(hdc, &pfd);
    DescribePixelFormat(hdc, pf_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    SetPixelFormat(hdc, pf_index, &pfd);

    HGLRC opengl_context = wglCreateContext(hdc);
    if(!wglMakeCurrent(hdc, opengl_context) || !gladLoadGL()) {
        CRASH;
    }

    ReleaseDC(window, hdc);
}


DWORD WINAPI update_and_render(LPVOID param) {
    Bitmap bitmap = {0};
    HWND window_handle = (HWND)param;
    win32_opengl_init(window_handle);
    HDC hdc = GetDC(window_handle);
    
    // u32 x_offset = 0;

    // win32_resize_bitmap(&bitmap, width, height);

    while (window_is_running) {
        // set_bitmap_gradient(&bitmap, x_offset, 0);
        // win32_draw_bitmap(window_handle, &bitmap, 0, 0);
        // x_offset++;

        RECT client_rect;
        GetClientRect(window_handle, &client_rect);
        int width = client_rect.right - client_rect.left;
        int height = client_rect.bottom - client_rect.top;

        glViewport(0, 0, width, height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        glRotatef(0.1f, 0, 0, 1);
    
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.6f, -0.75f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.6f, -0.75f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.0f, 0.75f);
        glEnd();

        SwapBuffers(hdc);
    }

    ReleaseDC(window_handle, hdc);
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    HWND window_handle = window_create(hInstance, 800, 600, "Testing Window");

    window_is_running = true;
    HANDLE render_thread = CreateThread(0, 0, update_and_render, (void*)window_handle, 0, 0);
    CloseHandle(render_thread);

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