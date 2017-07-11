/* Generated code for Python source for module 'pygame.version'
 * created by Nuitka version 0.5.26
 *
 * This code is in part copyright 2017 Kay Hayen.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nuitka/prelude.h"

#include "__helpers.h"

/* The _module_pygame$version is a Python object pointer of module type. */

/* Note: For full compatibility with CPython, every module variable access
 * needs to go through it except for cases where the module cannot possibly
 * have changed in the mean time.
 */

PyObject *module_pygame$version;
PyDictObject *moduledict_pygame$version;

/* The module constants used, if any. */
static PyObject *const_int_pos_9;
extern PyObject *const_str_plain___file__;
extern PyObject *const_str_digest_749448536aec7c722f6b23d71bbbb4b1;
extern PyObject *const_str_plain___loader__;
static PyObject *const_str_plain_rev;
extern PyObject *const_str_plain___package__;
extern PyObject *const_str_empty;
extern PyObject *const_str_plain_pygame;
extern PyObject *const_str_plain_ver;
static PyObject *const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple;
static PyObject *const_str_digest_b6eca294032dfc1047d6cccb49539897;
static PyObject *const_str_plain_vernum;
extern PyObject *const_int_pos_3;
extern PyObject *const_str_plain___doc__;
extern PyObject *const_str_plain___cached__;
static PyObject *const_str_digest_3ac7935e3c52ff65f7e2890ee4c6cae3;
extern PyObject *const_int_pos_1;
static PyObject *const_str_digest_21a96b5b636c9708d40d610b6975dd03;
static PyObject *module_filename_obj;

static bool constants_created = false;

static void createModuleConstants( void )
{
    const_int_pos_9 = PyLong_FromUnsignedLong( 9ul );
    const_str_plain_rev = UNSTREAM_STRING( &constant_bin[ 16100 ], 3, 1 );
    const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple = PyTuple_New( 3 );
    PyTuple_SET_ITEM( const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple, 0, const_int_pos_1 ); Py_INCREF( const_int_pos_1 );
    PyTuple_SET_ITEM( const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple, 1, const_int_pos_9 ); Py_INCREF( const_int_pos_9 );
    PyTuple_SET_ITEM( const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple, 2, const_int_pos_3 ); Py_INCREF( const_int_pos_3 );
    const_str_digest_b6eca294032dfc1047d6cccb49539897 = UNSTREAM_STRING( &constant_bin[ 1628342 ], 5, 0 );
    const_str_plain_vernum = UNSTREAM_STRING( &constant_bin[ 1634931 ], 6, 1 );
    const_str_digest_3ac7935e3c52ff65f7e2890ee4c6cae3 = UNSTREAM_STRING( &constant_bin[ 1634937 ], 17, 0 );
    const_str_digest_21a96b5b636c9708d40d610b6975dd03 = UNSTREAM_STRING( &constant_bin[ 1634954 ], 400, 0 );

    constants_created = true;
}

#ifndef __NUITKA_NO_ASSERT__
void checkModuleConstants_pygame$version( void )
{
    // The module may not have been used at all.
    if (constants_created == false) return;


}
#endif

// The module code objects.


static void createModuleCodeObjects(void)
{
    module_filename_obj = MAKE_RELATIVE_PATH( const_str_digest_3ac7935e3c52ff65f7e2890ee4c6cae3 );
}

// The module function declarations.


// The module function definitions.



#if PYTHON_VERSION >= 300
static struct PyModuleDef mdef_pygame$version =
{
    PyModuleDef_HEAD_INIT,
    "pygame.version",   /* m_name */
    NULL,                /* m_doc */
    -1,                  /* m_size */
    NULL,                /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
  };
#endif

#if PYTHON_VERSION >= 300
extern PyObject *metapath_based_loader;
#endif

extern void _initCompiledCellType();
extern void _initCompiledGeneratorType();
extern void _initCompiledFunctionType();
extern void _initCompiledMethodType();
extern void _initCompiledFrameType();
#if PYTHON_VERSION >= 350
extern void _initCompiledCoroutineTypes();
#endif
#if PYTHON_VERSION >= 360
extern void _initCompiledAsyncgenTypes();
#endif

// The exported interface to CPython. On import of the module, this function
// gets called. It has to have an exact function name, in cases it's a shared
// library export. This is hidden behind the MOD_INIT_DECL.

