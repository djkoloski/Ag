#ifndef ag_window_h
#define ag_window_h

namespace Ag
{
	class Window
	{
		private:
			_win32::HWND _handle;
			_win32::HDC _deviceContext;
			_win32::HGLRC _renderContext;

			static const bool _classExists(const std::string &);
			static const bool _createClass();
			static _win32::LRESULT CALLBACK _callback(_win32::HWND, _win32::UINT, _win32::WPARAM, _win32::LPARAM);
			
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
