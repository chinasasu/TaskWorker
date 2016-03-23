/* -------------------------------------------------------------------------
//	filename :	notification_observer.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 15:59
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __NOTIFICATION_OBSERVER_H__
#define __NOTIFICATION_OBSERVER_H__

class NotificationDetails;
class NotificationSource;

class NotificationObserver
{
public:
	virtual void Observe(int type, 
					const NotificationSource& source,
					const NotificationDetails& details) = 0;
protected:
	virtual ~NotificationObserver() {}
};


#endif /* __NOTIFICATION_OBSERVER_H__ */


