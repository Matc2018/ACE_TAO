
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl NULL_INTERFACE_STUB
// ------------------------------
#ifndef NULL_INTERFACE_STUB_EXPORT_H
#define NULL_INTERFACE_STUB_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (NULL_INTERFACE_STUB_HAS_DLL)
#  define NULL_INTERFACE_STUB_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && NULL_INTERFACE_STUB_HAS_DLL */

#if !defined (NULL_INTERFACE_STUB_HAS_DLL)
#  define NULL_INTERFACE_STUB_HAS_DLL 1
#endif /* ! NULL_INTERFACE_STUB_HAS_DLL */

#if defined (NULL_INTERFACE_STUB_HAS_DLL) && (NULL_INTERFACE_STUB_HAS_DLL == 1)
#  if defined (NULL_INTERFACE_STUB_BUILD_DLL)
#    define NULL_INTERFACE_STUB_Export ACE_Proper_Export_Flag
#    define NULL_INTERFACE_STUB_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define NULL_INTERFACE_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* NULL_INTERFACE_STUB_BUILD_DLL */
#    define NULL_INTERFACE_STUB_Export ACE_Proper_Import_Flag
#    define NULL_INTERFACE_STUB_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define NULL_INTERFACE_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* NULL_INTERFACE_STUB_BUILD_DLL */
#else /* NULL_INTERFACE_STUB_HAS_DLL == 1 */
#  define NULL_INTERFACE_STUB_Export
#  define NULL_INTERFACE_STUB_SINGLETON_DECLARATION(T)
#  define NULL_INTERFACE_STUB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* NULL_INTERFACE_STUB_HAS_DLL == 1 */

// Set NULL_INTERFACE_STUB_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (NULL_INTERFACE_STUB_NTRACE)
#  if (ACE_NTRACE == 1)
#    define NULL_INTERFACE_STUB_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define NULL_INTERFACE_STUB_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !NULL_INTERFACE_STUB_NTRACE */

#if (NULL_INTERFACE_STUB_NTRACE == 1)
#  define NULL_INTERFACE_STUB_TRACE(X)
#else /* (NULL_INTERFACE_STUB_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define NULL_INTERFACE_STUB_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (NULL_INTERFACE_STUB_NTRACE == 1) */

#endif /* NULL_INTERFACE_STUB_EXPORT_H */

// End of auto generated file.
