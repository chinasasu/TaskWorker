// Copyright (c) 2016 Tencent Inc. All rights reserved.
// Author : cjzhou@tencent.com
// Created: 2016-12-8 10:06

#ifndef __TRACKER_H__
#define __TRACKER_H__

class TrackedLocation
{
public:
	TrackedLocation(const char* functionName,
		const char* fileName,
		int line,
		const void* programCounter);

	TrackedLocation();

private:
	const char* function_name_;
	const char* file_name_;
	int line_;
	const void* program_counter_;
};

// Define a macro to record the current source location.
#define FROM_HERE FROM_HERE_WITH_EXPLICIT_FUNCTION(__FUNCTION__)

#define FROM_HERE_WITH_EXPLICIT_FUNCTION(function_name)            \
    TrackedLocation(function_name,                                 \
                                __FILE__,                          \
                                __LINE__,                          \
                                0)

#endif /* __TRACKER_H__ */