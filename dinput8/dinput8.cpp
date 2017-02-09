// dinput8.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "IDirectInput8Hook.h"
#include <Strsafe.h>

// Direct input module
static HMODULE mDirectInputModule = NULL;

// Real function pointer
HRESULT(WINAPI *SteamDirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) = NULL;
HRESULT(WINAPI *RealDirectInput8Create)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) = NULL;
HRESULT(WINAPI *RealDllCanUnloadNow)(void) = NULL;
HRESULT(WINAPI *RealDllGetClassObject)(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv) = NULL;
HRESULT(WINAPI *RealDllRegisterServer)(void) = NULL;
HRESULT(WINAPI *RealDllUnregisterServer)(void) = NULL;

// DirectInput8Create
extern "C" HRESULT WINAPI __E__0__(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	// Log call
	OutputDebugString(L"DirectInput8Create hook has been called!\n");

	// Create DirectInput8 instance
	HRESULT hr = RealDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	// Fetch real DirectInput8 instance
	IDirectInput8 * realDirectInput8Interface = (IDirectInput8 *)*ppvOut;

	// Wrap it in our proxy class
	*ppvOut = new IDirectInput8Hook(realDirectInput8Interface);

	// Return creation result
	return hr;
}

// DllCanUnloadNow
extern "C" HRESULT WINAPI __E__1__(void)
{
	return RealDllCanUnloadNow();
}

// DllGetClassObject
extern "C" HRESULT WINAPI __E__2__(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
	return RealDllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer
extern "C" HRESULT WINAPI __E__3__(void)
{
	return RealDllRegisterServer();
}

// DllUnregisterServer
extern "C" HRESULT WINAPI __E__4__(void)
{
	return RealDllUnregisterServer();
}

// Entry point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID v)
{
	// Proxy DLL is being attached
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Load direct input module
		TCHAR path[MAX_PATH];
		if (GetSystemDirectory(path, MAX_PATH) != 0)
		{
			StringCchCat(path, MAX_PATH, TEXT("\\dinput8.dll"));
		}
		mDirectInputModule = LoadLibrary(path);

		// Direct input module couldn't be loaded
		if (!mDirectInputModule)
		{
			// Report to caller that we failed to initialize our own module
			return FALSE;
		}

		// Fetch the real function pointer
		RealDirectInput8Create = (HRESULT(WINAPI *)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN))GetProcAddress(mDirectInputModule, "DirectInput8Create");
		RealDllCanUnloadNow = (HRESULT(WINAPI *)(void))GetProcAddress(mDirectInputModule, "DllCanUnloadNow");
		RealDllGetClassObject = (HRESULT(WINAPI *)(REFCLSID, REFIID, LPVOID FAR*))GetProcAddress(mDirectInputModule, "DllGetClassObject");
		RealDllRegisterServer = (HRESULT(WINAPI *)(void))GetProcAddress(mDirectInputModule, "DllRegisterServer");
		RealDllUnregisterServer = (HRESULT(WINAPI *)(void))GetProcAddress(mDirectInputModule, "DllUnregisterServer");

		// Log initialization
		OutputDebugString(L"Initialized dinput8.dll proxy module!\n");
	}

	// Proxy DLL is being detached
	if (reason == DLL_PROCESS_DETACH)
	{
		// Free direct input module
		FreeLibrary(mDirectInputModule);

		// Log shutdown
		OutputDebugString(L"dinput8.dll proxy module has been shut down!\n");
	}

	// We successfully initialized our module
	return TRUE;
}
