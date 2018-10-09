#include "node.h"
#include "model.h"

namespace MODEL
{
 
	AABB node::get_AABB(model* mod)
	{
		AABB aabb;
		aabb.zero();
		for (auto itr = m_param.begin(); itr != m_param.end(); itr++)
		{
			mesh* mesh = mod->get_meshs(itr->meshindex);
			if (!mesh) {
				BOOST_LOG_TRIVIAL(error) << "node::get_AABB: mesh no find: "<< itr->meshindex;
				continue;
			}
			aabb = AABB::combine(aabb, mesh->get_AABB());
		}

		std::vector<std::unique_ptr<node>>& child = childs();
		for (auto itr = child.begin(); itr != child.end(); itr++)
		{
			aabb = AABB::combine(aabb, (*itr)->get_AABB(mod));
		}
		return aabb;
	}

}