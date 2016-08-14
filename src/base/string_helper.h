/* -------------------------------------------------------------------------
//     filename :	string_helper.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-3-17 20:35
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __STRING_HELPER_H__
#define __STRING_HELPER_H__
#include <stdio.h>
#include <string>
#include <wchar.h>
#include <assert.h>

// https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
template <typename T>
T Argument(T value) noexcept
{
	return value;
}

template <typename T>
T const * Argument(std::basic_string<T> const & value) noexcept
{
	return value.c_str();
}

template <typename ... Args>
void Print(char const * const format, Args const & ... args) noexcept
{
	printf(format, Argument(args) ...);
}

template <typename ... Args>
int StringPrint(char * const buffer,
	size_t const bufferCount,
	char const * const format,
	Args const & ... args) noexcept
{
	int const result = snprintf(buffer,
		bufferCount,
		format,
		Argument(args) ...);
	//assert(-1 != result);
	return result;
}

template <typename ... Args>
int StringPrint(wchar_t * const buffer,
	size_t const bufferCount,
	wchar_t const * const format,
	Args const & ... args) noexcept
{
	// http://www.cplusplus.com/reference/cwchar/swprintf/?kw=swprintf
	// swprintf与snprintf的返回值含义不同，当buffer不足时，swprintf可能返回-1,snprintf返回所需要的buffer长度
	// buffer为空,则返回所需要的buffer长度(TODO 目前行为如此，不确定是否为编译器确定行为)
	int const result = swprintf(buffer,
		bufferCount,
		format,
		Argument(args) ...);
	//assert(-1 != result);
	return result;
}

template <typename T, typename ... Args>
void Format(std::basic_string<T> & buffer,
	T const * const format,
	Args const & ... args)
{
	const int size = StringPrint(nullptr,
		0,
		format,
		args ...);
	if (size > 0)
	{
		buffer.resize(size);
		StringPrint(&buffer[0], buffer.size() + 1, format, args ...);
	}
}

template <typename ... Args>
std::string StringPrintf(const char* format, Args const &... args)
{
	std::string result;
	Format(result, format, args...);
	return result;
}

template <typename ... Args>
std::wstring StringPrintf(const wchar_t* format, Args const &... args)
{
	std::wstring result;
	Format(result, format, args...);
	return result;
}

#endif /* __STRING_HELPER_H__ */