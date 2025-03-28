#pragma once
#ifndef STACK_REALISATION_H
#define STACK_REALISATION_H
#define MAX_STACK_SIZE 50

class custom_stack
{
public:
	custom_stack()
	{
		top = -1;
		stack_arr = new uint16_t[MAX_STACK_SIZE];
	}
	~custom_stack()
	{
		delete[] stack_arr;
	}
	void push(uint16_t val)
	{
		if (top >= MAX_STACK_SIZE - 1) return;
		top++;
		stack_arr[top] = val;
	}
	int pop()
	{
		if (top < 0) return 0xFFFF;
		uint16_t bufferVal = top;
		top--;
		return stack_arr[bufferVal];
	}
	uint16_t peek()
	{
		if (top < 0) return 0xFFFF;
		return stack_arr[top];
	}
	bool isEmpty()
	{
		return (top < 0);
	}
private:
	int16_t top;
	uint16_t* stack_arr;

};
#endif // !STACK_REALISATION_H
