
/* -------------------------------------------------------------------------
//	filename :	notification_details.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 15:41
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATION_DETAILS_H__
#define __NOTIFICATION_DETAILS_H__

#include "basictypes.h"


class NotificationDetails
{
public:
	NotificationDetails():ptr_(NULL) {}
	NotificationDetails(const NotificationDetails& other) : ptr_(other.ptr_) {}
	~NotificationDetails() {}

	uintptr_t map_key() const { return reinterpret_cast<uintptr_t>(ptr_); }

	bool operator!=(const NotificationDetails& other) const {
		return ptr_ != other.ptr_;
	}

	bool operator==(const NotificationDetails& other) const {
		return ptr_ == other.ptr_;
	}

protected:
	explicit NotificationDetails(const void* ptr) : ptr_(ptr) {}

	const void* ptr_;

};


template <class T>
class Details : public NotificationDetails
{
public:
	// TODO(erg): Our code hard relies on implicit conversion
	Details(T* ptr) : NotificationDetails(ptr) {}  // NOLINT
	Details(const NotificationDetails& other)      // NOLINT
		: NotificationDetails(other) {}

	T* operator->() const { return ptr(); }
	// The casts here allow this to compile with both T = Foo and T = const Foo.
	T* ptr() const { return static_cast<T*>(const_cast<void*>(ptr_)); }
};


#endif /* __NOTIFICATION_DETAILS_H__ */


