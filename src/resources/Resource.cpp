#include "Resource.h"

namespace LGE {

Resource::Resource(ResourceType type, const std::string& name)
    : m_Type(type), m_Name(name), m_IsLoaded(false), m_RefCount(0) {
}

}
