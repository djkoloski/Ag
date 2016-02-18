#ifndef ag_window_c
#define ag_window_c

namespace Ag
{
	Window::Event::Event() :
		win(0),
		id(Window::Event::UNDEFINED),
		args()
	{
		args[0] = 0;
		args[1] = 0;
		args[2] = 0;
		args[3] = 0;
	}
	Window::Event::Event(Window *w, unsigned int i) :
		win(w),
		id(i),
		args()
	{
		args[0] = 0;
		args[1] = 0;
		args[2] = 0;
		args[3] = 0;
	}
	
	const bool Window::_classExists(const std::string &name)
	{
		using namespace _win32;
		
		WNDCLASSEX wc;
		return
			GetClassInfoEx(
				GetModuleHandle(0),
				name.c_str(),
				&wc
			);
	}
	const bool Window::_createClass()
	{
		using namespace _win32;
		
		WNDCLASSEX wc =
		{	
			sizeof(WNDCLASSEX),
			CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
			_callback,
			sizeof(LONG_PTR),
			sizeof(LONG_PTR),
			GetModuleHandle(0),
			LoadIcon(0, IDI_APPLICATION),
			LoadCursor(0, IDC_ARROW),
			(HBRUSH)GetStockObject(BLACK_BRUSH),
			0,
			"Ag_Window",
			0
		};
		
		return RegisterClassEx(&wc);
	}
	_win32::LRESULT CALLBACK Window::_callback(_win32::HWND window, _win32::UINT message, _win32::WPARAM wparam, _win32::LPARAM lparam)
	{
		using namespace _win32;
		
		if (message == WM_NCCREATE)
			SetWindowLong(window, 0, (LONG_PTR)((LPCREATESTRUCT)lparam)->lpCreateParams);
		
		Window *target = (Window *)GetWindowLongPtr(window, 0);
		
		if (!target)
			return DefWindowProc(window, message, wparam, lparam);
		
		Window::Event e;
		e.win = target;
		
		if (
			message == WM_SYSKEYDOWN ||
			message == WM_SYSKEYUP ||
			message == WM_KEYDOWN ||
			message == WM_KEYUP ||
			message == WM_LBUTTONDOWN ||
			message == WM_LBUTTONUP ||
			message == WM_RBUTTONDOWN ||
			message == WM_RBUTTONUP ||
			message == WM_MBUTTONDOWN ||
			message == WM_MBUTTONUP ||
			message == WM_MOUSEMOVE ||
			message == WM_MOUSEWHEEL
		)
			e.args[3] =
				(GetAsyncKeyState(VK_SHIFT) ? Key::SHIFT : 0) |
				(GetKeyState(VK_CAPITAL) ? Key::CAPSLOCK | Key::SHIFT : 0) |
				(GetAsyncKeyState(VK_CONTROL) ? Key::CONTROL : 0) |
				(GetAsyncKeyState(VK_MENU) ? Key::ALT : 0);
		
		switch (message)
		{
			case WM_CREATE:
				e.id = Window::Event::OPEN;
				e.args[3] = 0;
				break;
			case WM_CLOSE:
				e.id = Window::Event::CLOSE;
				e.args[3] = 0;
				break;
			case WM_MOVE:
				e.id = Window::Event::MOVE;
				e.args[0] = (int)(short)LOWORD(lparam);
				e.args[1] = (int)(short)HIWORD(lparam);
				break;
			case WM_SIZE:
				e.id = Window::Event::RESIZE;
				e.args[0] = (int)(short)LOWORD(lparam);
				e.args[1] = (int)(short)HIWORD(lparam);
				break;
			case WM_ACTIVATE:
				if (wparam == WA_INACTIVE)
					e.id = Window::Event::LOSEFOCUS;
				else
					e.id = Window::Event::GAINFOCUS;
				break;
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				switch (wparam)
				{
					case VK_LBUTTON:
					case VK_RBUTTON:
					case VK_MBUTTON:
						e.id = ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) ? Window::Event::MOUSEDOWN : Window::Event::MOUSEUP);
						break;
					case VK_BACK:
					case VK_TAB:
					case VK_CLEAR:
					case VK_RETURN:
					case VK_SHIFT:
					case VK_CONTROL:
					case VK_MENU:
					case VK_PAUSE:
					case VK_CAPITAL:
					case VK_ESCAPE:
					case VK_SPACE:
					case VK_PRIOR:
					case VK_NEXT:
					case VK_END:
					case VK_HOME:
					case VK_LEFT:
					case VK_UP:
					case VK_RIGHT:
					case VK_DOWN:
					case VK_INSERT:
					case VK_DELETE:
					case VK_MULTIPLY:
					case VK_ADD:
					case VK_SEPARATOR:
					case VK_SUBTRACT:
					case VK_DECIMAL:
					case VK_DIVIDE:
					case VK_NUMLOCK:
					case VK_SCROLL:
					case VK_LSHIFT:
					case VK_RSHIFT:
					case VK_LCONTROL:
					case VK_RCONTROL:
					case VK_LMENU:
					case VK_RMENU:
						e.id = ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) ? Window::Event::KEYDOWN : Window::Event::KEYUP);
						break;
					default:
						if (
							(wparam >= 0x30 && wparam <= 0x39) ||
							(wparam >= 0x41 && wparam <= 0x5a) ||
							(wparam >= 0x60 && wparam <= 0x69) ||
							(wparam >= 0x70 && wparam <= 0x87)
						)
							e.id = ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) ? Window::Event::KEYDOWN : Window::Event::KEYUP);
						
						break;
				}
				
				switch (wparam)
				{
					case VK_LBUTTON:
						e.args[0] = Key::MOUSELEFT;
						break;
					case VK_RBUTTON:
						e.args[0] = Key::MOUSERIGHT;
						break;
					case VK_MBUTTON:
						e.args[0] = Key::MOUSEMIDDLE;
						break;
					case VK_BACK:
						e.args[0] = Key::BACKSPACE;
						break;
					case VK_TAB:
						e.args[0] = Key::TAB;
						break;
					case VK_CLEAR:
						e.args[0] = Key::CLEAR;
						break;
					case VK_RETURN:
						e.args[0] = Key::ENTER;
						break;
					case VK_SHIFT:
						e.args[0] = Key::SHIFT;
						e.args[1] = (lparam & 0x1000000 ? 1 : 0);
						break;
					case VK_CONTROL:
						e.args[0] = Key::CONTROL;
						e.args[1] = (lparam & 0x1000000 ? 1 : 0);
						break;
					case VK_MENU:
						e.args[0] = Key::ALT;
						e.args[1] = (lparam & 0x1000000 ? 1 : 0);
						break;
					case VK_PAUSE:
						e.args[0] = Key::PAUSE;
						break;
					case VK_CAPITAL:
						e.args[0] = Key::CAPSLOCK;
						break;
					case VK_ESCAPE:
						e.args[0] = Key::ESCAPE;
						break;
					case VK_SPACE:
						e.args[0] = Key::CHAR;
						e.args[1] = ' ';
						break;
					case VK_PRIOR:
						e.args[0] = Key::PAGEUP;
						break;
					case VK_NEXT:
						e.args[0] = Key::PAGEDOWN;
						break;
					case VK_END:
						e.args[0] = Key::END;
						break;
					case VK_HOME:
						e.args[0] = Key::HOME;
						break;
					case VK_LEFT:
						e.args[0] = Key::LEFT;
						break;
					case VK_UP:
						e.args[0] = Key::UP;
						break;
					case VK_RIGHT:
						e.args[0] = Key::RIGHT;
						break;
					case VK_DOWN:
						e.args[0] = Key::DOWN;
						break;
					case VK_INSERT:
						e.args[0] = Key::INSERT;
						break;
					case VK_DELETE:
						e.args[0] = Key::DELETE;
						break;
					case VK_MULTIPLY:
						e.args[0] = Key::CHAR;
						e.args[1] = '*';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_ADD:
						e.args[0] = Key::CHAR;
						e.args[1] = '+';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_SEPARATOR:
						e.args[0] = Key::CHAR;
						e.args[1] = ',';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_SUBTRACT:
						e.args[0] = Key::CHAR;
						e.args[1] = '-';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_DECIMAL:
						e.args[0] = Key::CHAR;
						e.args[1] = '.';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_DIVIDE:
						e.args[0] = Key::CHAR;
						e.args[1] = '/';
						e.args[3] |= Key::NUMPAD;
						break;
					case VK_NUMLOCK:
						e.args[0] = Key::NUMLOCK;
						break;
					case VK_SCROLL:
						e.args[0] = Key::SCROLLLOCK;
						break;
					case VK_LSHIFT:
						e.args[0] = Key::SHIFT;
						e.args[1] = 0;
						break;
					case VK_RSHIFT:
						e.args[0] = Key::SHIFT;
						e.args[1] = 1;
						break;
					case VK_LCONTROL:
						e.args[0] = Key::CONTROL;
						e.args[1] = 0;
						break;
					case VK_RCONTROL:
						e.args[0] = Key::CONTROL;
						e.args[1] = 1;
						break;
					case VK_LMENU:
						e.args[0] = Key::ALT;
						e.args[1] = 0;
						break;
					case VK_RMENU:
						e.args[0] = Key::ALT;
						e.args[1] = 1;
						break;
					default:
						if (
							(wparam >= 0x30 && wparam <= 0x39) ||
							(wparam >= 0x41 && wparam <= 0x5a)
						)
						{
							e.args[0] = Key::CHAR;
							e.args[1] = wparam;
						}
						else
						{
							if (wparam >= 0x60 && wparam <= 0x69)
							{
								e.args[0] = Key::CHAR;
								e.args[1] = '0' + (wparam - 0x60);
								e.args[3] |= Key::NUMPAD;
							}
							if (wparam >= 0x70 && wparam <= 0x87)
							{
								e.args[0] = Key::FUNCTION;
								e.args[1] = wparam - 0x6f;
							}
						}
						
						break;
				}
				
				break;
			}
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			{
				switch (message)
				{
					case WM_LBUTTONDOWN:
						e.id = Window::Event::MOUSEDOWN;
						e.args[0] = Key::MOUSELEFT;
						break;
					case WM_LBUTTONUP:
						e.id = Window::Event::MOUSEUP;
						e.args[0] = Key::MOUSELEFT;
						break;
					case WM_RBUTTONDOWN:
						e.id = Window::Event::MOUSEDOWN;
						e.args[0] = Key::MOUSERIGHT;
						break;
					case WM_RBUTTONUP:
						e.id = Window::Event::MOUSEUP;
						e.args[0] = Key::MOUSERIGHT;
						break;
					case WM_MBUTTONDOWN:
						e.id = Window::Event::MOUSEDOWN;
						e.args[0] = Key::MOUSEMIDDLE;
						break;
					case WM_MBUTTONUP:
						e.id = Window::Event::MOUSEUP;
						e.args[0] = Key::MOUSEMIDDLE;
						break;
					default:
						e.id = Window::Event::UNDEFINED;
						break;
				}
				
				unsigned int
					width,
					height;
				
				if (!target->getSize(width, height))
					e.id = Window::Event::UNDEFINED;
				
				e.args[1] = (signed short)LOWORD(lparam);
				e.args[2] = height - (signed short)HIWORD(lparam);
				
				break;
			}
			case WM_MOUSEMOVE:
			{
				e.id = Window::Event::MOUSEMOVE;
				
				unsigned int
					width,
					height;
				
				if (!target->getSize(width, height))
					e.id = Window::Event::UNDEFINED;
				
				e.args[0] = (signed short)LOWORD(lparam);
				e.args[1] = height - (signed short)HIWORD(lparam);
				
				break;
			}
			case WM_MOUSEWHEEL:
				e.id = Window::Event::MOUSEWHEEL;
				e.args[0] = GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
				break;
			default:
				e.id = Window::Event::UNDEFINED;
				break;
		}
		
		if (target->callback && e.id != Window::Event::UNDEFINED)
		{
			if (target->callData)
			{
				if (target->callback.cast<const bool (*)(const Window::Event &, const Pointer &)>()(e, target->callData))
					return 0;
				else
					return DefWindowProc(window, message, wparam, lparam);
			}
			else
			{
				if (target->callback.cast<const bool (*)(const Window::Event &)>()(e))
					return 0;
				else
					return DefWindowProc(window, message, wparam, lparam);
			}
		}
		else
			return DefWindowProc(window, message, wparam, lparam);
	}
	Window::Window() :
		_handle(),
		_deviceContext(),
		_renderContext(),
		state(Window::CLOSED),
		callback(),
		callData()
	{ }
	Window::~Window()
	{
		wglDeleteContext(_renderContext);
		ReleaseDC(_handle, _deviceContext);

		state = Window::CLOSED;
	}
	const bool Window::open(unsigned int w, unsigned int h, const std::string &t)
	{
		return open(0, 0, w, h, 8, 8, 8, 8, 8, 8, 8, 8, 24, 0, t, 0);
	}
	const bool Window::open(unsigned int w, unsigned int h, const std::string &t, unsigned int opt)
	{
		return open(0, 0, w, h, 8, 8, 8, 8, 8, 8, 8, 8, 24, 0, t, opt);
	}
	const bool Window::open(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::string &t)
	{
		return open(x, y, w, h, 8, 8, 8, 8, 8, 8, 8, 8, 24, 0, t, 0);
	}
	const bool Window::open(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::string &t, unsigned int opt)
	{
		return open(x, y, w, h, 8, 8, 8, 8, 8, 8, 8, 8, 24, 0, t, opt);
	}
	const bool Window::open(unsigned int w, unsigned int h, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca, unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa, unsigned char d, unsigned char s, const std::string &t)
	{
		return open(0, 0, w, h, cr, cg, cb, ca, ar, ag, ab, aa, d, s, t, 0);
	}
	const bool Window::open(unsigned int w, unsigned int h, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca, unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa, unsigned char d, unsigned char s, const std::string &t, unsigned int opt)
	{
		return open(0, 0, w, h, cr, cg, cb, ca, ar, ag, ab, aa, d, s, t, opt);
	}
	const bool Window::open(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca, unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa, unsigned char d, unsigned char s, const std::string &t)
	{
		return open(x, y, w, h, cr, cg, cb, ca, ar, ag, ab, aa, d, s, t, 0);
	}
	const bool Window::open(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca, unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa, unsigned char d, unsigned char s, const std::string &t, unsigned int opt)
	{
		using namespace _win32;
		
		if (state != Window::CLOSED)
			return state;
		
		if (!_classExists("Ag_Window"))
			if (!_createClass())
				return false;
		
		unsigned long long wclass = 0;

		if (opt & RESIZE)
			wclass |= WS_SIZEBOX;
		if (opt & MINIMIZE)
			wclass |= WS_MINIMIZEBOX;
		if (opt & MAXIMIZE)
			wclass |= WS_MAXIMIZEBOX;
		
		_handle = CreateWindowEx(
			0,
			"Ag_Window",
			t.c_str(),
			wclass | WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
			x, y,
			w, h,
			0,
			0,
			GetModuleHandle(0),
			(LPVOID)this
		);
		
		if (!_handle)
			return false;
		
		ULONG_PTR windowCount = GetClassLongPtr(_handle, 0);
		++windowCount;
		
		SetClassLongPtr(_handle, 0, windowCount);

		_deviceContext = GetDCEx(_handle, 0, 0);

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			cr + cg + cb,
			cr, 0,
			cg, 0,
			cb, 0,
			ca, 0,
			ar + ag + ab + aa,
			ar,
			ag,
			ab,
			aa,
			d,
			s,
			0,
			0,
			0,
			0,
			0,
			0
		};

		int pixelformat = ChoosePixelFormat(_deviceContext, &pfd);

		if (pixelformat == 0)
			return false;

		if (SetPixelFormat(_deviceContext, pixelformat, &pfd) == FALSE)
			return false;

		_renderContext = wglCreateContext(_deviceContext);

		return state = Window::OPEN;
	}
	const bool Window::close()
	{
		using namespace _win32;
		
		if (!_renderContext || !_handle || !_deviceContext)
			return false;

		ULONG_PTR windowCount = GetClassLongPtr(_handle, 0);
		--windowCount;

		if (windowCount != 0)
			SetClassLongPtr(_handle, 0, windowCount);
		
		wglDeleteContext(_renderContext);
		ReleaseDC(_handle, _deviceContext);
		
		DestroyWindow(_handle);

		state = Window::CLOSED;

		if (windowCount == 0)
			UnregisterClass("Ag_Window", GetModuleHandle(0));
		
		return true;
	}
	const bool Window::activate()
	{
		using namespace _win32;
		
		return wglMakeCurrent(_deviceContext, _renderContext);
	}
	const bool Window::minimize()
	{
		using namespace _win32;
		
		ShowWindow(_handle, SW_MINIMIZE);
		return true;
	}
	const bool Window::maximize()
	{
		using namespace _win32;
		
		ShowWindow(_handle, SW_MAXIMIZE);
		return true;
	}
	const bool Window::restore()
	{
		using namespace _win32;
		
		ShowWindow(_handle, SW_RESTORE);
		return true;
	}
	const bool Window::swap()
	{
		using namespace _win32;
		
		return SwapBuffers(_deviceContext) == TRUE;
	}
	const bool Window::poll()
	{
		using namespace _win32;
		
		MSG msg;
		while (PeekMessage(&msg, _handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}
	const bool Window::setTitle(const std::string &t)
	{
		using namespace _win32;
		
		if (!_handle)
			return false;

		return SetWindowText(_handle, t.c_str()) == TRUE;
	}
	const bool Window::setSize(unsigned int w, unsigned int h)
	{
		using namespace _win32;
		
		RECT
			clientRect,
			windowRect;
		POINT
			diff;

		GetClientRect(_handle, &clientRect);
		GetWindowRect(_handle, &windowRect);
		diff.x = (windowRect.right - windowRect.left) - clientRect.right;
		diff.y = (windowRect.bottom - windowRect.top) - clientRect.bottom;
		return !MoveWindow(_handle, windowRect.left, windowRect.top, w + diff.x, h + diff.y, TRUE);
	}
	const bool Window::setPos(unsigned int x, unsigned int y)
	{
		using namespace _win32;
		
		RECT windowRect;
		GetWindowRect(_handle, &windowRect);
		return !MoveWindow(_handle, x, y, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, TRUE);
	}
	const bool Window::setCallback(const bool (*c)(const Window::Event &))
	{
		callback = c;
		callData = 0;

		return true;
	}
	const bool Window::setCallback(const bool (*c)(const Window::Event &, const Pointer &), const Pointer &d)
	{
		callback = c;
		callData = d;

		return true;
	}
	const bool Window::setCallData(const Pointer &d)
	{
		callData = d;
		
		return true;
	}
	const bool Window::getTitle(std::string &t)
	{
		using namespace _win32;
		
		int l = GetWindowTextLength(_handle);
		
		if (!l)
			return false;
		
		t.resize(l);

		if (SetWindowText(_handle, t.c_str()) != TRUE)
			return false;

		return true;
	}
	const bool Window::getSize(unsigned int &w, unsigned int &h)
	{
		using namespace _win32;
		
		RECT clientRect;

		GetClientRect(_handle, &clientRect);

		w = clientRect.right - clientRect.left;
		h = clientRect.bottom - clientRect.top;

		return true;
	}
	const bool Window::getPos(unsigned int &x, unsigned int &y)
	{
		using namespace _win32;
		
		RECT windowRect;

		GetWindowRect(_handle, &windowRect);

		x = windowRect.left;
		y = windowRect.top;

		return true;
	}
}

#endif /* ag_window_c */
