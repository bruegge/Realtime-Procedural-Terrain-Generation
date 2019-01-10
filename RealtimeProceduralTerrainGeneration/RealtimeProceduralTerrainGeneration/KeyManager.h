#pragma once
#include <unordered_map>
#include <functional>

//to use this class: you should inherit this class to the objects class which should interact when pressing a key
class CKeyManager 
{
public:

	enum EMouseAxis : unsigned int
	{
		None = 0,
		XAxis = 1,
		YAxis = 2
	};

	void AddKeyBinding(unsigned int nKey, std::function<void()> function); //add a keybinding and function to this object
	void AddMouseAxisBinding(EMouseAxis eAxis, std::function<void(double)> function); //add a mousemove event and function to this object
	void RemoveKeyBinding(unsigned int nKey); //removes keybindings
	void RemoveMouseAxisBinding(EMouseAxis eAxis); //removes mousemove event

	static void KeyHasPressed(unsigned int nKey); //this method is called from the CGLFWWindow class everytime a key has been pressed
	static void MouseHasMoved(EMouseAxis eAxis, double dDirection); //this method is called from the CGLFWWindow class everytime the mouse has been moved

protected:
	CKeyManager();
	~CKeyManager();

private:
	std::unordered_map<unsigned int, std::function<void()>> m_umKeyBindings; //a list of all keybinding for this object
	std::unordered_map<EMouseAxis, std::function<void(double)>> m_umMouseAxisBindings; //a list of all mousemove bindings for this object

};

