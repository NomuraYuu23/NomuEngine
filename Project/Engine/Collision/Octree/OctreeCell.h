#pragma once
#include <cstdint>
#include <list>
#include "../../Collider/ColliderShape.h"

class OctreeCell
{

private:

	// objリスト
	std::list<ColliderShape*> objList_;

	// 下にある空間
	std::list<OctreeCell*> cellList_;

public:

	///
	void Initialize(std::list<OctreeCell*>& cellList);

	void ObjListClear();

	void ObjListRegister(ColliderShape* octreeObject);

	std::list<ColliderShape*>& GetObjList() { return objList_; }

	std::list<OctreeCell*>& GetCellList() { return cellList_; }

};

