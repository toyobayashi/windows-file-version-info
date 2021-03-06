#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winver.h>
#include <strsafe.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "file_version_info.h"

typedef struct {
  uint16_t wLanguage;
  uint16_t wCodePage;
} translation_s;

static const char* errmsgs[] = {
  "",
  "Invalid fvi_t",
  "Invalid argument",
  "VS_FIXEDFILEINFO is NULL",
  "translation_s is NULL",
  "Windows API Error",
  "Unsupported platform",
};

const char* fvi_err(fvi_result code) {
  return errmsgs[code];
}

#ifdef _WIN32

#define FVI_LOWORD(i32) ((uint16_t)((i32) & 0xFFFF))
#define FVI_HIWORD(i32) ((uint16_t)(((uint32_t)(i32) >> 16) & 0xFFFF))

struct fvi {
  uint16_t* file_name;
  void* block;
  VS_FIXEDFILEINFO *ffi;
  UINT ffi_size;
  translation_s* trans;
  UINT trans_size;
  WCHAR* lang_buf;
  DWORD lang_count;
};

typedef struct fvi fvi_s;

static const WCHAR empty_string[1] = { 0 };

static const WCHAR* string_file_info_keys[] = {
  L"\\StringFileInfo\\%04x%04x\\Comments",
  L"\\StringFileInfo\\%04x%04x\\CompanyName",
  L"\\StringFileInfo\\%04x%04x\\FileDescription",
  L"\\StringFileInfo\\%04x%04x\\FileVersion",
  L"\\StringFileInfo\\%04x%04x\\InternalName",
  L"\\StringFileInfo\\%04x%04x\\LegalCopyright",
  L"\\StringFileInfo\\%04x%04x\\LegalTrademarks",
  L"\\StringFileInfo\\%04x%04x\\OriginalFilename",
  L"\\StringFileInfo\\%04x%04x\\PrivateBuild",
  L"\\StringFileInfo\\%04x%04x\\ProductName",
  L"\\StringFileInfo\\%04x%04x\\ProductVersion",
  L"\\StringFileInfo\\%04x%04x\\SpecialBuild"
};

fvi_t fvi_init(const uint16_t* file_name) {
  fvi_t info =
    (fvi_t) malloc(sizeof(fvi_s));
  if (info == NULL) return NULL;
  ZeroMemory(info, sizeof(fvi_s));

  HRESULT hr;
  size_t len = 0;
  hr = StringCchLengthW(file_name, STRSAFE_MAX_CCH, &len);
  if (FAILED(hr)) {
    return info;
  }
  uint16_t* copied_file_name =
    (uint16_t*) malloc(sizeof(uint16_t) * (len + 1));  // NOLINT
  if (copied_file_name == NULL) {
    return info;
  }
  hr = StringCchCopyW(copied_file_name, len + 1, file_name);
  if (FAILED(hr)) {
    free(copied_file_name);
    return info;
  }
  info->file_name = copied_file_name;

  DWORD handle;
  DWORD datasize = GetFileVersionInfoSizeW((LPCWSTR) file_name, &handle);

  if (datasize <= 0) return info;

  void* block = malloc(datasize);
  if (!block) return info;

  if (!GetFileVersionInfoW(file_name, handle, datasize, block)) {
    free(block);
    return info;
  }
  info->block = block;

  VS_FIXEDFILEINFO *ffi = NULL;
  UINT ffi_size = 0;
  VerQueryValueW(block, L"\\", (LPVOID*) &ffi, &ffi_size);  // NOLINT
  info->ffi = ffi;
  info->ffi_size = ffi_size;

  translation_s* trans = NULL;
  UINT trans_size = 0;
  VerQueryValueW(block, L"\\VarFileInfo\\Translation", (LPVOID*) &trans, &trans_size);  // NOLINT
  info->trans = trans;
  info->trans_size = trans_size;

  DWORD lang_count = 0;
  WCHAR* lang_buf = (WCHAR*) malloc(128 * sizeof(WCHAR));  // NOLINT
  ZeroMemory(lang_buf, 128 * sizeof(WCHAR));
  if (lang_buf == NULL) {
    return info;
  }
  if (trans == NULL) {
    lang_count = VerLanguageNameW(0x0409, lang_buf, 128);
  } else {
    if (trans_size >= 1) {
      uint32_t lang = ((*trans).wLanguage) |
                      ((*trans).wCodePage << 16);
      lang_count = VerLanguageNameW(lang & 0xFFFF, lang_buf, 128);
    }
  }
  info->lang_buf = lang_buf;
  info->lang_count = lang_count;

  return info;
}

void fvi_free(fvi_t info) {
  if (info != NULL) {
    if (info->block) free(info->block);
    if (info->file_name) free(info->file_name);
    if (info->lang_buf) free(info->lang_buf);
    free(info);
  }
}

#define FVI_FFI_RETURN(ret, default) \
  do { \
    if (result == NULL) return fvi_invalid_result; \
    if (!info || !info->ffi) { \
      *result = (default); \
    } else { \
      *result = (ret); \
    } \
    return fvi_ok; \
  } while (0)

