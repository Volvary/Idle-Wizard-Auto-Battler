#include "pch.h"
#include "ObjectPooler.h"
#include <typeinfo>
#include "Parchment.h"

static ObjectPooler* Singleton = nullptr;

ObjectPooler::ObjectPooler()
{
}


ObjectPooler::~ObjectPooler()
{
	/*while (ActiveObjects.size() > 0) {
		delete ActiveObjects.back();
		ActiveObjects.pop_back();
	}
	while (InactiveObjects.size() > 0) {
		delete InactiveObjects.back();
		InactiveObjects.pop_back();
	}*/
}

void ObjectPooler::DestroySingleton()
{
	if (Singleton) {
		delete Singleton;
	}
}

const ObjectPooler* ObjectPooler::Get()
{
	if (Singleton) {
		return Singleton;
	}
	else {
		Singleton = new ObjectPooler();
		return Singleton;
	}
}

Parchment* ObjectPooler::NewParchment(type_info InTypeid)
{
	Parchment* Out = nullptr;

	for (Parchment* Parch : InactiveParchments) {
		if (typeid(Parch) == InTypeid) {

		}
	}

	return Out;
}