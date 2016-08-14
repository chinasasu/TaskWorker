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
	size_t const size = StringPrint(&buffer[0],
		buffer.size() + 1,
		format,
		args ...);
	if (size > buffer.size())
	{
		buffer.resize(size);
		StringPrint(&buffer[0], buffer.size() + 1, format, args ...);
	}
	else if (size < buffer.size())
	{
		buffer.resize(size);
	}
}

template <typename ... Args>
std::string StringPrintf(const char* format, Args const &... args)
{
	std::string result;
	Format(result, format, args...);
	return result;
}


#endif /* __STRING_HELPER_H__ */