MOD_INIT_DECL( pygame$version )
{
#if defined(_NUITKA_EXE) || PYTHON_VERSION >= 300
    static bool _init_done = false;

    // Modules might be imported repeatedly, which is to be ignored.
    if ( _init_done )
    {
        return MOD_RETURN_VALUE( module_pygame$version );
    }
    else
    {
        _init_done = true;
    }
#endif

#ifdef _NUITKA_MODULE
    // In case of a stand alone extension module, need to call initialization
    // the init here because that's the first and only time we are going to get
    // called here.

    // Initialize the constant values used.
    _initBuiltinModule();
    createGlobalConstants();

    /* Initialize the compiled types of Nuitka. */
    _initCompiledCellType();
    _initCompiledGeneratorType();
    _initCompiledFunctionType();
    _initCompiledMethodType();
    _initCompiledFrameType();
#if PYTHON_VERSION >= 350
    _initCompiledCoroutineTypes();
#endif
#if PYTHON_VERSION >= 360
    _initCompiledAsyncgenTypes();
#endif

#if PYTHON_VERSION < 300
    _initSlotCompare();
#endif
#if PYTHON_VERSION >= 270
    _initSlotIternext();
#endif

    patchBuiltinModule();
    patchTypeComparison();

    // Enable meta path based loader if not already done.
    setupMetaPathBasedLoader();

#if PYTHON_VERSION >= 300
    patchInspectModule();
#endif

#endif

    /* The constants only used by this module are created now. */
#ifdef _NUITKA_TRACE
    puts("pygame.version: Calling createModuleConstants().");
#endif
    createModuleConstants();

    /* The code objects used by this module are created now. */
#ifdef _NUITKA_TRACE
    puts("pygame.version: Calling createModuleCodeObjects().");
#endif
    createModuleCodeObjects();

    // puts( "in initpygame$version" );

    // Create the module object first. There are no methods initially, all are
    // added dynamically in actual code only.  Also no "__doc__" is initially
    // set at this time, as it could not contain NUL characters this way, they
    // are instead set in early module code.  No "self" for modules, we have no
    // use for it.
#if PYTHON_VERSION < 300
    module_pygame$version = Py_InitModule4(
        "pygame.version",       // Module Name
        NULL,                    // No methods initially, all are added
                                 // dynamically in actual module code only.
        NULL,                    // No __doc__ is initially set, as it could
                                 // not contain NUL this way, added early in
                                 // actual code.
        NULL,                    // No self for modules, we don't use it.
        PYTHON_API_VERSION
    );
#else
    module_pygame$version = PyModule_Create( &mdef_pygame$version );
#endif

    moduledict_pygame$version = (PyDictObject *)((PyModuleObject *)module_pygame$version)->md_dict;

    CHECK_OBJECT( module_pygame$version );

// Seems to work for Python2.7 out of the box, but for Python3, the module
// doesn't automatically enter "sys.modules", so do it manually.
#if PYTHON_VERSION >= 300
    {
        int r = PyObject_SetItem( PySys_GetObject( (char *)"modules" ), const_str_digest_749448536aec7c722f6b23d71bbbb4b1, module_pygame$version );

        assert( r != -1 );
    }
#endif

    // For deep importing of a module we need to have "__builtins__", so we set
    // it ourselves in the same way than CPython does. Note: This must be done
    // before the frame object is allocated, or else it may fail.

    PyObject *module_dict = PyModule_GetDict( module_pygame$version );

    if ( PyDict_GetItem( module_dict, const_str_plain___builtins__ ) == NULL )
    {
        PyObject *value = (PyObject *)builtin_module;

        // Check if main module, not a dict then.
#if !defined(_NUITKA_EXE) || !0
        value = PyModule_GetDict( value );
#endif

#ifndef __NUITKA_NO_ASSERT__
        int res =
#endif
            PyDict_SetItem( module_dict, const_str_plain___builtins__, value );

        assert( res == 0 );
    }

#if PYTHON_VERSION >= 330
    PyDict_SetItem( module_dict, const_str_plain___loader__, metapath_based_loader );
#endif

    // Temp variables if any
    PyObject *tmp_assign_source_1;
    PyObject *tmp_assign_source_2;
    PyObject *tmp_assign_source_3;
    PyObject *tmp_assign_source_4;
    PyObject *tmp_assign_source_5;
    PyObject *tmp_assign_source_6;
    PyObject *tmp_assign_source_7;

    // Module code.
    tmp_assign_source_1 = const_str_digest_21a96b5b636c9708d40d610b6975dd03;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain___doc__, tmp_assign_source_1 );
    tmp_assign_source_2 = module_filename_obj;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain___file__, tmp_assign_source_2 );
    tmp_assign_source_3 = Py_None;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain___cached__, tmp_assign_source_3 );
    tmp_assign_source_4 = const_str_plain_pygame;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain___package__, tmp_assign_source_4 );
    tmp_assign_source_5 = const_str_digest_b6eca294032dfc1047d6cccb49539897;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain_ver, tmp_assign_source_5 );
    tmp_assign_source_6 = const_tuple_int_pos_1_int_pos_9_int_pos_3_tuple;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain_vernum, tmp_assign_source_6 );
    tmp_assign_source_7 = const_str_empty;
    UPDATE_STRING_DICT0( moduledict_pygame$version, (Nuitka_StringObject *)const_str_plain_rev, tmp_assign_source_7 );

    return MOD_RETURN_VALUE( module_pygame$version );
}
