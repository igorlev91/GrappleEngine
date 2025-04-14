#include "TypeInitializer.h"

namespace Grapple
{
    TypeInitializer::TypeInitializer(std::string_view typeName, size_t size, DestructorFunction destructor, const std::initializer_list<FieldData>& fields)
        : TypeName(typeName), Size(size), Destructor(destructor), SerializedFields(fields)
    {
        GetInitializers().push_back(this);
    }

    TypeInitializer::~TypeInitializer()
    {
        auto& initializers = GetInitializers();
        for (size_t i = 0; i < initializers.size(); i++)
        {
            if (initializers[i] == this)
            {
                initializers.erase(initializers.begin() + i);
                break;
            }
        }
    }

    std::vector<TypeInitializer*>& TypeInitializer::GetInitializers()
    {
        static std::vector<TypeInitializer*> s_Initializers;
        return s_Initializers;
    }
}