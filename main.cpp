#include "src/ag.h"

#include <iostream>
#include <cmath>

class State
{
	public:
		unsigned int wheel;
		
		State();
};

State::State() :
	wheel(0)
{ }

const bool callback(const Ag::Window::Event &e, const Ag::Pointer &d)
{
	State *state = d.cast<State *>();
	
	if (
		e.id == Ag::Window::Event::CLOSE ||
		(e.id == Ag::Window::Event::KEYDOWN && e.args[0] == Ag::Key::ESCAPE)
	)
	{
		e.win->close();
		
		return true;
	}
	
	if (e.id == Ag::Window::Event::KEYDOWN)
	{
		switch (e.args[0])
		{
			case Ag::Key::UP:
				if (e.args[3] & Ag::Key::ALT)
					++state->wheel;
				
				return true;
			default:
				break;
		}
	}
	
	return false;
}

int main()
{
	Ag::Window
		win1,
		win2;
	
	State
		state1,
		state2;
	
	win1.setCallback(callback, &state1);
	win2.setCallback(callback, &state2);
	
	if (!win1.open(800, 600, "Ag Test Window 1", Ag::Window::MINIMIZE))
		return 0;
	
	if (!win2.open(800, 600, "Ag Test Window 2", Ag::Window::MINIMIZE))
		return 0;
	
	while (win1.state || win2.state)
	{
		if (win1.state)
		{
			win1.activate();
			
			glClearColor(state1.wheel / 16.0f, 0.0f, state1.wheel / 16.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			win1.swap();
			win1.poll();
		}
		
		if (win2.state)
		{
			win2.activate();
		
			glClearColor(state2.wheel / 16.0f, 0.0f, state2.wheel / 16.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
			win2.swap();
			win2.poll();
		}
	}
	
	return 0;
}