fvi_result fvi_file_major_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_HIWORD(info->ffi->dwFileVersionMS), 0);
}

fvi_result fvi_file_minor_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_LOWORD(info->ffi->dwFileVersionMS), 0);
}

fvi_result fvi_file_build_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_HIWORD(info->ffi->dwFileVersionLS), 0);
}

fvi_result fvi_file_private_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_LOWORD(info->ffi->dwFileVersionLS), 0);
}

fvi_result fvi_product_major_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_HIWORD(info->ffi->dwProductVersionMS), 0);
}

fvi_result fvi_product_minor_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_LOWORD(info->ffi->dwProductVersionMS), 0);
}

fvi_result fvi_product_build_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_HIWORD(info->ffi->dwProductVersionLS), 0);
}

fvi_result fvi_product_private_part(fvi_t info, uint16_t* result) {
  FVI_FFI_RETURN(FVI_LOWORD(info->ffi->dwProductVersionLS), 0);
}

fvi_result fvi_is_debug(fvi_t info, bool* result) {
  FVI_FFI_RETURN(((info->ffi->dwFileFlags & info->ffi->dwFileFlagsMask) & VS_FF_DEBUG) != 0, false);  // NOLINT
}

fvi_result fvi_is_pre_release(fvi_t info, bool* result) {
  FVI_FFI_RETURN(((info->ffi->dwFileFlags & info->ffi->dwFileFlagsMask) & VS_FF_PRERELEASE) != 0, false);  // NOLINT
}

fvi_result fvi_is_patched(fvi_t info, bool* result) {
  FVI_FFI_RETURN(((info->ffi->dwFileFlags & info->ffi->dwFileFlagsMask) & VS_FF_PATCHED) != 0, false);  // NOLINT
}

fvi_result fvi_is_private_build(fvi_t info, bool* result) {
  FVI_FFI_RETURN(((info->ffi->dwFileFlags & info->ffi->dwFileFlagsMask) & VS_FF_PRIVATEBUILD) != 0, false);  // NOLINT
}

fvi_result fvi_is_special_build(fvi_t info, bool* result) {
  FVI_FFI_RETURN(((info->ffi->dwFileFlags & info->ffi->dwFileFlagsMask) & VS_FF_SPECIALBUILD) != 0, false);  // NOLINT
}

fvi_result fvi_language(fvi_t info, const uint16_t** result) {
  if (result == NULL) return fvi_invalid_result;
  if (info == NULL || info->lang_count == 0) {
    *result = empty_string;
  } else {
    *result = info->lang_buf;
  }
  return fvi_ok;
}

fvi_result fvi_file_name(fvi_t info, const uint16_t** result) {
  if (result == NULL) return fvi_invalid_result;
  if (info == NULL) return fvi_invalid_type;
  *result = info->file_name;
  return fvi_ok;
}

fvi_result fvi_string_file_info(fvi_t info,
                                uint32_t id,
                                const uint16_t** result,
                                uint32_t* len) {
  if (id > 11 || result == NULL || len == NULL) return fvi_invalid_result;
  if (info == NULL) {
    *result = empty_string;
    *len = 0;
    return fvi_ok;
  }

  uint32_t lang_count = 0;
  if (info->trans == NULL) {
    *result = empty_string;
    *len = 0;
    return fvi_ok;
  }
  if (info->trans_size == 0) return fvi_trans_not_found;
  WCHAR sub_block[64] = { 0 };
  HRESULT hr = StringCchPrintfW(sub_block,
                                64,
                                string_file_info_keys[id],
                                (*(info->trans)).wLanguage,
                                (*(info->trans)).wCodePage);
  if (FAILED(hr)) {
    return fvi_system_error;
  }
  if (!VerQueryValueW(info->block, sub_block, (LPVOID*) result, len)) {  // NOLINT
    *result = empty_string;
    *len = 0;
  } else {
    *len = *len - 1;
  }
  return fvi_ok;
}

#else
fvi_t fvi_init(const uint16_t* file_name) { return NULL; }
void fvi_free(fvi_t info) {}
fvi_result fvi_file_major_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_file_minor_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_file_build_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_file_private_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_product_major_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_product_minor_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_product_build_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_product_private_part(fvi_t info, uint16_t* result) {
  return fvi_not_supported;
}
fvi_result fvi_is_debug(fvi_t info, bool* result) {
  return fvi_not_supported;
}
fvi_result fvi_is_pre_release(fvi_t info, bool* result) {
  return fvi_not_supported;
}
fvi_result fvi_is_patched(fvi_t info, bool* result) {
  return fvi_not_supported;
}
fvi_result fvi_is_private_build(fvi_t info, bool* result) {
  return fvi_not_supported;
}
fvi_result fvi_is_special_build(fvi_t info, bool* result) {
  return fvi_not_supported;
}
fvi_result fvi_language(fvi_t info, const uint16_t** result) {
  return fvi_not_supported;
}
fvi_result fvi_file_name(fvi_t info, const uint16_t** result) {
  return fvi_not_supported;
}
fvi_result fvi_string_file_info(fvi_t info,
                                uint32_t id,
                                const uint16_t** result,
                                uint32_t* len) { return fvi_not_supported; }
#endif
