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
#include <strsafe.h>
#include <fileapi.h>
#include <cstdint>
#include <cstddef>
#include <map>

using namespace nm;
using namespace std;

namespace
{

struct ProductionPackage
{
  uintptr_t *vft0x0;
  // imcomplete
};

struct chunk_info
{
  int64_t offset_to_data;
  uint32_t decompressed_size;
  uint32_t compressed_size;
  uint32_t data0x10;
  uint16_t data0x14;
  uint8_t data0x16;
  uint8_t data0x17;
  // only valid for NGS1 databin
  uint8_t data0x18;
  uint8_t data0x19;
  uint8_t data0x1a;
  uint8_t data0x1b;
  uint32_t data0x1c;
};

bool ProductionPackage_vfunction2 (ProductionPackage *, uintptr_t, const struct chunk_info *, void *);
struct chunk_info *ProductionPackage_vfunction3 (ProductionPackage *, int32_t);
HANDLE open_mod_file (int32_t);

//void *(*tmcl_malloc)(void *, uint32_t);
VFPHook <decltype (ProductionPackage_vfunction2)> *ProductionPackage_vfunction2_hook;
VFPHook <decltype (ProductionPackage_vfunction3)> *ProductionPackage_vfunction3_hook;

struct index_and_original_size
{
  int32_t index;
  uint32_t original_size;
};
map <const struct chunk_info *, struct index_and_original_size> chunk_info_to_index_and_original_size;

/*
bool
load_data_ngs1 (ProductionPackage *thisptr, uintptr_t param2, const struct chunk_info *ci, void *out_buf)
{
  HANDLE hFile;

  // We assume that chunk_info mapping is already made.
  auto e = chunk_info_to_index_and_original_size.find (ci)->second;
  if (e.first == -1)
    goto BAIL;

  hFile = open_mod_file (e.first);
  if (hFile == INVALID_HANDLE_VALUE)
    goto BAIL;

  DWORD nBytesRead;
  BOOL r;
  if (ci->data0x1a == 0x11)
    {
      void *buf = tmcl_malloc (out_buf, ci->decompressed_size);
      if (r = ReadFile (hFile, buf, ci->decompressed_size, &nBytesRead, nullptr))
	{
	  auto init_ngs1_tmcl_buf = reinterpret_cast<void (*)(ProductionPackage *, void *, void*)>(*(thisptr->vft0x0 + 1));
	  init_ngs1_tmcl_buf (thisptr, out_buf, buf);
	}
    }
  else
    r = ReadFile (hFile, out_buf, ci->decompressed_size, &nBytesRead, nullptr);
  CloseHandle (hFile);
  return r;

BAIL:
  return load_data_hook->call (thisptr, param2, ci, out_buf);
}
*/

// This function loads the data which corresponds to the passed chunk info.
// param2 is never used.
bool
ProductionPackage_vfunction2 (ProductionPackage *thisptr, uintptr_t param2, const struct chunk_info *ci, void *out_buf)
{
  HANDLE hFile;

  // We assume that chunk_info mapping is already made.
  auto e = chunk_info_to_index_and_original_size.find (ci)->second;
  if (e.index == -1)
    goto BAIL;

  hFile = open_mod_file (e.index);
  if (hFile == INVALID_HANDLE_VALUE)
    goto BAIL;

  DWORD nBytesRead;
  BOOL r;
  r = ReadFile (hFile, out_buf, ci->decompressed_size, &nBytesRead, nullptr);
  CloseHandle (hFile);
  return r;

BAIL:
  return ProductionPackage_vfunction2_hook->call (thisptr, param2, ci, out_buf);
}

// This function loads a chunk info from databin.
struct chunk_info *
ProductionPackage_vfunction3 (ProductionPackage *thisptr, int32_t index)
{
  auto ci = ProductionPackage_vfunction3_hook->call (thisptr, index);
  if (!ci)
    return ci;

  struct index_and_original_size i {index, ci->decompressed_size};
  auto e = chunk_info_to_index_and_original_size.emplace (ci, i).first->second;

  HANDLE hFile;
  if ((hFile = open_mod_file (index)) == INVALID_HANDLE_VALUE)
    {
      e.index = -1;
      ci->decompressed_size = e.original_size;
      return ci;
    }
  e.index = index;

  LARGE_INTEGER size;
  GetFileSizeEx (hFile, &size);
  ci->decompressed_size = size.u.LowPart;
  CloseHandle (hFile);
  return ci;
}

HANDLE
open_mod_file (int32_t index)
{
  TCHAR name[16];
  StringCbPrintf (name, sizeof (name), TEXT ("%05d.dat"), index);

  const TCHAR *mod_dirs[] = { TEXT("mods\\"), };

  HANDLE hFile = INVALID_HANDLE_VALUE;
  for (auto &i : mod_dirs)
    {
      TCHAR path[MAX_PATH];
      StringCbCopy (path, sizeof (path), i);
      StringCbCat (path, sizeof (path), name);

      hFile = CreateFile (path, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
			  FILE_ATTRIBUTE_NORMAL, nullptr);
      if (hFile != INVALID_HANDLE_VALUE)
	break;
    }
  return hFile;
}

void
init ()
{

/*
  switch (image_id)
  {
  case ImageId::NGS1SteamAE:
    load_data_hook = new VFPHook {0x0994b50, load_data_ngs1};
    get_chunk_info_hook = new VFPHook {0x0994b58, get_chunk_info};
    tmcl_malloc = reinterpret_cast<decltype (tmcl_malloc)>(base_of_image + 0x07e5630);
    break;
  case ImageId::NGS2SteamAE:
    load_data_hook = new VFPHook {0x18ef6f0, load_data};
    get_chunk_info_hook = new VFPHook {0x18ef6f8, get_chunk_info};
    break;
  case ImageId::NGS2SteamJP:
    load_data_hook = new VFPHook {0x18ee6f0, load_data};
    get_chunk_info_hook = new VFPHook {0x18ee6f8, get_chunk_info};
    break;
  }
*/
  ProductionPackage_vfunction2_hook = new VFPHook {0x19e1de8, ProductionPackage_vfunction2};
  ProductionPackage_vfunction3_hook = new VFPHook {0x19e1df0, ProductionPackage_vfunction3};
}

} // namespace

extern "C" DLLEXPORT BOOL
DllMain (HINSTANCE hinstDLL,
	 DWORD fdwReason,
	 LPVOID lpvReserved)
{
  switch (fdwReason)
  {
  case DLL_PROCESS_ATTACH:
    DisableThreadLibraryCalls (hinstDLL);
    init ();
    break;
  case DLL_PROCESS_DETACH:
    break;
  default:
    break;
  }
  return TRUE;
}
