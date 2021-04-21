#pragma once
#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <chrono>

namespace Application
{
	class Randomizer
	{
	public:
		Randomizer()
		{
			seed = std::chrono::system_clock::now().time_since_epoch().count();
		}

		unsigned int randomInt()
		{
			seed ^= (seed << 5);
			seed ^= (seed >> 13);
			seed ^= (seed << 6);
			return seed;
		}

	private:
		unsigned int seed;
	};
}

#endif // !RANDOMIZER_H