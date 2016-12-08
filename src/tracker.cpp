#include "tracker.h"

TrackedLocation::TrackedLocation(const char* functionName,
	const char* fileName,
	int line,
	const void* programCounter)
	: function_name_(functionName),
	file_name_(fileName),
	line_(line),
	program_counter_(programCounter)
{

}

TrackedLocation::TrackedLocation()
	:function_name_("Unknown"),
	file_name_("Unknown"),
	line_(-1),
	program_counter_(0)
{

}