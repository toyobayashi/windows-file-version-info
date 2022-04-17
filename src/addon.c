#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winver.h>
#include <strsafe.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "addon.h"
#include "file_version_info.h"

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      const char* err_message = error_info->error_message;        \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (err_message == NULL)               \
            ? "empty error message"                               \
            : err_message;                                        \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while (0)

static void finalize_instance(napi_env env,
                              void* finalize_data,
                              void* finalize_hint) {
  napi_delete_reference(env, (napi_ref) finalize_data);
}

static void finalize_obj(napi_env env,
                         void* finalize_data,
                         void* finalize_hint) {
  fvi_free((fvi_t) finalize_data);
}

static napi_value js_fvi_constructor(napi_env env, napi_callback_info info) {
  napi_value external;
  void* data = NULL;
  napi_value this_arg;
  size_t argc = 1;
  NAPI_CALL(env, napi_get_cb_info(
    env, info, &argc, &external, &this_arg, NULL));
  NAPI_CALL(env, napi_get_value_external(env, external, &data));
  NAPI_CALL(env, napi_wrap(env, this_arg, data, finalize_obj, NULL, NULL));
  return this_arg;
}

#define STRING_TABLE_GETTER(fname, id) \
  static napi_value getter_##fname(napi_env env, napi_callback_info info) { \
    fvi_t data; \
    napi_value this_arg; \
    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &this_arg, NULL)); \
    NAPI_CALL(env, napi_unwrap(env, this_arg, &data)); \
    char16_t* res = NULL; \
    uint32_t res_size = 0; \
    fvi_result r = fvi_string_file_info(data, (id), &res, &res_size); \
    if (r != fvi_ok) { \
      napi_throw_error(env, NULL, fvi_err(r)); \
      return NULL; \
    } \
    napi_value ret; \
    NAPI_CALL(env, napi_create_string_utf16(env, res, res_size, &ret)); \
    return ret; \
  }

STRING_TABLE_GETTER(comments, FVI_SFI_COMMENTS)
STRING_TABLE_GETTER(company_name, FVI_SFI_COMPANYNAME)
STRING_TABLE_GETTER(file_description, FVI_SFI_FILEDESCRIPTION)
STRING_TABLE_GETTER(file_version, FVI_SFI_FILEVERSION)
STRING_TABLE_GETTER(internal_name, FVI_SFI_INTERNALNAME)
STRING_TABLE_GETTER(legal_copyright, FVI_SFI_LEGALCOPYRIGHT)
STRING_TABLE_GETTER(legal_trademarks, FVI_SFI_LEGALTRADEMARKS)
STRING_TABLE_GETTER(original_filename, FVI_SFI_ORIGINALFILENAME)
STRING_TABLE_GETTER(private_build, FVI_SFI_PRIVATEBUILD)
STRING_TABLE_GETTER(product_name, FVI_SFI_PRODUCTNAME)
STRING_TABLE_GETTER(product_version, FVI_SFI_PRODUCTVERSION)
STRING_TABLE_GETTER(special_build, FVI_SFI_SPECIALBUILD)

#define UINT_GETTER(fname) \
  static napi_value getter_##fname(napi_env env, napi_callback_info info) { \
    fvi_t data; \
    napi_value this_arg; \
    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &this_arg, NULL)); \
    NAPI_CALL(env, napi_unwrap(env, this_arg, &data)); \
    uint16_t res = 0; \
    fvi_result r = fvi_##fname(data, &res); \
    if (r != fvi_ok) { \
      napi_throw_error(env, NULL, fvi_err(r)); \
      return NULL; \
    } \
    napi_value ret; \
    NAPI_CALL(env, napi_create_uint32(env, res, &ret)); \
    return ret; \
  }

UINT_GETTER(file_major_part)
UINT_GETTER(file_minor_part)
UINT_GETTER(file_build_part)
UINT_GETTER(file_private_part)
UINT_GETTER(product_major_part)
UINT_GETTER(product_minor_part)
UINT_GETTER(product_build_part)
UINT_GETTER(product_private_part)

