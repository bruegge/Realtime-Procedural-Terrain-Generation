#include "KeyManager.h"
#include <set>

static std::set<CKeyManager*> setGlobalObjects;

CKeyManager::CKeyManager()
{
	setGlobalObjects.insert(this);
}

CKeyManager::~CKeyManager()
{
	setGlobalObjects.erase(this);
}

void CKeyManager::AddKeyBinding(unsigned int nKey, std::function<void()> function)
{
	std::pair<unsigned int, std::function<void()>> values;
	values.first = nKey;
	values.second = function;
	m_umKeyBindings.insert(values);
}

void CKeyManager::AddMouseAxisBinding(CKeyManager::EMouseAxis eAxis, std::function<void(double)> function)
{
	std::pair<EMouseAxis, std::function<void(double)>> values;
	values.first = eAxis;
	values.second = function;
	m_umMouseAxisBindings.insert(values);
}

void CKeyManager::RemoveKeyBinding(unsigned int nKey)
{
	m_umKeyBindings.erase(nKey);
}

void CKeyManager::RemoveMouseAxisBinding(EMouseAxis eAxis)
{
	m_umMouseAxisBindings.erase(eAxis);
}

void CKeyManager::KeyHasPressed(unsigned int nKey)
{
	for (std::set<CKeyManager*>::iterator keyManagerIT = setGlobalObjects.begin(); keyManagerIT != setGlobalObjects.end(); ++keyManagerIT)
	{
		std::unordered_map<unsigned int, std::function<void()>>::iterator keyBindingIT = (*keyManagerIT)->m_umKeyBindings.find(nKey);
		if(keyBindingIT != (*keyManagerIT)->m_umKeyBindings.end()) //this object has bound this key!
		{ 
			keyBindingIT->second();
		}
	}
}

void CKeyManager::MouseHasMoved(EMouseAxis eAxis, double dDirection)
{
	for (std::set<CKeyManager*>::iterator keyManagerIT = setGlobalObjects.begin(); keyManagerIT != setGlobalObjects.end(); ++keyManagerIT)
	{
		std::unordered_map<EMouseAxis, std::function<void(double)>>::iterator mouseAxisBindingIT = (*keyManagerIT)->m_umMouseAxisBindings.find(eAxis);
		if (mouseAxisBindingIT != (*keyManagerIT)->m_umMouseAxisBindings.end()) //this object has bound this key!
		{
			mouseAxisBindingIT->second(dDirection);
		}
	}
}


