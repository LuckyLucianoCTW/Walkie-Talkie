#pragma once
#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include <d3d9.h>
#include <d3dx9.h> 
#include <Windows.h>
#include <tchar.h>
#include <vector> 
#include <Python.h>
#define DIRECTINPUT_VERSION 0x0800
 

using namespace std;
using namespace ImGui;

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pd3dDevice;
D3DPRESENT_PARAMETERS g_d3dpp;
char* text_to_send;
vector<string>ChatLog;
void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects(); 
}

char* text_saved;

PyObject* pName, * pModule;
vector<string>to_be_Send;
bool sendMessage;
void PythonShit(void* params)
{ 
	printf("Joined Python Shit\n"); 
	PyObject* pFunc;
	PyObject* pArgs, * pValue;
	Py_Initialize();
	pName = PyUnicode_FromString("client");
	if (pName != NULL) {
		printf("Passed ClientPy\n");
		pModule = PyImport_Import(pName);
		Py_DECREF(pName);
		if (pModule != NULL)
		{
			printf("Passed Module\n");
			pFunc = PyObject_GetAttrString(pModule, "InitRecvMessages"); 
			PyObject* pValue_sock = PyObject_CallObject(pFunc, NULL);
			if (pValue_sock != NULL) {
				pArgs = PyTuple_New(1);
				PyTuple_SetItem(pArgs, 0, pValue_sock);
				while (1) {
					Sleep(100);
					if (sendMessage) { 
						sendMessage = false; 
						PyObject* new_pArgs = PyTuple_New(1);
						PyObject* text = PyUnicode_FromString(text_saved);
						PyTuple_SetItem(new_pArgs, 0, text);
						PyObject_CallObject(PyObject_GetAttrString(pModule, "SendMessage"), new_pArgs);
					}
					pValue = PyObject_CallObject(PyObject_GetAttrString(pModule, "ReceiveMessages"), pArgs);
					if (pValue != NULL)
						if (strlen(PyBytes_AS_STRING(pValue)) > 0) {
							const char* text_recv = PyBytes_AS_STRING(pValue);
							ChatLog.push_back(text_recv); 
						}
				}
			}

		}
	}
}
 
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0; 
	}
 
	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

  
void DisplayWalkieTalkie()
{
	SetNextWindowSize(ImVec2(435, 700));

	SetNextWindowPos(ImVec2(0.5f, 0.5f));
	if (Begin("WalkieTalkie", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
	{
		 
		PushID(1); 
		ListBoxHeader("", ImVec2(-1, 500)); 
		for (unsigned int i = 0; i < ChatLog.size(); ++i)
		{
			Text(ChatLog[i].c_str());
		}
		 
		PopID(); 
		ImGui::ListBoxFooter();
		PushItemWidth(-1);  
		if (InputText("", text_to_send, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{ 
			ChatLog.push_back(text_to_send);
			strcpy(text_saved, text_to_send);
			sendMessage = true; 
			strcpy(text_to_send, "");

		}
		PopItemWidth();
		End();
	} 
}

void Render_Walkie()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();  
  
	DisplayWalkieTalkie();
    // Rendering
    ImGui::EndFrame();
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(0.1f * 255.0f), (int)(0.1f * 255.0f), (int)(0.1f * 255.0f), (int)(0.1f * 255.0f));
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    // Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    AllocConsole();
    freopen("CON", "w", stdout);
    SetConsoleTitle("[DEBUG] WalkieTalkie");
    printf("Starting debugger...\n\n");  
	text_to_send = (char*)malloc(256 * sizeof(char));
	text_saved = (char*)malloc(256 * sizeof(char));
	strcpy(text_to_send, "");
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Chess Window"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, _T("Walkie-Talkie"), WS_OVERLAPPEDWINDOW, 100, 100, 450, 700, NULL, NULL, wc.hInstance, NULL);
	while ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		Sleep(100);
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	while (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		Sleep(100);
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PythonShit, 0, 0, 0);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		Render_Walkie();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	return 0;
}