#define BOOL_GETTER(fname) \
  static napi_value getter_##fname(napi_env env, napi_callback_info info) { \
    fvi_t data; \
    napi_value this_arg; \
    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &this_arg, NULL)); \
    NAPI_CALL(env, napi_unwrap(env, this_arg, &data)); \
    bool res = false; \
    fvi_result r = fvi_##fname(data, &res); \
    if (r != fvi_ok) { \
      napi_throw_error(env, NULL, fvi_err(r)); \
      return NULL; \
    } \
    napi_value ret; \
    NAPI_CALL(env, napi_get_boolean(env, res, &ret)); \
    return ret; \
  }

BOOL_GETTER(is_debug)
BOOL_GETTER(is_pre_release)
BOOL_GETTER(is_patched)
BOOL_GETTER(is_private_build)
BOOL_GETTER(is_special_build)

#define UTF16_GETTER(fname) \
  static napi_value getter_##fname(napi_env env, napi_callback_info info) { \
    fvi_t data; \
    napi_value this_arg; \
    NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &this_arg, NULL)); \
    NAPI_CALL(env, napi_unwrap(env, this_arg, &data)); \
    const uint16_t* res = NULL; \
    fvi_result r = fvi_##fname(data, &res); \
    if (r != fvi_ok) { \
      napi_throw_error(env, NULL, fvi_err(r)); \
      return NULL; \
    } \
    napi_value ret; \
    NAPI_CALL(env, napi_create_string_utf16( \
      env, res, NAPI_AUTO_LENGTH, &ret)); \
    return ret; \
  }

UTF16_GETTER(file_name)
UTF16_GETTER(language)

static napi_value
get_version_info_static(napi_env env, napi_callback_info info) {
  napi_value file_name;
  size_t argc = 1;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &file_name, NULL, NULL));

  napi_valuetype type;
  NAPI_CALL(env, napi_typeof(env, file_name, &type));

  if (type != napi_string) {
    napi_throw_type_error(env, NULL, "Expect path string");
    return NULL;
  }

  size_t len_exclude_null = 0;
  NAPI_CALL(env, napi_get_value_string_utf16(env,
                                             file_name,
                                             NULL,
                                             0,
                                             &len_exclude_null));

  size_t len = (len_exclude_null + 1) * sizeof(char16_t);
  char16_t* filename = (char16_t*) malloc(len);  // NOLINT
  if (!filename) return NULL;
  napi_get_value_string_utf16(env,
                              file_name,
                              filename,
                              len,
                              &len_exclude_null);

  fvi_t file_version_info = fvi_init(filename);
  free(filename);
  if (file_version_info == NULL) {
    napi_throw_error(env, NULL, "Open file failed");
    return NULL;
  }

  napi_ref cons_ref;
  napi_value constructor;
  NAPI_CALL(env, napi_get_instance_data(env, (void**) &cons_ref));  // NOLINT
  NAPI_CALL(env, napi_get_reference_value(env, cons_ref, &constructor));
  napi_value external;
  NAPI_CALL(env, napi_create_external(env,
    file_version_info, NULL, NULL, &external));
  napi_value ret;
  NAPI_CALL(env, napi_new_instance(env, constructor, 1, &external, &ret));
  return ret;
}

