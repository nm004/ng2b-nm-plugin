/*
 * NINJA GAIDEN Master Collection NM Plugin by Nozomi Miyamori
 * is marked with CC0 1.0. This file is a part of NINJA GAIDEN
 * Master Collection NM Plugin.
 */

#if defined(_MSVC_LANG)
# define DLLEXPORT __declspec (dllexport)
# define WIN32_LEAN_AND_MEAN
#else
# define DLLEXPORT
#endif

#include "util.hpp"
#include <windows.h>
#include <fileapi.h>
#include <strsafe.h>
#include <cstdint>
#include <cassert>

using namespace nm;
using namespace std;

namespace {

//BOOL pre_init (LPCWSTR);
//bool check_dlls ();

//SimpleInlineHook<decltype (SetCurrentDirectoryW)> *SetCurrentDirectoryW_hook
//= new SimpleInlineHook {SetCurrentDirectoryW, pre_init};
//SimpleInlineHook<decltype (check_dlls)> *check_dlls_hook;

//Patch<Bytes<7>> *patch1;

struct Sigma2;

void Sigma2_vfunction1 (Sigma2 *);

VFPHook <decltype (Sigma2_vfunction1)> Sigma2_vfunction1_hook = VFPHook {0x19eba58, Sigma2_vfunction1};

void
Sigma2_vfunction1 (Sigma2 *thisptr)
{
  // Dll search paths starting from the current directory
  const TCHAR *search_paths[] = { TEXT("plugin\\"), };
  for (auto &i : search_paths)
    {
      // Main thread sets the current directory where the executable sits.
      TCHAR path[MAX_PATH];
      StringCbCopy (path, sizeof path, i);
      SetDllDirectory (path);
      StringCbCat (path, sizeof path, TEXT ("*.dll"));

      WIN32_FIND_DATA findFileData;
      HANDLE hFindFile = FindFirstFile (path, &findFileData);
      if (hFindFile == INVALID_HANDLE_VALUE)
	continue;
      do
        {
	  if (GetModuleHandle (findFileData.cFileName))
	    continue;
	  LoadLibrary (findFileData.cFileName);
        }
      while (FindNextFile(hFindFile, &findFileData));
      FindClose (hFindFile);
    }

  Sigma2_vfunction1_hook.call (thisptr);
}



// "check_dlls()" is called from WinMain. WinMain expects that "check_dlls()" returns true to
// continue the process, otherwise it aborts the process.
//
// We load plugins here. This always returns true.
//bool
//check_dlls ()
//{
  //delete check_dlls_hook;
  //return true;
//}

// This is needed to keep SteamDRM from wrongly decoding our codes.
// We postpone the execution of our codes by hooking SetCurrentDirectoryW, which
// is called from the WinMain function.
//BOOL
//pre_init (LPCWSTR lpPathName)
//{
  //constexpr auto XOR_R8_NOP4 = make_bytes (0x4d, 0x31, 0xc0,  0x0f, 0x1f, 0x40, 0x00);
  //switch (image_id)
  //{
  //case ImageId::NGS1SteamAE:
    //check_dlls_hook = new SimpleInlineHook {0x571fd0, check_dlls};
    // This allows users to run multiple instances of the game.
    //patch1 = new Patch {0x056c463, XOR_R8_NOP4};
    //break;
  //case ImageId::NGS2SteamAE:
    //check_dlls_hook = new SimpleInlineHook {0xb5c460, check_dlls};
    //patch1 = new Patch {0x1340d3b, XOR_R8_NOP4};
    //break;
  //case ImageId::NGS2SteamJP:
    //check_dlls_hook = new SimpleInlineHook {0xb5c4b0, check_dlls};
    //patch1 = new Patch {0x1340b0b, XOR_R8_NOP4};
    //break;
  //}

  //delete SetCurrentDirectoryW_hook;
  //return SetCurrentDirectoryW (lpPathName);
//}

HMODULE d3d9dll;

} // namespace

static HRESULT (*Direct3DCreate9Ex_orig) (UINT, uintptr_t);
extern "C" DLLEXPORT HRESULT
Direct3DCreate9Ex (UINT SDKVersion, uintptr_t unnamedParam2) { return Direct3DCreate9Ex_orig (SDKVersion, unnamedParam2); }

extern "C" DLLEXPORT BOOL
DllMain (HINSTANCE hinstDLL,
	 DWORD fdwReason,
	 LPVOID lpvReserved)
{
  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    {
      DisableThreadLibraryCalls (hinstDLL);
      SetDllDirectory ("");
      HMODULE dbghelpdll = LoadLibrary ("d3d9");
      SetDllDirectory (nullptr);
      Direct3DCreate9Ex_orig = reinterpret_cast <decltype (Direct3DCreate9Ex_orig)> (GetProcAddress (d3d9dll, "Direct3DCreate9Ex"));
    }
    break;
  default:
    break;
  }
  return TRUE;
}



