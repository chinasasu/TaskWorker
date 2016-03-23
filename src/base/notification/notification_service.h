/* -------------------------------------------------------------------------
//	filename :	notification_service.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 16:22
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATION_SERVICE_H__
#define __NOTIFICATION_SERVICE_H__

#include "notification/notification_details.h"
#include "notification/notification_source.h"

class NotificationService
{
public:

	static NotificationService* current();
	static NotificationService* create();

	virtual ~NotificationService() {}

	// Synchronously posts a notification to all interested observers.
	// Source is a reference to a NotificationSource object representing
	// the object originating the notification (can be
	// NotificationService::AllSources(), in which case
	// only observers interested in all sources will be notified).
	// Details is a reference to an object containing additional data about
	// the notification.  If no additional data is needed, NoDetails() is used.
	// There is no particular order in which the observers will be notified.
	virtual void Notify(int type,
		const NotificationSource& source,
		const NotificationDetails& details) = 0;

	// Returns a NotificationSource that represents all notification sources
	// (for the purpose of registering an observer for events from all sources).
	static Source<void> AllSources() { return Source<void>(NULL); }

	// Returns the same value as AllSources(). This function has semantic
	// differences to the programmer: We have checked that this AllSources()
	// usage is safe in the face of multiple profiles. Objects that were
	// singletons now will always have multiple instances, one per browser
	// context.
	//
	// Some usage is safe, where the Source is checked to see if it's a member of
	// a container before use. But, we want the number of AllSources() calls to
	// drop to almost nothing, because most usages are not multiprofile safe and
	// were done because it was easier to listen to everything.
	static Source<void> AllBrowserContextsAndSources() {
		return Source<void>(NULL);
	}

	// Returns a NotificationDetails object that represents a lack of details
	// associated with a notification.  (This is effectively a null pointer.)
	static Details<void> NoDetails() { return Details<void>(NULL); }

protected:
private:
};


#endif /* __NOTIFICATION_SERVICE_H__ */