napi_value create_addon(napi_env env, napi_value exports) {
  napi_value true_value;
  NAPI_CALL(env, napi_get_boolean(env, true, &true_value));
  napi_property_descriptor properties = {
    "__esModule", NULL, NULL, NULL, NULL, true_value, napi_default, NULL
  };
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &properties));

  napi_property_descriptor prototype[] = {
    { "comments", NULL,
      NULL, getter_comments, NULL, NULL,
      napi_configurable, NULL },
    { "companyName", NULL,
      NULL, getter_company_name, NULL, NULL,
      napi_configurable, NULL },
    { "fileDescription", NULL,
      NULL, getter_file_description, NULL, NULL,
      napi_configurable, NULL },
    { "fileVersion", NULL,
      NULL, getter_file_version, NULL, NULL,
      napi_configurable, NULL },
    { "internalName", NULL,
      NULL, getter_internal_name, NULL, NULL,
      napi_configurable, NULL },
    { "legalCopyright", NULL,
      NULL, getter_legal_copyright, NULL, NULL,
      napi_configurable, NULL },
    { "legalTrademarks", NULL,
      NULL, getter_legal_trademarks, NULL, NULL,
      napi_configurable, NULL },
    { "originalFilename", NULL,
      NULL, getter_original_filename, NULL, NULL,
      napi_configurable, NULL },
    { "privateBuild", NULL,
      NULL, getter_private_build, NULL, NULL,
      napi_configurable, NULL },
    { "productName", NULL,
      NULL, getter_product_name, NULL, NULL,
      napi_configurable, NULL },
    { "productVersion", NULL,
      NULL, getter_product_version, NULL, NULL,
      napi_configurable, NULL },
    { "specialBuild", NULL,
      NULL, getter_special_build, NULL, NULL,
      napi_configurable, NULL },

    { "fileMajorPart", NULL,
      NULL, getter_file_major_part, NULL, NULL,
      napi_configurable, NULL },
    { "fileMinorPart", NULL,
      NULL, getter_file_minor_part, NULL, NULL,
      napi_configurable, NULL },
    { "fileBuildPart", NULL,
      NULL, getter_file_build_part, NULL, NULL,
      napi_configurable, NULL },
    { "filePrivatePart", NULL,
      NULL, getter_file_private_part, NULL, NULL,
      napi_configurable, NULL },
    { "productMajorPart", NULL,
      NULL, getter_product_major_part, NULL, NULL,
      napi_configurable, NULL },
    { "productMinorPart", NULL,
      NULL, getter_product_minor_part, NULL, NULL,
      napi_configurable, NULL },
    { "productBuildPart", NULL,
      NULL, getter_product_build_part, NULL, NULL,
      napi_configurable, NULL },
    { "productPrivatePart", NULL,
      NULL, getter_product_private_part, NULL, NULL,
      napi_configurable, NULL },

    { "isDebug", NULL,
      NULL, getter_is_debug, NULL, NULL,
      napi_configurable, NULL },
    { "isPreRelease", NULL,
      NULL, getter_is_pre_release, NULL, NULL,
      napi_configurable, NULL },
    { "isPatched", NULL,
      NULL, getter_is_patched, NULL, NULL,
      napi_configurable, NULL },
    { "isPrivateBuild", NULL,
      NULL, getter_is_private_build, NULL, NULL,
      napi_configurable, NULL },
    { "isSpecialBuild", NULL,
      NULL, getter_is_special_build, NULL, NULL,
      napi_configurable, NULL },

    { "fileName", NULL,
      NULL, getter_file_name, NULL, NULL,
      napi_configurable, NULL },
    { "language", NULL,
      NULL, getter_language, NULL, NULL,
      napi_configurable, NULL }
  };
  napi_value constructor;
  size_t property_count = sizeof(prototype) / sizeof(napi_property_descriptor);
  NAPI_CALL(env, napi_define_class(env,
                                   "FileVersionInfo",
                                   NAPI_AUTO_LENGTH,
                                   js_fvi_constructor,
                                   NULL,
                                   property_count,
                                   prototype,
                                   &constructor));
  napi_value js_get_version_info_static;
  NAPI_CALL(env, napi_create_function(env,
                                      "getVersionInfo",
                                      NAPI_AUTO_LENGTH,
                                      get_version_info_static,
                                      NULL,
                                      &js_get_version_info_static));

  NAPI_CALL(env, napi_set_named_property(env,
                                         constructor,
                                         "getVersionInfo",
                                         js_get_version_info_static));

  napi_ref cons_ref;
  NAPI_CALL(env, napi_create_reference(env, constructor, 1, &cons_ref));
  NAPI_CALL(env, napi_set_instance_data(env,
    cons_ref, finalize_instance, NULL));

  NAPI_CALL(env, napi_set_named_property(env,
                                         exports,
                                         "FileVersionInfo",
                                         constructor));

  return exports;
}
