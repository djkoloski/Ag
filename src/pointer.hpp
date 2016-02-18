#ifndef ag_pointer_h
#define ag_pointer_h

namespace Ag
{
	class Pointer
	{
		public:
			class ptr_base
			{
				private:
					ptr_base(const ptr_base &);
					ptr_base &operator=(const ptr_base &);
					
				public:
					ptr_base();
					virtual ~ptr_base();
					
					virtual ptr_base *copy() = 0;
			};

			template <class T>
			class ptr : public ptr_base
			{
				public:
					T data;
					
					ptr();
					ptr(T);
					ptr(const ptr<T> &);
					virtual ~ptr();
					
					virtual ptr_base *copy();
					
					ptr<T> &operator=(const ptr<T> &);
			};
			
			ptr_base *p;

			Pointer();
			template <class T>
			Pointer(const T);
			Pointer(const Pointer &);

			~Pointer();

			template <class T>
			T cast() const;

			operator bool() const;
			const bool operator!() const;

			template <class T>
			Pointer &operator=(const T);
			Pointer &operator=(const Pointer &);
	};
}

#endif /* ag_pointer_h */
