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
	
	const long Window::_eventMask =
		KeyPressMask |
		KeyReleaseMask |
		ButtonPressMask |
		FocusChangeMask |
		ButtonReleaseMask |
		PointerMotionMask |
		StructureNotifyMask;
	unsigned int Window::_users = 0;
	_x11::Display *Window::_display = 0;
	
	Bool Window::_checkCloseEvent(_x11::Display *_d, _x11::XEvent *ev, _x11::XPointer p)
	{
		using namespace _x11;
		
		Ag::Window &win = *(Ag::Window *)p;
		
		if (ev->type != ClientMessage)
			return false;
		
		if (
			ev->xclient.window == win._handle &&
			ev->xclient.message_type == XInternAtom(_display, "WM_PROTOCOLS", False) &&
			(Atom)ev->xclient.data.l[0] == *(Atom *)p
		)
			return true;
		
		return false;
	}
	const bool Window::_init()
	{
		using namespace _x11;
		
		_display = XOpenDisplay(0);
		
		if (!_display)
			return false;
		
		return true;
	}
	const bool Window::_quit()
	{
		using namespace _x11;
		
		XCloseDisplay(_display);
		
		_display = 0;
		
		return true;
	}
	const bool Window::_callback(const Window::Event &e)
	{
		if (callback && e.id != Window::Event::UNDEFINED)
		{
			if (callData)
				return callback.cast<const bool (*)(const Window::Event &, const Pointer &)>()(e, callData);
			else
				return callback.cast<const bool (*)(const Window::Event &)>()(e);
		}
		
		return false;
	}
	Window::Window() :
		_deleteAtom(),
		_handle(),
		_context(),
		state(Window::CLOSED),
		callback(),
		callData()
	{ }
	Window::~Window()
	{
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
		using namespace _x11;
		
		if (!_display)
			if (!_init())
				return false;
		
		if (state != Window::CLOSED)
			return state;
		
		XVisualInfo *vi;
		
		GLint att[] =
		{
			GLX_RGBA,
			GLX_DOUBLEBUFFER,
			GLX_RED_SIZE, cr,
			GLX_GREEN_SIZE, cg,
			GLX_BLUE_SIZE, cb,
			GLX_ALPHA_SIZE, ca,
			GLX_ACCUM_RED_SIZE, ar,
			GLX_ACCUM_GREEN_SIZE, ag,
			GLX_ACCUM_BLUE_SIZE, ab,
			GLX_ACCUM_ALPHA_SIZE, aa,
			GLX_DEPTH_SIZE, d,
			GLX_STENCIL_SIZE, s,
			None
		};
		
		vi = glXChooseVisual(_display, 0, att);
		
		if (!vi)
			return false;
		
		Colormap cmap = XCreateColormap(_display, DefaultRootWindow(_display), vi->visual, AllocNone);
		
		XSetWindowAttributes swa;
		
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;
		
		_handle = XCreateWindow(
			_display,
			DefaultRootWindow(_display),
			x, y,
			w, h,
			0,
			vi->depth,
			InputOutput,
			vi->visual,
			CWColormap | CWEventMask,
			&swa
		);
		
		_deleteAtom = XInternAtom(_display, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(_display, _handle, &_deleteAtom, 1);
		
		XSelectInput(
			_display,
			_handle,
			_eventMask
		);
		
		XMapWindow(_display, _handle);
		
		XStoreName(_display, _handle, t.c_str());
		
		_context = glXCreateContext(_display, vi, 0, True);
		
		if (!_context)
			return false;
		
		while (true)
		{
			XEvent e;
			XNextEvent(_display, &e);
			if (e.type == MapNotify)
				break;
		}
		
		MotifWmHints newhints;
		newhints.flags =
			MWM_HINTS_FUNCTIONS |
			MWM_HINTS_DECORATIONS;
		newhints.functions =
			MWM_FUNC_MOVE |
			MWM_FUNC_CLOSE |
			(opt & Window::RESIZE ? MWM_FUNC_RESIZE : 0) |
			(opt & Window::MINIMIZE ? MWM_FUNC_MINIMIZE : 0) |
			(opt & Window::MAXIMIZE ? MWM_FUNC_MAXIMIZE : 0);
		newhints.decorations =
			MWM_DECOR_BORDER |
			MWM_DECOR_TITLE |
			MWM_DECOR_MENU |
			(opt & Window::RESIZE ? MWM_DECOR_RESIZEH : 0) |
			(opt & Window::MINIMIZE ? MWM_DECOR_MINIMIZE : 0) |
			(opt & Window::MAXIMIZE ? MWM_DECOR_MAXIMIZE : 0);
		
		Atom
			prop,
			type;
		int format;
		unsigned long
			nitems,
			bytes_after;
		unsigned char *data;
		MotifWmHints *hints;
		
		prop = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
		if (prop == None)
			return False;
		
		XGetWindowProperty(
			_display,
			_handle,
			prop,
			0,
			sizeof(MotifWmHints) / sizeof(long),
			False,
			AnyPropertyType,
			&type,
			&format,
			&nitems,
			&bytes_after,
			&data
		);
		
		if (type == None)
			hints = &newhints;
		else
		{
			hints = (MotifWmHints *)data;
			
			if (newhints.flags & MWM_HINTS_FUNCTIONS)
			{
				hints->flags |= MWM_HINTS_FUNCTIONS;
				hints->functions = newhints.functions;
			}
			if (newhints.flags & MWM_HINTS_DECORATIONS)
			{
				hints->flags |= MWM_HINTS_DECORATIONS;
				hints->decorations = newhints.decorations;
			}
		}
		
		XChangeProperty(
			_display,
			_handle,
			prop,
			prop,
			32,
			PropModeReplace,
			(unsigned char *)hints,
			sizeof(MotifWmHints) / sizeof(long)
		);
		
		if (hints != &newhints)
			XFree(hints);
		
		++_users;
		
		return (state = Window::OPEN);
	}
	const bool Window::close()
	{
		using namespace _x11;
		
		glXMakeCurrent(_display, None, 0);
		glXDestroyContext(_display, _context);
		XDestroyWindow(_display, _handle);
		
		state = Window::CLOSED;
		--_users;
		
		if (_users == 0)
			return _quit();
		
		return true;
	}
	const bool Window::activate()
	{
		using namespace _x11;
		
		return glXMakeCurrent(_display, _handle, _context);
	}
	const bool Window::minimize()
	{
		using namespace _x11;
		
		XClientMessageEvent ev;
		Atom prop;
		
		prop = XInternAtom(_display, "WM_CHANGE_STATE", False);
		if (prop == None)
			return False;
		
		ev.type = ClientMessage;
		ev.window = _handle;
		ev.message_type = prop;
		ev.format = 32;
		ev.data.l[0] = IconicState;
		XSendEvent(
			_display,
			DefaultRootWindow(_display),
			False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			(XEvent *)&ev
		);
		
		return true;
	}
	const bool Window::maximize()
	{
		using namespace _x11;
		
		XClientMessageEvent ev;
		Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
		Atom max_horz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		Atom max_vert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		
		memset(&ev, 0, sizeof(ev));
		ev.type = ClientMessage;
		ev.window = _handle;
		ev.message_type = wm_state;
		ev.format = 32;
		ev.data.l[0] = 1;
		ev.data.l[1] = max_horz;
		ev.data.l[2] = max_vert;
		
		XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureNotifyMask, (XEvent *)&ev);
		
		return true;
	}
	const bool Window::restore()
	{
		using namespace _x11;
		
		XClientMessageEvent ev;
		Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
		Atom max_horz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		Atom max_vert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
	
		memset(&ev, 0, sizeof(ev));
		ev.type = ClientMessage;
		ev.window = _handle;
		ev.message_type = wm_state;
		ev.format = 32;
		ev.data.l[0] = 0;
		ev.data.l[1] = max_horz;
		ev.data.l[2] = max_vert;
	
		XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureNotifyMask, (XEvent *)&ev);
		
		return true;
	}
	const bool Window::swap()
	{
		using namespace _x11;
		
		glXSwapBuffers(_display, _handle);
		
		return true;
	}
	const bool Window::poll()
	{
		using namespace _x11;
		
		XEvent ev;
		while (state == Window::OPEN && XCheckIfEvent(_display, &ev, _checkCloseEvent, (XPointer)this))
		{
			if (!callback)
				return close();
			
			if (_callback(Window::Event(this, Window::Event::CLOSE)))
				return true;
		}
		
		while (state == Window::OPEN && XCheckWindowEvent(_display, _handle, _eventMask, &ev) == True)
		{
			switch (ev.type)
			{
				case CreateNotify:
					_callback(Window::Event(this, Window::Event::OPEN));
					break;
				case DestroyNotify:
					break;
				case ConfigureNotify:
				{
					Window::Event e (this, Window::Event::MOVE);
					
					e.args[0] = ev.xconfigure.x;
					e.args[1] = ev.xconfigure.y;
					_callback(e);
					
					e.id = Window::Event::RESIZE;
					e.args[0] = ev.xconfigure.width;
					e.args[1] = ev.xconfigure.height;
					_callback(e);
					
					break;
				}
				case ButtonPress:
				case ButtonRelease:
				{
					Window::Event e (this, (ev.type == ButtonPress ? Window::Event::MOUSEDOWN : Window::Event::MOUSEUP));
					
					e.args[3] =
						(ev.xbutton.state & ShiftMask ? Key::SHIFT : 0) |
						(ev.xbutton.state & LockMask ? Key::CAPSLOCK | Key::SHIFT : 0) |
						(ev.xbutton.state & ControlMask ? Key::CONTROL : 0) |
						(ev.xbutton.state & Mod1Mask ? Key::ALT : 0);
					
					switch (ev.xbutton.button)
					{
						case Button1:
						case Button2:
						case Button3:
							switch (ev.xbutton.button)
							{
								case Button1:
									e.args[0] = Key::MOUSELEFT;
									break;
								case Button2:
									e.args[0] = Key::MOUSEMIDDLE;
									break;
								case Button3:
									e.args[0] = Key::MOUSERIGHT;
									break;
								default:
									break;
							}
							
							unsigned int
								width,
								height;
							
							if (!getSize(width, height))
								e.id = Window::Event::UNDEFINED;
							
							e.args[1] = ev.xbutton.x;
							e.args[2] = height - ev.xbutton.y;
							
							break;
						case Button4:
							e.id = Window::Event::MOUSEWHEEL;
							e.args[0] = 1;
							break;
						case Button5:
							e.id = Window::Event::MOUSEWHEEL;
							e.args[0] = -1;
							break;
						default:
							break;
					}
					_callback(e);
					
					break;
				}
				case FocusIn:
					_callback(Window::Event(this, Window::Event::GAINFOCUS));
					break;
				case FocusOut:
					_callback(Window::Event(this, Window::Event::LOSEFOCUS));
					break;
				case KeyPress:
				case KeyRelease:
				{
					Window::Event e (this, (ev.type == KeyPress ? Window::Event::KEYDOWN : Window::Event::KEYUP));
					KeySym key;
					char buf[256];
					XLookupString(
						&ev.xkey,
						buf,
						256,
						&key,
						0
					);
					
					e.args[3] =
						(ev.xkey.state & ShiftMask ? Key::SHIFT : 0) |
						(ev.xkey.state & LockMask ? Key::CAPSLOCK | Key::SHIFT : 0) |
						(ev.xkey.state & ControlMask ? Key::CONTROL : 0) |
						(ev.xkey.state & Mod1Mask ? Key::ALT : 0);
					
					switch (key)
					{
						case NoSymbol:
							e.id = Window::Event::UNDEFINED;
							break;
						case XK_BackSpace:
							e.args[0] = Key::BACKSPACE;
							break;
						case XK_Tab:
							e.args[0] = Key::TAB;
							break;
						case XK_Clear:
							e.args[0] = Key::CLEAR;
							break;
						case XK_Return:
							e.args[0] = Key::ENTER;
							break;
						case XK_Shift_L:
							e.args[0] = Key::SHIFT;
							e.args[1] = 0;
							break;
						case XK_Shift_R:
							e.args[0] = Key::SHIFT;
							e.args[1] = 1;
							break;
						case XK_Control_L:
							e.args[0] = Key::CONTROL;
							e.args[1] = 0;
							break;
						case XK_Control_R:
							e.args[0] = Key::CONTROL;
							e.args[1] = 1;
							break;
						case XK_Alt_L:
							e.args[0] = Key::ALT;
							e.args[1] = 0;
							break;
						case XK_Alt_R:
							e.args[0] = Key::ALT;
							e.args[1] = 1;
							break;
						case XK_Pause:
							e.args[0] = Key::PAUSE;
							break;
						case XK_Caps_Lock:
							e.args[0] = Key::CAPSLOCK;
							break;
						case XK_Escape:
							e.args[0] = Key::ESCAPE;
							break;
						case XK_Page_Up:
							e.args[0] = Key::PAGEUP;
							break;
						case XK_Page_Down:
							e.args[0] = Key::PAGEDOWN;
							break;
						case XK_End:
							e.args[0] = Key::END;
							break;
						case XK_Home:
							e.args[0] = Key::HOME;
							break;
						case XK_Left:
							e.args[0] = Key::LEFT;
							break;
						case XK_Up:
							e.args[0] = Key::UP;
							break;
						case XK_Right:
							e.args[0] = Key::RIGHT;
							break;
						case XK_Down:
							e.args[0] = Key::DOWN;
							break;
						case XK_Insert:
							e.args[0] = Key::INSERT;
							break;
						case XK_Delete:
							e.args[0] = Key::DELETE;
							break;
						case XK_KP_Home:
							e.args[0] = Key::HOME;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Left:
							e.args[0] = Key::LEFT;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Up:
							e.args[0] = Key::UP;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Right:
							e.args[0] = Key::RIGHT;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Down:
							e.args[0] = Key::DOWN;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Page_Up:
							e.args[0] = Key::PAGEUP;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Page_Down:
							e.args[0] = Key::PAGEDOWN;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_End:
							e.args[0] = Key::END;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Delete:
							e.args[0] = Key::DELETE;
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Equal:
							e.args[0] = Key::CHAR;
							e.args[1] = '=';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Multiply:
							e.args[0] = Key::CHAR;
							e.args[1] = '*';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Add:
							e.args[0] = Key::CHAR;
							e.args[1] = '+';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Separator:
							e.args[0] = Key::CHAR;
							e.args[1] = ',';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Subtract:
							e.args[0] = Key::CHAR;
							e.args[1] = '-';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Decimal:
							e.args[0] = Key::CHAR;
							e.args[1] = '.';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_KP_Divide:
							e.args[0] = Key::CHAR;
							e.args[1] = '/';
							e.args[3] |= Key::NUMPAD;
							break;
						case XK_Num_Lock:
							e.args[0] = Key::NUMLOCK;
							break;
						case XK_Scroll_Lock:
							e.args[0] = Key::SCROLLLOCK;
							break;
						default:
							if (key >= 0x20 && key <= 0x7e)
							{
								e.args[0] = Key::CHAR;
								e.args[1] = (key >= 0x61 && key <= 0x7a ? key - 0x20 : key);
							}
							if (key >= XK_KP_0 && key <= XK_KP_9)
							{
								e.args[0] = Key::CHAR;
								e.args[1] = '0' + (key - XK_KP_0);
								e.args[3] |= Key::NUMPAD;
							}
							if (key >= XK_F1 && key <= XK_F24)
							{
								e.args[0] = Key::FUNCTION;
								e.args[1] = key - XK_F1 + 1;
							}
							
							break;
					}
					
					_callback(e);
					
					break;
				}
				case MotionNotify:
				{
					Window::Event e (this, Window::Event::MOUSEMOVE);
					
					e.args[3] =
						(ev.xmotion.state & ShiftMask ? Key::SHIFT : 0) |
						(ev.xmotion.state & LockMask ? Key::CAPSLOCK | Key::SHIFT : 0) |
						(ev.xmotion.state & ControlMask ? Key::CONTROL : 0) |
						(ev.xmotion.state & Mod1Mask ? Key::ALT : 0);
					
					unsigned int
						width,
						height;
					
					if (!getSize(width, height))
						e.id = Window::Event::UNDEFINED;
					
					e.args[0] = ev.xmotion.x;
					e.args[1] = height - ev.xmotion.y;
					
					_callback(e);
					
					break;
				}
				default:
					break;
			}
		}
		
		return true;
	}
	const bool Window::setTitle(const std::string &t)
	{
		using namespace _x11;
		
		XStoreName(_display, _handle, t.c_str());
		
		return true;
	}
	const bool Window::setSize(unsigned int w, unsigned int h)
	{
		using namespace _x11;
		
		XResizeWindow(_display, _handle, w, h);
		
		return true;
	}
	const bool Window::setPos(unsigned int x, unsigned int y)
	{
		using namespace _x11;
		
		XMoveWindow(_display, _handle, x, y);
		
		return true;
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
		using namespace _x11;
		
		XTextProperty text;
		Atom prop = 0;
		
		XGetTextProperty(_display, _handle, &text, prop);
		
		t = std::string((const char *)text.value, 0, text.nitems);

		return true;
	}
	const bool Window::getSize(unsigned int &w, unsigned int &h)
	{
		using namespace _x11;
		
		_x11::Window r;
		int
			x,
			y;
		unsigned int
			b,
			d;
		XGetGeometry(_display, _handle, &r, &x, &y, &w, &h, &b, &d);
		
		return true;
	}
	const bool Window::getPos(unsigned int &x, unsigned int &y)
	{
		using namespace _x11;
		
		_x11::Window r;
		int
			x2,
			y2;
		unsigned int
			w,
			h,
			b,
			d;
		XGetGeometry(_display, _handle, &r, &x2, &y2, &w, &h, &b, &d);
		
		x = x2;
		y = y2;
		
		return true;
	}
}

#endif /* ag_window_c */
