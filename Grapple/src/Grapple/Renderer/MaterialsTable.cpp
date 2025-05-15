#include "MaterialsTable.h"

namespace Grapple
{
	Grapple_IMPL_ASSET(MaterialsTable);
	Grapple_SERIALIZABLE_IMPL(MaterialsTable);

	MaterialsTable::MaterialsTable()
		: Asset(AssetType::MaterialsTable)
	{
	}
}
