#include "Core/Core.h"
#include <Windows.h>

const char windowTitle[] = "Cube Renderer";

bool running = true;

void* bufferMemory;
int bufferWidth;
int bufferHeight;
BITMAPINFO bufferBitmapInfo;

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            running = false;
            break;
        }

        case WM_SIZE: 
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            bufferWidth = rect.right - rect.left;
            bufferHeight = rect.bottom - rect.top;

            int bufferSize = bufferWidth * bufferHeight * sizeof(unsigned int);

            if (bufferMemory)
            {
                VirtualFree(bufferMemory, 0, MEM_RELEASE);
            }

            bufferMemory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            bufferBitmapInfo.bmiHeader.biSize = sizeof(bufferBitmapInfo.bmiHeader);
            bufferBitmapInfo.bmiHeader.biWidth = bufferWidth;
            bufferBitmapInfo.bmiHeader.biHeight = bufferHeight;
            bufferBitmapInfo.bmiHeader.biPlanes = 1;
            bufferBitmapInfo.bmiHeader.biBitCount = 32;
            bufferBitmapInfo.bmiHeader.biCompression = BI_RGB;

            break;
        }

        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }

    }
    return result;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
    // Create Window Class
    WNDCLASS windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpszClassName = "Window Class";
    windowClass.lpfnWndProc = WindowCallback;
    // Register Window Class
	RegisterClass(&windowClass);

    //Create Window
	HWND window = CreateWindow(windowClass.lpszClassName, windowTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, nullptr);
    HDC hdc = GetDC(window);

    while (running) {
        //Get Windows messages to allow the OS to communicate with the app
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        //Display the buffer within the window
        StretchDIBits(hdc, 0, 0, bufferWidth, bufferHeight, 0, 0, bufferWidth, bufferHeight, bufferMemory, &bufferBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }

    return 0;
}