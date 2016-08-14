/* -------------------------------------------------------------------------
//	filename :	observer_list.h
//	author	 :	chuanjie
//	created	 :	2014-7-24 16:49
//	purpose	 :
//
// -----------------------------------------------------------------------*/

#ifndef __OBSERVER_LIST_H__
#define __OBSERVER_LIST_H__

#include <memory>
#include <limits>
#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
//
// OVERVIEW:
//
//   A container for a list of observers.  Unlike a normal STL vector or list,
//   this container can be modified during iteration without invalidating the
//   iterator.  So, it safely handles the case of an observer removing itself
//   or other observers from the list while observers are being notified.
//
// TYPICAL USAGE:
//
//   class MyWidget {
//    public:
//     ...
//
//     class Observer {
//      public:
//       virtual void OnFoo(MyWidget* w) = 0;
//       virtual void OnBar(MyWidget* w, int x, int y) = 0;
//     };
//
//     void AddObserver(Observer* obs) {
//       observer_list_.AddObserver(obs);
//     }
//
//     void RemoveObserver(Observer* obs) {
//       observer_list_.RemoveObserver(obs);
//     }
//
//     void NotifyFoo() {
//       FOR_EACH_OBSERVER(Observer, observer_list_, OnFoo(this));
//     }
//
//     void NotifyBar(int x, int y) {
//       FOR_EACH_OBSERVER(Observer, observer_list_, OnBar(this, x, y));
//     }
//
//    private:
//     ObserverList<Observer> observer_list_;
//   };
//
//
///////////////////////////////////////////////////////////////////////////////

template <typename ObserverType>
class ObserverListThreadSafe;

template <class ObserverType>
class ObserverListBase
	: public std::shared_ptr<ObserverListBase<ObserverType> > {
public:
	// Enumeration of which observers are notified.
	enum NotificationType {
		// Specifies that any observers added during notification are notified.
		// This is the default type if non type is provided to the constructor.
		NOTIFY_ALL,

		// Specifies that observers added while sending out notification are not
		// notified.
		NOTIFY_EXISTING_ONLY
	};

	// An iterator class that can be used to access the list of observers.  See
	// also the FOR_EACH_OBSERVER macro defined below.
	class Iterator {
	public:
		Iterator(ObserverListBase<ObserverType>& list)
			: list_(list.As()),
			index_(0),
			max_index_(list.type_ == NOTIFY_ALL ?
			(std::numeric_limits<size_t>::max)() :
		list.observers_.size()) {
			++(list_.lock()->notify_depth_);
		}

		~Iterator() {
			if (!list_.expired() && --list_.lock()->notify_depth_ == 0)
				list_.lock()->Compact();
		}

		ObserverType* GetNext() {
			if (list_.expired())
				return NULL;
			ListType& observers = list_.lock()->observers_;
			// Advance if the current element is null
			size_t max_index = std::min(max_index_, observers.size());
			while (index_ < max_index && !observers[index_])
				++index_;
			return index_ < max_index ? observers[index_++] : NULL;
		}

	private:
		std::weak_ptr<ObserverListBase<ObserverType> > list_;
		size_t index_;
		size_t max_index_;
	};

	ObserverListBase() : notify_depth_(0), type_(NOTIFY_ALL) {}
	explicit ObserverListBase(NotificationType type)
		: notify_depth_(0), type_(type) {}

	// Add an observer to the list.  An observer should not be added to
	// the same list more than once.
	void AddObserver(ObserverType* obs) {
		if (std::find(observers_.begin(), observers_.end(), obs)
			!= observers_.end()) {
				return;
		}
		observers_.push_back(obs);
	}

	// Remove an observer from the list if it is in the list.
	void RemoveObserver(ObserverType* obs) {
		typename ListType::iterator it =
			std::find(observers_.begin(), observers_.end(), obs);
		if (it != observers_.end()) {
			if (notify_depth_) {
				*it = 0;
			} else {
				observers_.erase(it);
			}
		}
	}

	bool HasObserver(ObserverType* observer) const {
		for (size_t i = 0; i < observers_.size(); ++i) {
			if (observers_[i] == observer)
				return true;
		}
		return false;
	}

	void Clear() {
		if (notify_depth_) {
			for (typename ListType::iterator it = observers_.begin();
				it != observers_.end(); ++it) {
					*it = 0;
			}
		} else {
			observers_.clear();
		}
	}

	std::weak_ptr<ObserverListBase<ObserverType>> As()
	{
		if (!owner_.get())
			owner_ = std::shared_ptr<ObserverListBase<ObserverType>>(this);

		return std::weak_ptr<ObserverListBase<ObserverType> >(owner_);
	}

protected:
	size_t size() const { return observers_.size(); }

	void Compact() {
		observers_.erase(
			std::remove(observers_.begin(), observers_.end(),
			static_cast<ObserverType*>(NULL)), observers_.end());
	}

private:
	friend class ObserverListThreadSafe<ObserverType>;

	typedef std::vector<ObserverType*> ListType;

	ListType observers_;
	int notify_depth_;
	NotificationType type_;

	std::shared_ptr<ObserverListBase<ObserverType> > owner_;

	friend class ObserverListBase::Iterator;
};

template <class ObserverType, bool check_empty = false>
class ObserverList : public ObserverListBase<ObserverType> {
public:
	typedef typename ObserverListBase<ObserverType>::NotificationType
		NotificationType;

	ObserverList() {}
	explicit ObserverList(NotificationType type)
		: ObserverListBase<ObserverType>(type) {}

	~ObserverList() {
		// When check_empty is true, assert that the list is empty on destruction.
		if (check_empty) {
			ObserverListBase<ObserverType>::Compact();
		}
	}

	bool might_have_observers() const {
		return ObserverListBase<ObserverType>::size() != 0;
	}
};

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func)               \
	do {                                                                     \
	if ((observer_list).might_have_observers()) {                          \
	ObserverListBase<ObserverType>::Iterator                             \
	it_inside_observer_macro(observer_list);                         \
	ObserverType* obs;                                                   \
	while ((obs = it_inside_observer_macro.GetNext()) != NULL)           \
	obs->func;                                                         \
	}                                                                      \
	} while (0)
#endif /* __OBSERVER_LIST_H__ */


