/* -------------------------------------------------------------------------
//	filename :	notification_source.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 16:00
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATION_SOURCE_H__
#define __NOTIFICATION_SOURCE_H__

class NotificationSource
{
public:
	NotificationSource(const NotificationSource& other) : ptr_(other.ptr_) {}
	~NotificationSource() {}

	// NotificationSource can be used as the index for a map; this method
	// returns the pointer to the current source as an identifier, for use as a
	// map index.
	uintptr_t map_key() const { return reinterpret_cast<uintptr_t>(ptr_); }

	bool operator!=(const NotificationSource& other) const {
		return ptr_ != other.ptr_;
	}
	bool operator==(const NotificationSource& other) const {
		return ptr_ == other.ptr_;
	}

protected:
	explicit NotificationSource(const void* ptr) : ptr_(ptr) {}

	// Declaring this const allows Source<T> to be used with both T = Foo and
	// T = const Foo.
	const void* ptr_;
};

template <class T>
class Source : public NotificationSource {
public:
	// TODO(erg): Our code hard relies on implicit conversion
	Source(const T* ptr) : NotificationSource(ptr) {}  // NOLINT
	Source(const NotificationSource& other)      // NOLINT
		: NotificationSource(other) {}

	T* operator->() const { return ptr(); }
	// The casts here allow this to compile with both T = Foo and T = const Foo.
	T* ptr() const { return static_cast<T*>(const_cast<void*>(ptr_)); }
};


#endif /* __NOTIFICATION_SOURCE_H__ */


