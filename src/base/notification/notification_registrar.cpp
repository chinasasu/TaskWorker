/* -------------------------------------------------------------------------
//	filename :	notification_registrar.cpp
//	author	 :	chuanjie
//	created	 :	2014-7-24 16:05
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#include "notification/notification_registrar.h"
#include "notification/notification_service_impl.h"

#include <algorithm>

struct NotificationRegistrar::Record 
{
	bool operator==(const Record& other) const;

	NotificationObserver* observer;
	int type;
	NotificationSource source;
};

bool NotificationRegistrar::Record::operator==(const Record& other) const 
{
	return observer == other.observer &&
		type == other.type &&
		source == other.source;
}

NotificationRegistrar::NotificationRegistrar() 
{
	NotificationServiceImpl::current();
}

NotificationRegistrar::~NotificationRegistrar() {
	RemoveAll();
}

void NotificationRegistrar::Add(NotificationObserver* observer,
	int type,
	const NotificationSource& source) 
{
		Record record = { observer, type, source };
		registered_.push_back(record);

		NotificationServiceImpl::current()->AddObserver(observer, type, source);
}

void NotificationRegistrar::Remove(NotificationObserver* observer,
	int type,
	const NotificationSource& source) 
{
		Record record = { observer, type, source };
		RecordVector::iterator found = std::find(
			registered_.begin(), registered_.end(), record);

		registered_.erase(found);

		// This can be NULL if our owner outlives the NotificationService, e.g. if our
		// owner is a Singleton.
		NotificationServiceImpl* service = NotificationServiceImpl::current();
		if (service)
			service->RemoveObserver(observer, type, source);
}

void NotificationRegistrar::RemoveAll() {
	// Early-exit if no registrations, to avoid calling
	// NotificationService::current.  If we've constructed an object with a
	// NotificationRegistrar member, but haven't actually used the notification
	// service, and we reach prgram exit, then calling current() below could try
	// to initialize the service's lazy TLS pointer during exit, which throws
	// wrenches at things.
	if (registered_.empty())
		return;

	// This can be NULL if our owner outlives the NotificationService, e.g. if our
	// owner is a Singleton.
	NotificationServiceImpl* service = NotificationServiceImpl::current();
	if (service) {
		for (size_t i = 0; i < registered_.size(); i++) {
			service->RemoveObserver(registered_[i].observer,
				registered_[i].type,
				registered_[i].source);
		}
	}
	registered_.clear();
}

bool NotificationRegistrar::IsEmpty() const {
	return registered_.empty();
}

bool NotificationRegistrar::IsRegistered(NotificationObserver* observer,
	int type,
	const NotificationSource& source) 
{
		Record record = { observer, type, source };
		return std::find(registered_.begin(), registered_.end(), record) !=
			registered_.end();
}