#ifndef ag_pointer_c
#define ag_pointer_c

namespace Ag
{
	Pointer::ptr_base::ptr_base()
	{ }
	Pointer::ptr_base::~ptr_base()
	{ }
	template <class T>
	Pointer::ptr<T>::ptr() :
		data(0)
	{ }
	template <class T>
	Pointer::ptr<T>::ptr(T d) :
		data(d)
	{ }
	template <class T>
	Pointer::ptr<T>::ptr(const ptr<T> &p) :
		data(p.data)
	{ }
	template <class T>
	Pointer::ptr<T>::~ptr()
	{ }
	template <class T>
	Pointer::ptr_base *Pointer::ptr<T>::copy()
	{
		return new ptr<T>(data);
	}
	template <class T>
	Pointer::ptr<T> &Pointer::ptr<T>::operator=(const Pointer::ptr<T> &p)
	{
		data = p.data;
		return *this;
	}
	
	Pointer::Pointer() :
		p(0)
	{ }
	template <class T>
	Pointer::Pointer(const T d) :
		p(new ptr<T>(d))
	{ }
	Pointer::Pointer(const Pointer &d) :
		p(d.p ? d.p->copy() : 0)
	{ }
	Pointer::~Pointer()
	{
		delete p;
	}
	template <class T>
	T Pointer::cast() const
	{
		if (p)
			return ((Pointer::ptr<T> *)p)->data;

		return 0;
	}
	Pointer::operator bool() const
	{
		return p;
	}
	const bool Pointer::operator!() const
	{
		return !p;
	}
	template <class T>
	Pointer &Pointer::operator=(const T d)
	{
		if (p)
			delete p;

		p = new ptr<T>(d);

		return *this;
	}
	Pointer &Pointer::operator=(const Pointer &d)
	{
		if (p)
			delete p;

		if (d.p)
			p = d.p->copy();
		else
			p = 0;

		return *this;
	}
}

#endif /* ag_pointer_h */
