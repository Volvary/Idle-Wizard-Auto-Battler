#pragma once
#include <vector>

struct Parchment;

class ObjectPooler
{
public:
	ObjectPooler();
	~ObjectPooler();

	static void DestroySingleton();
	static const ObjectPooler* Get();

	Parchment* NewParchment(type_info InTypeid);

private:

	std::vector<Parchment*> InactiveParchments;
	std::vector<Parchment*> ActiveParchments;
};