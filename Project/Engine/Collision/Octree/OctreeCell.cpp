#include "OctreeCell.h"

void OctreeCell::Initialize(std::list<OctreeCell*>& cellList)
{

	cellList_ = cellList;

}

void OctreeCell::ObjListClear()
{

	objList_.clear();

}

void OctreeCell::ObjListRegister(ColliderShape* octreeObject)
{

	objList_.push_back(octreeObject);

}
