
#pragma once

//#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// Windows 2000
#ifndef _WIN32_NET_WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
// we set _SECURE_SCL for all pre vc10 compilers as all older versions did
// with vc10 we depend on the crt to do the right thing ( if(_DEBUG)_SECURE_SCL=1; else _SECURE_SCL=0; )
#if _MSC_VER < 1600
#define _SECURE_SCL 1
#endif

// No warnings for checked iterators
#define _SCL_SECURE_NO_WARNINGS

#ifndef _CRT_SECURE_NO_DEPRECATE
// No warnings deprecated CRT functions
#define _CRT_SECURE_NO_DEPRECATE
#endif

// Enable to enable library profiling
//#define PROFILING

#if _MSC_VER < 1300 
	#define POINTER_64
	#define NO_DSHOW_STRSAFE
	#define _STRSAFE_H_INCLUDED_
#endif
