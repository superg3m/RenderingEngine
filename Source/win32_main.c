#include <types.h>

typedef struct Bitmap {
    BITMAPINFO info;
    int width;
    int height;
    int bytes_per_pixel;
    int memory_size;
    void* memory;
} Bitmap;

void wgl_context_create(HDC dc_handle);

static bool interacting_with_left_menu = false;
static bool window_is_running = false;
internal Bitmap bitmap;

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
        case WM_CREATE: {
        
        } break;

        case WM_SIZE: { // Resize
            RECT client_rect;
            GetClientRect(handle, &client_rect);
            int width = client_rect.right - client_rect.left;
            int height = client_rect.bottom - client_rect.top;
            win32_resize_bitmap(&bitmap, width, height);
        } break;

        case WM_CLOSE: {
            PostQuitMessage(0);
        } break;

        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;

        case WM_NCLBUTTONDBLCLK: {
            // Handle the double-click on the top-left icon
            if (wParam == HTSYSMENU) {
                interacting_with_left_menu = TRUE;
            }

            return DefWindowProcA(handle, message, wParam, lParam);
        } break;

        case WM_SYSCOMMAND: {
            // Handle the double-click on the top-left icon
            if ((wParam & 0xFFF0) == SC_MOUSEMENU) {
                // you can full screen and minimize here like normal behaviour
                return 0;
            }
            
            if (((wParam & 0xFFF0) == SC_CLOSE) && interacting_with_left_menu) {
                interacting_with_left_menu = FALSE;
                return 0;
            }

            return DefWindowProcA(handle, message, wParam, lParam);
        } break;

        case WM_PAINT: { // Repaint window when its dirty
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(handle, &paint);

            // SetPropA(handle, "", something_here)
            // ckit_window_update_callback(handle);
            // ckit_window_render_callback(handle);

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


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    HWND window_handle = window_create(hInstance, 800, 600, "Testing Window");

    window_is_running = true;
    s32 x_offset = 0;

    while (window_is_running) {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                window_is_running = false;
                break;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
 
        set_bitmap_gradient(&bitmap, x_offset, 0);
        win32_draw_bitmap(window_handle, &bitmap, 0, 0);

        x_offset++;
    }

    return 0;
}