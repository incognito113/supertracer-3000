// Flag definitions to fix Metal build issues
#if __has_include(<Metal/Metal.hpp>)

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

#endif