#include "callback/bind_callback.h"

CallbackBase::CallbackBase(BindStorageBase* bindStorage)
	:bindStorage_(bindStorage),
	invokefunc_(NULL)
{

}

CallbackBase::~CallbackBase()
{

}

bool CallbackBase::Equals(const CallbackBase& other) const
{
	return bindStorage_.get() == other.bindStorage_.get() &&
		invokefunc_ == other.invokefunc_;
}

bool CallbackBase::IsNull() const
{
	return bindStorage_.get() == NULL;
}

void CallbackBase::Reset()
{
	invokefunc_ = NULL;
	bindStorage_ = NULL;
}
