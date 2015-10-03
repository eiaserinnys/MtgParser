
#pragma once

template <typename Class>
class PointerT {
public:
	PointerT() {}
	PointerT(Class* rhs) { Set(rhs); }
	PointerT(const PointerT& rhs) { Set(const_cast<Class*>(rhs.Get())); }
	~PointerT() { Clear(); }

	PointerT& operator = (Class* rhs)
		{ Set(rhs); return *this; }
	PointerT& operator = (const PointerT& rhs)
		{ Set(const_cast<Class*>(rhs.Get())); return *this; }

	Class* operator * () { return ptr; }
	const Class* operator * () const { return ptr; }

	Class* operator -> () { return ptr; }
	const Class* operator -> () const { return ptr; }

	Class* Get() { return ptr; }
	const Class* Get() const { return ptr; }

	void Set(Class* rhs)
	{
		if (rhs == ptr) { return;  }
		Clear();
		ptr = rhs;
		if (ptr != nullptr) { ptr->AddRef(); }
	}

	void Clear()
	{
		if (ptr != nullptr)
		{
			ptr->RemoveRef();
			if (ptr->GetRefCount() == 0)
			{ 
				delete ptr; 
			}
			ptr = nullptr;
		}
	}

private:
	Class* ptr = nullptr;
};