#pragma once
#ifndef COMPONENT_H

class Component
{
public:
	Component();
	~Component();

	virtual void init();
	virtual void input();
	virtual void update();
	virtual void render();
};

#endif // !COMPONENT_H
