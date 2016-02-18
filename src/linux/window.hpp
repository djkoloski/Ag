#ifndef ag_window_h
#define ag_window_h

namespace Ag
{
	class Window
	{
		public:
			class Event
			{
				public:
					Window *win;
					unsigned int id;
					int args[4];
					
					Event();
					Event(Window *, unsigned int);
					
					enum events
					{
						UNDEFINED = 0,
						OPEN,
						CLOSE,
						MOVE,
						RESIZE,
						MOUSEDOWN,
						MOUSEUP,
						KEYDOWN,
						KEYUP,
						MOUSEMOVE,
						MOUSEWHEEL,
						GAINFOCUS,
						LOSEFOCUS
					};
			};
		private:
			const static long _eventMask;
			static unsigned int _users;
			static _x11::Display *_display;
			_x11::Atom _deleteAtom;
			_x11::Window _handle;
			_x11::GLXContext _context;
			
			static Bool _checkCloseEvent(_x11::Display *, _x11::XEvent *, _x11::XPointer);
			static const bool _init();
			static const bool _quit();
			const bool _callback(const Window::Event &);
			
			Window(const Window &);
			Window &operator=(const Window &);		
		public:
			enum states
			{
				CLOSED = 0,
				OPEN = 1
			};
			
			enum options
			{
				RESIZE = 1,
				MINIMIZE = 2,
				MAXIMIZE = 4
			};
			
			int state;
			Pointer
				callback,
				callData;
			
			Window();
			~Window();
			
			const bool open(unsigned int, unsigned int, const std::string &);
			const bool open(unsigned int, unsigned int, const std::string &, unsigned int);
			const bool open(unsigned int, unsigned int, unsigned int, unsigned int, const std::string &);
			const bool open(unsigned int, unsigned int, unsigned int, unsigned int, const std::string &, unsigned int);
			const bool open(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, const std::string &);
			const bool open(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, const std::string &, unsigned int);
			const bool open(unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, const std::string &);
			const bool open(unsigned int, unsigned int, unsigned int, unsigned int, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, const std::string &, unsigned int);
			const bool close();
			
			const bool activate();
			const bool minimize();
			const bool maximize();
			const bool restore();
			const bool swap();
			const bool poll();
			
			const bool setTitle(const std::string &);
			const bool setSize(unsigned int, unsigned int);
			const bool setPos(unsigned int, unsigned int);
			const bool setCallback(const bool (*)(const Event &));
			const bool setCallback(const bool (*)(const Event &, const Pointer &), const Pointer &);
			const bool setCallData(const Pointer &);
			
			const bool getTitle(std::string &);
			const bool getSize(unsigned int &, unsigned int &);
			const bool getPos(unsigned int &, unsigned int &);
	};
}

#endif /* ag_window_h */
