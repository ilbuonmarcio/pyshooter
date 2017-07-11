// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 1 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 1; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 1 * sizeof(PyObject *) );
            memcpy( python_pars + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 1 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 1 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 1; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 1 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 1, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 1 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 1 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 1, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 1 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 2 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 2; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 2 * sizeof(PyObject *) );
            memcpy( python_pars + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 2 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 2 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 2; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 2 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 2, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 2 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 2 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 2, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 2 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 3 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 3; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 3 * sizeof(PyObject *) );
            memcpy( python_pars + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 3 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 3 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 3; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 3 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 3, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 3 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 3 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 3, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 3 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 4; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 4 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 4; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 4 * sizeof(PyObject *) );
            memcpy( python_pars + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 4 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 4 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 4; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 4 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 4, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 4 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 4 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 4, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 4 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 5; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 5 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 5; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 5 * sizeof(PyObject *) );
            memcpy( python_pars + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 5 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 5 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 5; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 5 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 5, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 5 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 5 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 5, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 5 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS6( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 6; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 6 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 6; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 6 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 6 * sizeof(PyObject *) );
            memcpy( python_pars + 6, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 6 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 6 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 6; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 6 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 6 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 6, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 6 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 6 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (6 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 6 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (6 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 6 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 6, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            6
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 6 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS7( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 7; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 7 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 7; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 7 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 7 * sizeof(PyObject *) );
            memcpy( python_pars + 7, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 7 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 7 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 7; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 7 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 7 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 7, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 7 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 7 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (7 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 7 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (7 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 7 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 7, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            7
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 7 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS8( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 8; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 8 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 8; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 8 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 8 * sizeof(PyObject *) );
            memcpy( python_pars + 8, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 8 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 8 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 8; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 8 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 8 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 8, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 8 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 8 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (8 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 8 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (8 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 8 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 8, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            8
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 8 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS9( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 9; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 9 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 9; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 9 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 9 * sizeof(PyObject *) );
            memcpy( python_pars + 9, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 9 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 9 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 9; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 9 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 9 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 9, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 9 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 9 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (9 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 9 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (9 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 9 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 9, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            9
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 9 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS14( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 14; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 14 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 14; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 14 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 14 * sizeof(PyObject *) );
            memcpy( python_pars + 14, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 14 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 14 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 14; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 14 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 14 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 14, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 14 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 14 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (14 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 14 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (14 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 14 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 14, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            14
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 14 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS15( PyObject *called, PyObject **args )
{
    CHECK_OBJECT( called );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 15; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    if ( Nuitka_Function_Check( called ) )
    {
        if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
        {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if ( function->m_args_simple && 15 == function->m_args_positional_count )
        {
            for( Py_ssize_t i = 0; i < 15; i++ )
            {
                Py_INCREF( args[ i ] );
            }

            result = function->m_c_code( function, args );
        }
        else if ( function->m_args_simple && 15 + function->m_defaults_given == function->m_args_positional_count )
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
            PyObject *python_pars[ function->m_args_positional_count ];
#endif
            memcpy( python_pars, args, 15 * sizeof(PyObject *) );
            memcpy( python_pars + 15, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

            for( Py_ssize_t i = 0; i < function->m_args_positional_count; i++ )
            {
                Py_INCREF( python_pars[ i ] );
            }

            result = function->m_c_code( function, python_pars );
        }
        else
        {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
            PyObject *python_pars[ function->m_args_overall_count ];
#endif
            memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

            if ( parseArgumentsPos( function, python_pars, args, 15 ))
            {
                result = function->m_c_code( function, python_pars );
            }
            else
            {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    }
    else if ( Nuitka_Method_Check( called ) )
    {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if ( method->m_object != NULL )
        {
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if ( function->m_args_simple && 15 + 1 == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                for( Py_ssize_t i = 0; i < 15; i++ )
                {
                    python_pars[ i + 1 ] = args[ i ];
                    Py_INCREF( args[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else if ( function->m_args_simple && 15 + 1 + function->m_defaults_given == function->m_args_positional_count )
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_positional_count );
#else
                PyObject *python_pars[ function->m_args_positional_count ];
#endif
                python_pars[ 0 ] = method->m_object;
                Py_INCREF( method->m_object );

                memcpy( python_pars+1, args, 15 * sizeof(PyObject *) );
                memcpy( python_pars+1 + 15, &PyTuple_GET_ITEM( function->m_defaults, 0 ), function->m_defaults_given * sizeof(PyObject *) );

                for( Py_ssize_t i = 1; i < function->m_args_overall_count; i++ )
                {
                    Py_INCREF( python_pars[ i ] );
                }

                result = function->m_c_code( function, python_pars );
            }
            else
            {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca( sizeof( PyObject * ) * function->m_args_overall_count );
#else
                PyObject *python_pars[ function->m_args_overall_count ];
#endif
                memset( python_pars, 0, function->m_args_overall_count * sizeof(PyObject *) );

                if ( parseArgumentsMethodPos( function, python_pars, method->m_object, args, 15 ) )
                {
                    result = function->m_c_code( function, python_pars );
                }
                else
                {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    }
    else if ( PyCFunction_Check( called ) )
    {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS( called ) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if ( flags & METH_NOARGS )
        {
#if 15 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (15 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else if ( flags & METH_O )
        {
#if 15 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (15 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        }
        else
        {
            PyCFunction method = PyCFunction_GET_FUNCTION( called );
            PyObject *self = PyCFunction_GET_SELF( called );

            PyObject *pos_args = MAKE_TUPLE( args, 15 );

            PyObject *result;

            assert( flags && METH_VARARGS );

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely( Py_EnterRecursiveCall( (char *)" while calling a Python object" ) ))
            {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if ( flags && METH_KEYWORDS )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#else
            if ( flags == ( METH_VARARGS | METH_KEYWORDS ) )
            {
                result = (*(PyCFunctionWithKeywords)method)( self, pos_args, NULL );
            }
            else if ( flags == METH_FASTCALL )
            {
                result = (*(_PyCFunctionFast)method)( self, &PyTuple_GET_ITEM( pos_args, 0 ), 15, NULL );;
            }
            else
            {
                result = (*method)( self, pos_args );
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if ( result != NULL )
            {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF( pos_args );
                return result;
            }
            else
            {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely( !ERROR_OCCURRED() ))
                {
                    PyErr_Format(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF( pos_args );
                return NULL;
            }
        }
    }
    else if ( PyFunction_Check( called ) )
    {
        return callPythonFunction(
            called,
            args,
            15
        );
    }

    PyObject *pos_args = MAKE_TUPLE( args, 15 );

    PyObject *result = CALL_FUNCTION(
        called,
        pos_args,
        NULL
    );

    Py_DECREF( pos_args );

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 1; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );
            return CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS1( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            }
            else
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1( method, args );
                Py_DECREF( method );
                return result;
            }
        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS2( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 2; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    2
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );
            return CALL_FUNCTION_WITH_ARGS2(
                descr,
                args
            );
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS2( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    2
                );
            }
            else
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS2( method, args );
                Py_DECREF( method );
                return result;
            }
        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS3( PyObject *source, PyObject *attr_name, PyObject **args )
{
    CHECK_OBJECT( source );
    CHECK_OBJECT( attr_name );

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for( size_t i = 0; i < 3; i++ )
    {
        CHECK_OBJECT( args[ i ] );
    }
#endif

    PyTypeObject *type = Py_TYPE( source );

    if ( type->tp_getattro == PyObject_GenericGetAttr )
    {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely( type->tp_dict == NULL ))
        {
            if (unlikely( PyType_Ready( type ) < 0 ))
            {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup( type, attr_name );
        descrgetfunc func = NULL;

        if ( descr != NULL )
        {
            Py_INCREF( descr );

#if PYTHON_VERSION < 300
            if ( PyType_HasFeature( Py_TYPE( descr ), Py_TPFLAGS_HAVE_CLASS ) )
            {
#endif
                func = Py_TYPE( descr )->tp_descr_get;

                if ( func != NULL && PyDescr_IsData( descr ) )
                {
                    PyObject *called_object = func( descr, source, (PyObject *)type );
                    Py_DECREF( descr );

                    PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                        called_object,
                        args
                    );
                    Py_DECREF( called_object );
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if ( dict != NULL )
        {
            CHECK_OBJECT( dict );

            Py_INCREF( dict );

            PyObject *called_object = PyDict_GetItem( dict, attr_name );

            if ( called_object != NULL )
            {
                Py_INCREF( called_object );
                Py_XDECREF( descr );
                Py_DECREF( dict );

                PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                    called_object,
                    args
                );
                Py_DECREF( called_object );
                return result;
            }

            Py_DECREF( dict );
        }

        if ( func != NULL )
        {
            if ( func == Nuitka_Function_Type.tp_descr_get )
            {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    3
                );

                Py_DECREF( descr );

                return result;
            }
            else
            {
                PyObject *called_object = func( descr, source, (PyObject *)type );
                CHECK_OBJECT( called_object );

                Py_DECREF( descr );

                PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                    called_object,
                    args
                );
                Py_DECREF( called_object );

                return result;
            }
        }

        if ( descr != NULL )
        {
            CHECK_OBJECT( descr );
            return CALL_FUNCTION_WITH_ARGS3(
                descr,
                args
            );
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if ( type == &PyInstance_Type )
    {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert( attr_name != const_str_plain___dict__ );
        assert( attr_name != const_str_plain___class__ );

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            return CALL_FUNCTION_WITH_ARGS3( called_object, args );
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if ( called_object != NULL )
        {
            descrgetfunc descr_get = Py_TYPE( called_object )->tp_descr_get;

            if ( descr_get == Nuitka_Function_Type.tp_descr_get )
            {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    3
                );
            }
            else
            {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely( method == NULL ))
                {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS3( method, args );
                Py_DECREF( method );
                return result;
            }
        }
        else if (unlikely( source_instance->in_class->cl_getattr == NULL ))
        {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        }
        else
        {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely( called_object == NULL ))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS3(
                called_object,
                args
            );
            Py_DECREF( called_object );
            return result;
        }
    }
#endif
    else if ( type->tp_getattro != NULL )
    {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS3(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else if ( type->tp_getattr != NULL )
    {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        if (unlikely( called_object == NULL ))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS3(
            called_object,
            args
        );
        Py_DECREF( called_object );
        return result;
    }
    else
    {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked( attr_name )
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
MOD_INIT_DECL( __main__ );
MOD_INIT_DECL( appdirs );
MOD_INIT_DECL( numpy );
MOD_INIT_DECL( numpy$__config__ );
MOD_INIT_DECL( numpy$_distributor_init );
MOD_INIT_DECL( numpy$_globals );
MOD_INIT_DECL( numpy$_import_tools );
MOD_INIT_DECL( numpy$add_newdocs );
MOD_INIT_DECL( numpy$compat );
MOD_INIT_DECL( numpy$compat$_inspect );
MOD_INIT_DECL( numpy$compat$py3k );
MOD_INIT_DECL( numpy$core );
MOD_INIT_DECL( numpy$core$_internal );
MOD_INIT_DECL( numpy$core$_methods );
MOD_INIT_DECL( numpy$core$arrayprint );
MOD_INIT_DECL( numpy$core$defchararray );
MOD_INIT_DECL( numpy$core$einsumfunc );
MOD_INIT_DECL( numpy$core$fromnumeric );
MOD_INIT_DECL( numpy$core$function_base );
MOD_INIT_DECL( numpy$core$getlimits );
MOD_INIT_DECL( numpy$core$info );
MOD_INIT_DECL( numpy$core$machar );
MOD_INIT_DECL( numpy$core$memmap );
MOD_INIT_DECL( numpy$core$numeric );
MOD_INIT_DECL( numpy$core$numerictypes );
MOD_INIT_DECL( numpy$core$records );
MOD_INIT_DECL( numpy$core$shape_base );
MOD_INIT_DECL( numpy$ctypeslib );
MOD_INIT_DECL( numpy$dual );
MOD_INIT_DECL( numpy$fft );
MOD_INIT_DECL( numpy$fft$fftpack );
MOD_INIT_DECL( numpy$fft$helper );
MOD_INIT_DECL( numpy$fft$info );
MOD_INIT_DECL( numpy$lib );
MOD_INIT_DECL( numpy$lib$_datasource );
MOD_INIT_DECL( numpy$lib$_iotools );
MOD_INIT_DECL( numpy$lib$_version );
MOD_INIT_DECL( numpy$lib$arraypad );
MOD_INIT_DECL( numpy$lib$arraysetops );
MOD_INIT_DECL( numpy$lib$arrayterator );
MOD_INIT_DECL( numpy$lib$financial );
MOD_INIT_DECL( numpy$lib$format );
MOD_INIT_DECL( numpy$lib$function_base );
MOD_INIT_DECL( numpy$lib$index_tricks );
MOD_INIT_DECL( numpy$lib$info );
MOD_INIT_DECL( numpy$lib$mixins );
MOD_INIT_DECL( numpy$lib$nanfunctions );
MOD_INIT_DECL( numpy$lib$npyio );
MOD_INIT_DECL( numpy$lib$polynomial );
MOD_INIT_DECL( numpy$lib$scimath );
MOD_INIT_DECL( numpy$lib$shape_base );
MOD_INIT_DECL( numpy$lib$stride_tricks );
MOD_INIT_DECL( numpy$lib$twodim_base );
MOD_INIT_DECL( numpy$lib$type_check );
MOD_INIT_DECL( numpy$lib$ufunclike );
MOD_INIT_DECL( numpy$lib$utils );
MOD_INIT_DECL( numpy$linalg );
MOD_INIT_DECL( numpy$linalg$info );
MOD_INIT_DECL( numpy$linalg$linalg );
MOD_INIT_DECL( numpy$ma );
MOD_INIT_DECL( numpy$ma$core );
MOD_INIT_DECL( numpy$ma$extras );
MOD_INIT_DECL( numpy$ma$mrecords );
MOD_INIT_DECL( numpy$matrixlib );
MOD_INIT_DECL( numpy$matrixlib$defmatrix );
MOD_INIT_DECL( numpy$polynomial );
MOD_INIT_DECL( numpy$polynomial$_polybase );
MOD_INIT_DECL( numpy$polynomial$chebyshev );
MOD_INIT_DECL( numpy$polynomial$hermite );
MOD_INIT_DECL( numpy$polynomial$hermite_e );
MOD_INIT_DECL( numpy$polynomial$laguerre );
MOD_INIT_DECL( numpy$polynomial$legendre );
MOD_INIT_DECL( numpy$polynomial$polynomial );
MOD_INIT_DECL( numpy$polynomial$polyutils );
MOD_INIT_DECL( numpy$random );
MOD_INIT_DECL( numpy$random$info );
MOD_INIT_DECL( numpy$version );
MOD_INIT_DECL( packaging );
MOD_INIT_DECL( packaging$__about__ );
MOD_INIT_DECL( pygame );
MOD_INIT_DECL( pygame$_numpysndarray );
MOD_INIT_DECL( pygame$_numpysurfarray );
MOD_INIT_DECL( pygame$colordict );
MOD_INIT_DECL( pygame$compat );
MOD_INIT_DECL( pygame$cursors );
MOD_INIT_DECL( pygame$ftfont );
MOD_INIT_DECL( pygame$locals );
MOD_INIT_DECL( pygame$macosx );
MOD_INIT_DECL( pygame$pkgdata );
MOD_INIT_DECL( pygame$sndarray );
MOD_INIT_DECL( pygame$sprite );
MOD_INIT_DECL( pygame$surfarray );
MOD_INIT_DECL( pygame$sysfont );
MOD_INIT_DECL( pygame$threads );
MOD_INIT_DECL( pygame$version );
MOD_INIT_DECL( six );
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] =
{
    { (char *)"__main__", MOD_INIT_NAME( __main__ ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"_asyncio", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_bisect", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_blake2", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_cn", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_hk", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_iso2022", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_jp", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_kr", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_codecs_tw", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_crypt", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_csv", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_curses_panel", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_datetime", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_dbm", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_gdbm", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_heapq", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_json", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lsprof", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_lzma", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multibytecodec", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_opcode", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_pickle", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_posixsubprocess", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_random", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_sha3", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_socket", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"_struct", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"appdirs", MOD_INIT_NAME( appdirs ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"array", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"audioop", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"binascii", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"fcntl", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"grp", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"math", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"mmap", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy", MOD_INIT_NAME( numpy ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.__config__", MOD_INIT_NAME( numpy$__config__ ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy._distributor_init", MOD_INIT_NAME( numpy$_distributor_init ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy._globals", MOD_INIT_NAME( numpy$_globals ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy._import_tools", MOD_INIT_NAME( numpy$_import_tools ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.add_newdocs", MOD_INIT_NAME( numpy$add_newdocs ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.compat", MOD_INIT_NAME( numpy$compat ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.compat._inspect", MOD_INIT_NAME( numpy$compat$_inspect ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.compat.py3k", MOD_INIT_NAME( numpy$compat$py3k ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core", MOD_INIT_NAME( numpy$core ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.core._internal", MOD_INIT_NAME( numpy$core$_internal ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core._methods", MOD_INIT_NAME( numpy$core$_methods ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.arrayprint", MOD_INIT_NAME( numpy$core$arrayprint ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.defchararray", MOD_INIT_NAME( numpy$core$defchararray ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.einsumfunc", MOD_INIT_NAME( numpy$core$einsumfunc ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.fromnumeric", MOD_INIT_NAME( numpy$core$fromnumeric ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.function_base", MOD_INIT_NAME( numpy$core$function_base ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.getlimits", MOD_INIT_NAME( numpy$core$getlimits ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.info", MOD_INIT_NAME( numpy$core$info ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.machar", MOD_INIT_NAME( numpy$core$machar ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.memmap", MOD_INIT_NAME( numpy$core$memmap ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.multiarray", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.core.numeric", MOD_INIT_NAME( numpy$core$numeric ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.numerictypes", MOD_INIT_NAME( numpy$core$numerictypes ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.records", MOD_INIT_NAME( numpy$core$records ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.shape_base", MOD_INIT_NAME( numpy$core$shape_base ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.core.umath", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.ctypeslib", MOD_INIT_NAME( numpy$ctypeslib ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.distutils", NULL, 1646024, 581, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.distutils.__config__", NULL, 1646605, 1063, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.__version__", NULL, 1647668, 332, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.ccompiler", NULL, 1648000, 20086, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command", NULL, 1668086, 1094, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.distutils.command.autodist", NULL, 1669180, 2215, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.bdist_rpm", NULL, 1671395, 899, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build", NULL, 1672294, 1896, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build_clib", NULL, 1674190, 6924, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build_ext", NULL, 1681114, 11010, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build_py", NULL, 1692124, 1415, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build_scripts", NULL, 1693539, 1726, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.build_src", NULL, 1695265, 18548, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.config", NULL, 1713813, 12233, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.config_compiler", NULL, 1726046, 4228, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.develop", NULL, 1730274, 922, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.egg_info", NULL, 1731196, 1152, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.install", NULL, 1732348, 2121, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.install_clib", NULL, 1734469, 1669, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.install_data", NULL, 1736138, 966, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.install_headers", NULL, 1737104, 1020, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.command.sdist", NULL, 1738124, 1010, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.compat", NULL, 1739134, 467, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.conv_template", NULL, 1739601, 8154, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.core", NULL, 1747755, 4760, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.cpuinfo", NULL, 1752515, 33624, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.environment", NULL, 1786139, 2699, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.exec_command", NULL, 1788838, 7934, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.extension", NULL, 1796772, 2093, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.fcompiler", NULL, 1798865, 27268, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.distutils.from_template", NULL, 1826133, 6973, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.info", NULL, 1833106, 323, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.lib2def", NULL, 1833429, 3293, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.log", NULL, 1836722, 2457, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.mingw32ccompiler", NULL, 1839179, 14372, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.misc_util", NULL, 1853551, 66845, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.npy_pkg_config", NULL, 1920396, 12494, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.numpy_distribution", NULL, 1932890, 837, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.system_info", NULL, 1933727, 67428, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.distutils.unixccompiler", NULL, 2001155, 3368, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.dual", MOD_INIT_NAME( numpy$dual ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.f2py", NULL, 2004523, 2054, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.f2py.__version__", NULL, 2006577, 403, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.auxfuncs", NULL, 2006980, 22302, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.capi_maps", NULL, 2029282, 17989, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.cb_rules", NULL, 2047271, 14788, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.cfuncs", NULL, 2062059, 35990, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.common_rules", NULL, 2098049, 4706, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.crackfortran", NULL, 2102755, 75976, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.diagnose", NULL, 2178731, 3759, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.f2py2e", NULL, 2182490, 19287, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.f2py_testing", NULL, 2201777, 1468, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.f90mod_rules", NULL, 2203245, 7388, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.func2subr", NULL, 2210633, 6757, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.rules", NULL, 2217390, 34214, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.f2py.use_rules", NULL, 2251604, 3103, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.fft", MOD_INIT_NAME( numpy$fft ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.fft.fftpack", MOD_INIT_NAME( numpy$fft$fftpack ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.fft.fftpack_lite", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.fft.helper", MOD_INIT_NAME( numpy$fft$helper ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.fft.info", MOD_INIT_NAME( numpy$fft$info ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib", MOD_INIT_NAME( numpy$lib ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.lib._datasource", MOD_INIT_NAME( numpy$lib$_datasource ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib._iotools", MOD_INIT_NAME( numpy$lib$_iotools ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib._version", MOD_INIT_NAME( numpy$lib$_version ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.arraypad", MOD_INIT_NAME( numpy$lib$arraypad ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.arraysetops", MOD_INIT_NAME( numpy$lib$arraysetops ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.arrayterator", MOD_INIT_NAME( numpy$lib$arrayterator ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.financial", MOD_INIT_NAME( numpy$lib$financial ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.format", MOD_INIT_NAME( numpy$lib$format ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.function_base", MOD_INIT_NAME( numpy$lib$function_base ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.index_tricks", MOD_INIT_NAME( numpy$lib$index_tricks ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.info", MOD_INIT_NAME( numpy$lib$info ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.mixins", MOD_INIT_NAME( numpy$lib$mixins ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.nanfunctions", MOD_INIT_NAME( numpy$lib$nanfunctions ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.npyio", MOD_INIT_NAME( numpy$lib$npyio ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.polynomial", MOD_INIT_NAME( numpy$lib$polynomial ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.scimath", MOD_INIT_NAME( numpy$lib$scimath ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.shape_base", MOD_INIT_NAME( numpy$lib$shape_base ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.stride_tricks", MOD_INIT_NAME( numpy$lib$stride_tricks ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.twodim_base", MOD_INIT_NAME( numpy$lib$twodim_base ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.type_check", MOD_INIT_NAME( numpy$lib$type_check ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.ufunclike", MOD_INIT_NAME( numpy$lib$ufunclike ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.lib.utils", MOD_INIT_NAME( numpy$lib$utils ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.linalg", MOD_INIT_NAME( numpy$linalg ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.linalg._umath_linalg", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.linalg.info", MOD_INIT_NAME( numpy$linalg$info ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.linalg.lapack_lite", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.linalg.linalg", MOD_INIT_NAME( numpy$linalg$linalg ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.ma", MOD_INIT_NAME( numpy$ma ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.ma.core", MOD_INIT_NAME( numpy$ma$core ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.ma.extras", MOD_INIT_NAME( numpy$ma$extras ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.ma.mrecords", MOD_INIT_NAME( numpy$ma$mrecords ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.matrixlib", MOD_INIT_NAME( numpy$matrixlib ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.matrixlib.defmatrix", MOD_INIT_NAME( numpy$matrixlib$defmatrix ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial", MOD_INIT_NAME( numpy$polynomial ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.polynomial._polybase", MOD_INIT_NAME( numpy$polynomial$_polybase ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.chebyshev", MOD_INIT_NAME( numpy$polynomial$chebyshev ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.hermite", MOD_INIT_NAME( numpy$polynomial$hermite ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.hermite_e", MOD_INIT_NAME( numpy$polynomial$hermite_e ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.laguerre", MOD_INIT_NAME( numpy$polynomial$laguerre ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.legendre", MOD_INIT_NAME( numpy$polynomial$legendre ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.polynomial", MOD_INIT_NAME( numpy$polynomial$polynomial ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.polynomial.polyutils", MOD_INIT_NAME( numpy$polynomial$polyutils ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.random", MOD_INIT_NAME( numpy$random ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.random.info", MOD_INIT_NAME( numpy$random$info ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"numpy.random.mtrand", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"numpy.testing", NULL, 2254707, 673, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"numpy.testing.decorators", NULL, 2255380, 8301, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.testing.noseclasses", NULL, 2263681, 8981, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.testing.nosetester", NULL, 2272662, 14524, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.testing.utils", NULL, 2287186, 63338, NUITKA_BYTECODE_FLAG },
    { (char *)"numpy.version", MOD_INIT_NAME( numpy$version ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"packaging", MOD_INIT_NAME( packaging ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"packaging.__about__", MOD_INIT_NAME( packaging$__about__ ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pkg_resources", NULL, 2350524, 96029, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pkg_resources.extern", NULL, 2446553, 2368, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame", MOD_INIT_NAME( pygame ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"pygame._freetype", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame._numpysndarray", MOD_INIT_NAME( pygame$_numpysndarray ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame._numpysurfarray", MOD_INIT_NAME( pygame$_numpysurfarray ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.base", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.bufferproxy", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.cdrom", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.color", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.colordict", MOD_INIT_NAME( pygame$colordict ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.compat", MOD_INIT_NAME( pygame$compat ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.constants", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.cursors", MOD_INIT_NAME( pygame$cursors ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.display", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.draw", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.event", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.fastevent", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.font", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.ftfont", MOD_INIT_NAME( pygame$ftfont ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.image", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.imageext", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.joystick", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.key", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.locals", MOD_INIT_NAME( pygame$locals ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.macosx", MOD_INIT_NAME( pygame$macosx ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.mask", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.math", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.mixer", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.mixer_music", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.mouse", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.overlay", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.pixelarray", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.pixelcopy", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.pkgdata", MOD_INIT_NAME( pygame$pkgdata ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.rect", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.rwobject", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.scrap", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.sndarray", MOD_INIT_NAME( pygame$sndarray ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.sprite", MOD_INIT_NAME( pygame$sprite ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.surface", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.surfarray", MOD_INIT_NAME( pygame$surfarray ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.surflock", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.sysfont", MOD_INIT_NAME( pygame$sysfont ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"pygame.threads", MOD_INIT_NAME( pygame$threads ), 0, 0, NUITKA_PACKAGE_FLAG },
    { (char *)"pygame.time", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.transform", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"pygame.version", MOD_INIT_NAME( pygame$version ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"readline", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"select", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"setuptools", NULL, 2448921, 5617, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"setuptools.archive_util", NULL, 2454538, 5102, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command", NULL, 2459640, 670, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"setuptools.command.bdist_egg", NULL, 2460310, 13608, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.bdist_rpm", NULL, 2473918, 1721, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.develop", NULL, 2475639, 6369, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.easy_install", NULL, 2482008, 64185, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.egg_info", NULL, 2546193, 20951, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.install", NULL, 2567144, 3918, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.install_scripts", NULL, 2571062, 2223, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.py36compat", NULL, 2573285, 4568, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.sdist", NULL, 2577853, 6193, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.command.setopt", NULL, 2584046, 4547, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.config", NULL, 2588593, 13650, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.depends", NULL, 2602243, 5225, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.dist", NULL, 2607468, 32671, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.extension", NULL, 2640139, 1919, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.extern", NULL, 2642058, 277, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"setuptools.glob", NULL, 2642335, 3787, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.monkey", NULL, 2646122, 5125, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.namespaces", NULL, 2651247, 3622, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.package_index", NULL, 2654869, 32419, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.py26compat", NULL, 2687288, 1007, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.py27compat", NULL, 2688295, 755, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.py31compat", NULL, 2689050, 1837, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.py33compat", NULL, 2690887, 1172, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.py36compat", NULL, 2692059, 2151, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.sandbox", NULL, 2694210, 15823, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.ssl_support", NULL, 2710033, 6619, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.unicode_utils", NULL, 2716652, 1113, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.version", NULL, 2717765, 272, NUITKA_BYTECODE_FLAG },
    { (char *)"setuptools.windows_support", NULL, 2718037, 955, NUITKA_BYTECODE_FLAG },
    { (char *)"six", MOD_INIT_NAME( six ), 0, 0, NUITKA_COMPILED_MODULE },
    { (char *)"termios", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"unicodedata", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"zlib", NULL, 0, 0, NUITKA_SHLIB_FLAG },
    { (char *)"__future__", NULL, 2718992, 4155, NUITKA_BYTECODE_FLAG },
    { (char *)"_dummy_thread", NULL, 2723147, 4839, NUITKA_BYTECODE_FLAG },
    { (char *)"_markupbase", NULL, 2727986, 7979, NUITKA_BYTECODE_FLAG },
    { (char *)"_osx_support", NULL, 2735965, 9663, NUITKA_BYTECODE_FLAG },
    { (char *)"_pyio", NULL, 2745628, 71352, NUITKA_BYTECODE_FLAG },
    { (char *)"_sitebuiltins", NULL, 2816980, 3423, NUITKA_BYTECODE_FLAG },
    { (char *)"_strptime", NULL, 2820403, 15951, NUITKA_BYTECODE_FLAG },
    { (char *)"_sysconfigdata_m_linux_x86_64-linux-gnu", NULL, 2836354, 18069, NUITKA_BYTECODE_FLAG },
    { (char *)"_threading_local", NULL, 2854423, 6609, NUITKA_BYTECODE_FLAG },
    { (char *)"aifc", NULL, 2861032, 25923, NUITKA_BYTECODE_FLAG },
    { (char *)"argparse", NULL, 2886955, 60162, NUITKA_BYTECODE_FLAG },
    { (char *)"asynchat", NULL, 2947117, 6803, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio", NULL, 2953920, 766, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"asyncio.base_events", NULL, 2954686, 38473, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.base_futures", NULL, 2993159, 2035, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.base_subprocess", NULL, 2995194, 9250, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.base_tasks", NULL, 3004444, 1849, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.compat", NULL, 3006293, 720, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.constants", NULL, 3007013, 209, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.coroutines", NULL, 3007222, 8341, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.events", NULL, 3015563, 24891, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.futures", NULL, 3040454, 13746, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.locks", NULL, 3054200, 15105, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.log", NULL, 3069305, 206, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.proactor_events", NULL, 3069511, 16568, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.protocols", NULL, 3086079, 5965, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.queues", NULL, 3092044, 8333, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.selector_events", NULL, 3100377, 29809, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.sslproto", NULL, 3130186, 20263, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.streams", NULL, 3150449, 19586, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.subprocess", NULL, 3170035, 6812, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.tasks", NULL, 3176847, 18932, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.test_utils", NULL, 3195779, 16825, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.transports", NULL, 3212604, 12048, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncio.unix_events", NULL, 3224652, 30334, NUITKA_BYTECODE_FLAG },
    { (char *)"asyncore", NULL, 3254986, 15815, NUITKA_BYTECODE_FLAG },
    { (char *)"bdb", NULL, 3270801, 16924, NUITKA_BYTECODE_FLAG },
    { (char *)"binhex", NULL, 3287725, 12074, NUITKA_BYTECODE_FLAG },
    { (char *)"bisect", NULL, 3299799, 2664, NUITKA_BYTECODE_FLAG },
    { (char *)"cProfile", NULL, 3302463, 4199, NUITKA_BYTECODE_FLAG },
    { (char *)"calendar", NULL, 3306662, 25870, NUITKA_BYTECODE_FLAG },
    { (char *)"cgi", NULL, 3332532, 27802, NUITKA_BYTECODE_FLAG },
    { (char *)"cgitb", NULL, 3360334, 10086, NUITKA_BYTECODE_FLAG },
    { (char *)"chunk", NULL, 3370420, 4888, NUITKA_BYTECODE_FLAG },
    { (char *)"cmd", NULL, 3375308, 12563, NUITKA_BYTECODE_FLAG },
    { (char *)"code", NULL, 3387871, 9824, NUITKA_BYTECODE_FLAG },
    { (char *)"codeop", NULL, 3397695, 6258, NUITKA_BYTECODE_FLAG },
    { (char *)"colorsys", NULL, 3403953, 3299, NUITKA_BYTECODE_FLAG },
    { (char *)"compileall", NULL, 3407252, 8246, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent", NULL, 3415498, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"concurrent.futures", NULL, 3415610, 628, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"concurrent.futures._base", NULL, 3416238, 19840, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent.futures.process", NULL, 3436078, 15457, NUITKA_BYTECODE_FLAG },
    { (char *)"concurrent.futures.thread", NULL, 3451535, 3785, NUITKA_BYTECODE_FLAG },
    { (char *)"configparser", NULL, 3455320, 45214, NUITKA_BYTECODE_FLAG },
    { (char *)"contextlib", NULL, 3500534, 11406, NUITKA_BYTECODE_FLAG },
    { (char *)"copy", NULL, 3511940, 7067, NUITKA_BYTECODE_FLAG },
    { (char *)"crypt", NULL, 3519007, 2230, NUITKA_BYTECODE_FLAG },
    { (char *)"csv", NULL, 3521237, 11844, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes", NULL, 3533081, 16048, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes._endian", NULL, 3549129, 1912, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.macholib", NULL, 3551041, 279, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"ctypes.macholib.dyld", NULL, 3551320, 4319, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.macholib.dylib", NULL, 3555639, 1906, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.macholib.framework", NULL, 3557545, 2186, NUITKA_BYTECODE_FLAG },
    { (char *)"ctypes.util", NULL, 3559731, 7214, NUITKA_BYTECODE_FLAG },
    { (char *)"curses", NULL, 3566945, 1771, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"curses.ascii", NULL, 3568716, 3909, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.has_key", NULL, 3572625, 4233, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.panel", NULL, 3576858, 203, NUITKA_BYTECODE_FLAG },
    { (char *)"curses.textpad", NULL, 3577061, 5896, NUITKA_BYTECODE_FLAG },
    { (char *)"datetime", NULL, 3582957, 53722, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm", NULL, 3636679, 4157, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"dbm.dumb", NULL, 3640836, 8201, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm.gnu", NULL, 3649037, 183, NUITKA_BYTECODE_FLAG },
    { (char *)"dbm.ndbm", NULL, 3649220, 182, NUITKA_BYTECODE_FLAG },
    { (char *)"decimal", NULL, 3649402, 163390, NUITKA_BYTECODE_FLAG },
    { (char *)"difflib", NULL, 3812792, 59508, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils", NULL, 3872300, 364, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.archive_util", NULL, 3872664, 6359, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.bcppcompiler", NULL, 3879023, 6467, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.ccompiler", NULL, 3885490, 33333, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cmd", NULL, 3918823, 14974, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command", NULL, 3933797, 521, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"distutils.command.bdist", NULL, 3934318, 3767, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_dumb", NULL, 3938085, 3639, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_rpm", NULL, 3941724, 13055, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.bdist_wininst", NULL, 3954779, 8193, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build", NULL, 3962972, 3959, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_clib", NULL, 3966931, 4957, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_ext", NULL, 3971888, 16429, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_py", NULL, 3988317, 10450, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.build_scripts", NULL, 3998767, 4320, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.check", NULL, 4003087, 4865, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.clean", NULL, 4007952, 2165, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.config", NULL, 4010117, 10300, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install", NULL, 4020417, 13764, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_data", NULL, 4034181, 2310, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_egg_info", NULL, 4036491, 2971, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_headers", NULL, 4039462, 1710, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_lib", NULL, 4041172, 5162, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.install_scripts", NULL, 4046334, 2176, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.register", NULL, 4048510, 8398, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.sdist", NULL, 4056908, 12961, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.command.upload", NULL, 4069869, 5058, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.config", NULL, 4074927, 3473, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.core", NULL, 4078400, 6789, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.cygwinccompiler", NULL, 4085189, 8494, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.debug", NULL, 4093683, 174, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dep_util", NULL, 4093857, 2690, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dir_util", NULL, 4096547, 5790, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.dist", NULL, 4102337, 34126, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.errors", NULL, 4136463, 5460, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.extension", NULL, 4141923, 6936, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.fancy_getopt", NULL, 4148859, 10639, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.file_util", NULL, 4159498, 5883, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.filelist", NULL, 4165381, 9851, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.log", NULL, 4175232, 2247, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.msvccompiler", NULL, 4177479, 14554, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.spawn", NULL, 4192033, 4959, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.sysconfig", NULL, 4196992, 11915, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.text_file", NULL, 4208907, 8444, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.unixccompiler", NULL, 4217351, 6516, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.util", NULL, 4223867, 15519, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.version", NULL, 4239386, 7340, NUITKA_BYTECODE_FLAG },
    { (char *)"distutils.versionpredicate", NULL, 4246726, 5069, NUITKA_BYTECODE_FLAG },
    { (char *)"doctest", NULL, 4251795, 75577, NUITKA_BYTECODE_FLAG },
    { (char *)"dummy_threading", NULL, 4327372, 1090, NUITKA_BYTECODE_FLAG },
    { (char *)"email", NULL, 4328462, 1657, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email._encoded_words", NULL, 4330119, 5632, NUITKA_BYTECODE_FLAG },
    { (char *)"email._header_value_parser", NULL, 4335751, 79766, NUITKA_BYTECODE_FLAG },
    { (char *)"email._parseaddr", NULL, 4415517, 12428, NUITKA_BYTECODE_FLAG },
    { (char *)"email._policybase", NULL, 4427945, 14784, NUITKA_BYTECODE_FLAG },
    { (char *)"email.base64mime", NULL, 4442729, 3206, NUITKA_BYTECODE_FLAG },
    { (char *)"email.charset", NULL, 4445935, 11499, NUITKA_BYTECODE_FLAG },
    { (char *)"email.contentmanager", NULL, 4457434, 7276, NUITKA_BYTECODE_FLAG },
    { (char *)"email.encoders", NULL, 4464710, 1628, NUITKA_BYTECODE_FLAG },
    { (char *)"email.errors", NULL, 4466338, 5945, NUITKA_BYTECODE_FLAG },
    { (char *)"email.feedparser", NULL, 4472283, 10632, NUITKA_BYTECODE_FLAG },
    { (char *)"email.generator", NULL, 4482915, 12501, NUITKA_BYTECODE_FLAG },
    { (char *)"email.header", NULL, 4495416, 16468, NUITKA_BYTECODE_FLAG },
    { (char *)"email.headerregistry", NULL, 4511884, 21235, NUITKA_BYTECODE_FLAG },
    { (char *)"email.iterators", NULL, 4533119, 1898, NUITKA_BYTECODE_FLAG },
    { (char *)"email.message", NULL, 4535017, 37980, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime", NULL, 4572997, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"email.mime.application", NULL, 4573109, 1423, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.audio", NULL, 4574532, 2582, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.base", NULL, 4577114, 1048, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.image", NULL, 4578162, 1868, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.message", NULL, 4580030, 1297, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.multipart", NULL, 4581327, 1519, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.nonmultipart", NULL, 4582846, 734, NUITKA_BYTECODE_FLAG },
    { (char *)"email.mime.text", NULL, 4583580, 1281, NUITKA_BYTECODE_FLAG },
    { (char *)"email.parser", NULL, 4584861, 5713, NUITKA_BYTECODE_FLAG },
    { (char *)"email.policy", NULL, 4590574, 9606, NUITKA_BYTECODE_FLAG },
    { (char *)"email.quoprimime", NULL, 4600180, 7668, NUITKA_BYTECODE_FLAG },
    { (char *)"email.utils", NULL, 4607848, 9663, NUITKA_BYTECODE_FLAG },
    { (char *)"filecmp", NULL, 4617511, 8293, NUITKA_BYTECODE_FLAG },
    { (char *)"fileinput", NULL, 4625804, 13140, NUITKA_BYTECODE_FLAG },
    { (char *)"fnmatch", NULL, 4638944, 2862, NUITKA_BYTECODE_FLAG },
    { (char *)"formatter", NULL, 4641806, 17567, NUITKA_BYTECODE_FLAG },
    { (char *)"fractions", NULL, 4659373, 18414, NUITKA_BYTECODE_FLAG },
    { (char *)"ftplib", NULL, 4677787, 28091, NUITKA_BYTECODE_FLAG },
    { (char *)"getopt", NULL, 4705878, 6205, NUITKA_BYTECODE_FLAG },
    { (char *)"getpass", NULL, 4712083, 4165, NUITKA_BYTECODE_FLAG },
    { (char *)"gettext", NULL, 4716248, 13990, NUITKA_BYTECODE_FLAG },
    { (char *)"glob", NULL, 4730238, 4247, NUITKA_BYTECODE_FLAG },
    { (char *)"gzip", NULL, 4734485, 16214, NUITKA_BYTECODE_FLAG },
    { (char *)"hashlib", NULL, 4750699, 6624, NUITKA_BYTECODE_FLAG },
    { (char *)"hmac", NULL, 4757323, 4824, NUITKA_BYTECODE_FLAG },
    { (char *)"html", NULL, 4762147, 3365, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"html.entities", NULL, 4765512, 50435, NUITKA_BYTECODE_FLAG },
    { (char *)"html.parser", NULL, 4815947, 11152, NUITKA_BYTECODE_FLAG },
    { (char *)"http", NULL, 4827099, 6515, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"http.client", NULL, 4833614, 34327, NUITKA_BYTECODE_FLAG },
    { (char *)"http.cookiejar", NULL, 4867941, 53893, NUITKA_BYTECODE_FLAG },
    { (char *)"http.cookies", NULL, 4921834, 16076, NUITKA_BYTECODE_FLAG },
    { (char *)"http.server", NULL, 4937910, 32206, NUITKA_BYTECODE_FLAG },
    { (char *)"imaplib", NULL, 4970116, 42044, NUITKA_BYTECODE_FLAG },
    { (char *)"imghdr", NULL, 5012160, 4138, NUITKA_BYTECODE_FLAG },
    { (char *)"imp", NULL, 5016298, 9667, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib.abc", NULL, 5025965, 11281, NUITKA_BYTECODE_FLAG },
    { (char *)"importlib.util", NULL, 5037246, 8888, NUITKA_BYTECODE_FLAG },
    { (char *)"ipaddress", NULL, 5046134, 62613, NUITKA_BYTECODE_FLAG },
    { (char *)"json", NULL, 5108747, 12627, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"json.decoder", NULL, 5121374, 9943, NUITKA_BYTECODE_FLAG },
    { (char *)"json.encoder", NULL, 5131317, 11245, NUITKA_BYTECODE_FLAG },
    { (char *)"json.scanner", NULL, 5142562, 1958, NUITKA_BYTECODE_FLAG },
    { (char *)"json.tool", NULL, 5144520, 1533, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3", NULL, 5146053, 109, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.btm_matcher", NULL, 5146162, 4920, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.btm_utils", NULL, 5151082, 6119, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_base", NULL, 5157201, 6211, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixer_util", NULL, 5163412, 12024, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes", NULL, 5175436, 115, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.fixes.fix_apply", NULL, 5175551, 1663, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_asserts", NULL, 5177214, 1238, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_basestring", NULL, 5178452, 628, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_buffer", NULL, 5179080, 773, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_dict", NULL, 5179853, 3296, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_except", NULL, 5183149, 2783, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exec", NULL, 5185932, 1114, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_execfile", NULL, 5187046, 1648, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_exitfunc", NULL, 5188694, 2261, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_filter", NULL, 5190955, 1911, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_funcattrs", NULL, 5192866, 939, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_future", NULL, 5193805, 749, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_getcwdu", NULL, 5194554, 753, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_has_key", NULL, 5195307, 2883, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_idioms", NULL, 5198190, 3868, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_import", NULL, 5202058, 2755, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports", NULL, 5204813, 4314, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_imports2", NULL, 5209127, 513, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_input", NULL, 5209640, 915, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_intern", NULL, 5210555, 1127, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_isinstance", NULL, 5211682, 1525, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools", NULL, 5213207, 1519, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_itertools_imports", NULL, 5214726, 1574, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_long", NULL, 5216300, 670, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_map", NULL, 5216970, 2658, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_metaclass", NULL, 5219628, 5341, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_methodattrs", NULL, 5224969, 901, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ne", NULL, 5225870, 772, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_next", NULL, 5226642, 3025, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_nonzero", NULL, 5229667, 888, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_numliterals", NULL, 5230555, 994, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_operator", NULL, 5231549, 4202, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_paren", NULL, 5235751, 1355, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_print", NULL, 5237106, 2295, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raise", NULL, 5239401, 2214, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_raw_input", NULL, 5241615, 760, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reduce", NULL, 5242375, 1095, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_reload", NULL, 5243470, 1127, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_renames", NULL, 5244597, 1958, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_repr", NULL, 5246555, 810, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_set_literal", NULL, 5247365, 1652, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_standarderror", NULL, 5249017, 685, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_sys_exc", NULL, 5249702, 1366, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_throw", NULL, 5251068, 1767, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_tuple_params", NULL, 5252835, 4561, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_types", NULL, 5257396, 1794, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_unicode", NULL, 5259190, 1508, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_urllib", NULL, 5260698, 5953, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_ws_comma", NULL, 5266651, 1088, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xrange", NULL, 5267739, 2503, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_xreadlines", NULL, 5270242, 1082, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.fixes.fix_zip", NULL, 5271324, 1130, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.main", NULL, 5272454, 8512, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.patcomp", NULL, 5280966, 5642, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2", NULL, 5286608, 145, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"lib2to3.pgen2.driver", NULL, 5286753, 4314, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.grammar", NULL, 5291067, 6842, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.literals", NULL, 5297909, 1520, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.parse", NULL, 5299429, 6315, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.pgen", NULL, 5305744, 9771, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.token", NULL, 5315515, 1838, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pgen2.tokenize", NULL, 5317353, 14847, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pygram", NULL, 5332200, 1142, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.pytree", NULL, 5333342, 25122, NUITKA_BYTECODE_FLAG },
    { (char *)"lib2to3.refactor", NULL, 5358464, 20777, NUITKA_BYTECODE_FLAG },
    { (char *)"logging", NULL, 5379241, 59586, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"logging.config", NULL, 5438827, 23206, NUITKA_BYTECODE_FLAG },
    { (char *)"logging.handlers", NULL, 5462033, 42949, NUITKA_BYTECODE_FLAG },
    { (char *)"lzma", NULL, 5504982, 11980, NUITKA_BYTECODE_FLAG },
    { (char *)"macpath", NULL, 5516962, 5629, NUITKA_BYTECODE_FLAG },
    { (char *)"macurl2path", NULL, 5522591, 1855, NUITKA_BYTECODE_FLAG },
    { (char *)"mailbox", NULL, 5524446, 63740, NUITKA_BYTECODE_FLAG },
    { (char *)"mailcap", NULL, 5588186, 6466, NUITKA_BYTECODE_FLAG },
    { (char *)"mimetypes", NULL, 5594652, 15502, NUITKA_BYTECODE_FLAG },
    { (char *)"modulefinder", NULL, 5610154, 15354, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing", NULL, 5625508, 493, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.connection", NULL, 5626001, 24769, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.context", NULL, 5650770, 13084, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.dummy", NULL, 5663854, 3703, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"multiprocessing.dummy.connection", NULL, 5667557, 2483, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.forkserver", NULL, 5670040, 6392, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.heap", NULL, 5676432, 6065, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.managers", NULL, 5682497, 33289, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.pool", NULL, 5715786, 20894, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_fork", NULL, 5736680, 2135, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_forkserver", NULL, 5738815, 2314, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.popen_spawn_posix", NULL, 5741129, 2094, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.process", NULL, 5743223, 8073, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.queues", NULL, 5751296, 9007, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.reduction", NULL, 5760303, 7938, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.resource_sharer", NULL, 5768241, 5142, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.semaphore_tracker", NULL, 5773383, 3426, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.sharedctypes", NULL, 5776809, 6862, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.spawn", NULL, 5783671, 6429, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.synchronize", NULL, 5790100, 11205, NUITKA_BYTECODE_FLAG },
    { (char *)"multiprocessing.util", NULL, 5801305, 9518, NUITKA_BYTECODE_FLAG },
    { (char *)"netrc", NULL, 5810823, 3838, NUITKA_BYTECODE_FLAG },
    { (char *)"nntplib", NULL, 5814661, 33768, NUITKA_BYTECODE_FLAG },
    { (char *)"ntpath", NULL, 5848429, 13653, NUITKA_BYTECODE_FLAG },
    { (char *)"nturl2path", NULL, 5862082, 1487, NUITKA_BYTECODE_FLAG },
    { (char *)"numbers", NULL, 5863569, 12130, NUITKA_BYTECODE_FLAG },
    { (char *)"optparse", NULL, 5875699, 48042, NUITKA_BYTECODE_FLAG },
    { (char *)"pathlib", NULL, 5923741, 40772, NUITKA_BYTECODE_FLAG },
    { (char *)"pdb", NULL, 5964513, 46082, NUITKA_BYTECODE_FLAG },
    { (char *)"pickletools", NULL, 6010595, 66034, NUITKA_BYTECODE_FLAG },
    { (char *)"pipes", NULL, 6076629, 7796, NUITKA_BYTECODE_FLAG },
    { (char *)"pkgutil", NULL, 6084425, 16249, NUITKA_BYTECODE_FLAG },
    { (char *)"platform", NULL, 6100674, 27972, NUITKA_BYTECODE_FLAG },
    { (char *)"plistlib", NULL, 6128646, 27352, NUITKA_BYTECODE_FLAG },
    { (char *)"poplib", NULL, 6155998, 13266, NUITKA_BYTECODE_FLAG },
    { (char *)"pprint", NULL, 6169264, 15812, NUITKA_BYTECODE_FLAG },
    { (char *)"profile", NULL, 6185076, 13889, NUITKA_BYTECODE_FLAG },
    { (char *)"pstats", NULL, 6198965, 21845, NUITKA_BYTECODE_FLAG },
    { (char *)"pty", NULL, 6220810, 3849, NUITKA_BYTECODE_FLAG },
    { (char *)"py_compile", NULL, 6224659, 6532, NUITKA_BYTECODE_FLAG },
    { (char *)"pyclbr", NULL, 6231191, 8353, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc", NULL, 6239544, 83899, NUITKA_BYTECODE_FLAG },
    { (char *)"pydoc_data", NULL, 6323443, 112, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"pydoc_data.topics", NULL, 6323555, 396695, NUITKA_BYTECODE_FLAG },
    { (char *)"queue", NULL, 6720250, 8743, NUITKA_BYTECODE_FLAG },
    { (char *)"random", NULL, 6728993, 19180, NUITKA_BYTECODE_FLAG },
    { (char *)"rlcompleter", NULL, 6748173, 5768, NUITKA_BYTECODE_FLAG },
    { (char *)"runpy", NULL, 6753941, 7921, NUITKA_BYTECODE_FLAG },
    { (char *)"sched", NULL, 6761862, 6552, NUITKA_BYTECODE_FLAG },
    { (char *)"secrets", NULL, 6768414, 2150, NUITKA_BYTECODE_FLAG },
    { (char *)"selectors", NULL, 6770564, 17685, NUITKA_BYTECODE_FLAG },
    { (char *)"shelve", NULL, 6788249, 9459, NUITKA_BYTECODE_FLAG },
    { (char *)"shlex", NULL, 6797708, 6958, NUITKA_BYTECODE_FLAG },
    { (char *)"shutil", NULL, 6804666, 30373, NUITKA_BYTECODE_FLAG },
    { (char *)"signal", NULL, 6835039, 2503, NUITKA_BYTECODE_FLAG },
    { (char *)"site", NULL, 6837542, 12413, NUITKA_BYTECODE_FLAG },
    { (char *)"smtpd", NULL, 6849955, 26671, NUITKA_BYTECODE_FLAG },
    { (char *)"smtplib", NULL, 6876626, 35269, NUITKA_BYTECODE_FLAG },
    { (char *)"sndhdr", NULL, 6911895, 6901, NUITKA_BYTECODE_FLAG },
    { (char *)"socket", NULL, 6918796, 21995, NUITKA_BYTECODE_FLAG },
    { (char *)"socketserver", NULL, 6940791, 23617, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3", NULL, 6964408, 140, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"sqlite3.dbapi2", NULL, 6964548, 2459, NUITKA_BYTECODE_FLAG },
    { (char *)"sqlite3.dump", NULL, 6967007, 1902, NUITKA_BYTECODE_FLAG },
    { (char *)"ssl", NULL, 6968909, 35857, NUITKA_BYTECODE_FLAG },
    { (char *)"statistics", NULL, 7004766, 18162, NUITKA_BYTECODE_FLAG },
    { (char *)"string", NULL, 7022928, 7940, NUITKA_BYTECODE_FLAG },
    { (char *)"subprocess", NULL, 7030868, 35086, NUITKA_BYTECODE_FLAG },
    { (char *)"sunau", NULL, 7065954, 16926, NUITKA_BYTECODE_FLAG },
    { (char *)"symbol", NULL, 7082880, 2505, NUITKA_BYTECODE_FLAG },
    { (char *)"symtable", NULL, 7085385, 10416, NUITKA_BYTECODE_FLAG },
    { (char *)"sysconfig", NULL, 7095801, 15819, NUITKA_BYTECODE_FLAG },
    { (char *)"tabnanny", NULL, 7111620, 6959, NUITKA_BYTECODE_FLAG },
    { (char *)"tarfile", NULL, 7118579, 62573, NUITKA_BYTECODE_FLAG },
    { (char *)"telnetlib", NULL, 7181152, 18085, NUITKA_BYTECODE_FLAG },
    { (char *)"tempfile", NULL, 7199237, 22119, NUITKA_BYTECODE_FLAG },
    { (char *)"textwrap", NULL, 7221356, 13672, NUITKA_BYTECODE_FLAG },
    { (char *)"this", NULL, 7235028, 1253, NUITKA_BYTECODE_FLAG },
    { (char *)"timeit", NULL, 7236281, 11591, NUITKA_BYTECODE_FLAG },
    { (char *)"trace", NULL, 7247872, 19567, NUITKA_BYTECODE_FLAG },
    { (char *)"tracemalloc", NULL, 7267439, 17217, NUITKA_BYTECODE_FLAG },
    { (char *)"tty", NULL, 7284656, 1060, NUITKA_BYTECODE_FLAG },
    { (char *)"typing", NULL, 7285716, 71039, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest", NULL, 7356755, 2977, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"unittest.case", NULL, 7359732, 47559, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.loader", NULL, 7407291, 13854, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.main", NULL, 7421145, 6930, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.mock", NULL, 7428075, 61744, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.result", NULL, 7489819, 7214, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.runner", NULL, 7497033, 6844, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.signals", NULL, 7503877, 2160, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.suite", NULL, 7506037, 9159, NUITKA_BYTECODE_FLAG },
    { (char *)"unittest.util", NULL, 7515196, 4741, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib", NULL, 7519937, 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"urllib.error", NULL, 7520045, 2742, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.parse", NULL, 7522787, 29294, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.request", NULL, 7552081, 72602, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.response", NULL, 7624683, 3215, NUITKA_BYTECODE_FLAG },
    { (char *)"urllib.robotparser", NULL, 7627898, 7022, NUITKA_BYTECODE_FLAG },
    { (char *)"uu", NULL, 7634920, 3486, NUITKA_BYTECODE_FLAG },
    { (char *)"uuid", NULL, 7638406, 20382, NUITKA_BYTECODE_FLAG },
    { (char *)"venv", NULL, 7658788, 13624, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wave", NULL, 7672412, 17873, NUITKA_BYTECODE_FLAG },
    { (char *)"webbrowser", NULL, 7690285, 15836, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref", NULL, 7706121, 704, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"wsgiref.handlers", NULL, 7706825, 16136, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.headers", NULL, 7722961, 7725, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.simple_server", NULL, 7730686, 5181, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.util", NULL, 7735867, 5180, NUITKA_BYTECODE_FLAG },
    { (char *)"wsgiref.validate", NULL, 7741047, 14788, NUITKA_BYTECODE_FLAG },
    { (char *)"xdrlib", NULL, 7755835, 8290, NUITKA_BYTECODE_FLAG },
    { (char *)"xml", NULL, 7764125, 672, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom", NULL, 7764797, 5419, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.dom.NodeFilter", NULL, 7770216, 939, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.domreg", NULL, 7771155, 2747, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.expatbuilder", NULL, 7773902, 27132, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minicompat", NULL, 7801034, 2777, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.minidom", NULL, 7803811, 55834, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.pulldom", NULL, 7859645, 10523, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.dom.xmlbuilder", NULL, 7870168, 13539, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree", NULL, 7883707, 111, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.etree.ElementInclude", NULL, 7883818, 1547, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementPath", NULL, 7885365, 6123, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.ElementTree", NULL, 7891488, 44716, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.etree.cElementTree", NULL, 7936204, 153, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.parsers", NULL, 7936357, 285, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.parsers.expat", NULL, 7936642, 314, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax", NULL, 7936956, 3109, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xml.sax._exceptions", NULL, 7940065, 5453, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.expatreader", NULL, 7945518, 12198, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.handler", NULL, 7957716, 12301, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.saxutils", NULL, 7970017, 12795, NUITKA_BYTECODE_FLAG },
    { (char *)"xml.sax.xmlreader", NULL, 7982812, 16910, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpc", NULL, 7999722, 108, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG },
    { (char *)"xmlrpc.client", NULL, 7999830, 34651, NUITKA_BYTECODE_FLAG },
    { (char *)"xmlrpc.server", NULL, 8034481, 29444, NUITKA_BYTECODE_FLAG },
    { (char *)"zipapp", NULL, 8063925, 5522, NUITKA_BYTECODE_FLAG },
    { (char *)"zipfile", NULL, 8069447, 47988, NUITKA_BYTECODE_FLAG },
    { NULL, NULL, 0, 0, 0 }
};

void setupMetaPathBasedLoader( void )
{
    static bool init_done = false;

    if ( init_done == false )
    {
        registerMetaPathBasedUnfreezer( meta_path_loader_entries );
        init_done = true;
    }
}
