/* -------------------------------------------------------------------------
//	filename :	notification_registrar.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 16:05
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATION_REGISTRAR_H__
#define __NOTIFICATION_REGISTRAR_H__

#include <vector>

class NotificationObserver;
class NotificationSource;

class NotificationRegistrar
{
public:
	NotificationRegistrar();
	~NotificationRegistrar();

	void Add(NotificationObserver* observer,
			 int type,
			 const NotificationSource& source);

	void Remove(NotificationObserver* observer,
				int type,
				const NotificationSource& source);

	// Unregisters all notifications.
	void RemoveAll();

	// Returns true if no notifications are registered.
	bool IsEmpty() const;

	// Returns true if there is already a registered notification with the
	// specified details.
	bool IsRegistered(NotificationObserver* observer,
		int type,
		const NotificationSource& source);

private:
	struct Record;

	// We keep registered notifications in a simple vector. This means we'll do
	// brute-force searches when removing them individually, but individual
	// removal is uncommon, and there will typically only be a couple of
	// notifications anyway.
	typedef std::vector<Record> RecordVector;

	// Lists all notifications we're currently registered for.
	RecordVector registered_;
};


#endif /* __NOTIFICATION_REGISTRAR_H__ */


