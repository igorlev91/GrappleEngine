#pragma once

#include "GrappleCore/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/format.h>
#include <glm/glm.hpp>

namespace Grapple
{
    class GrappleCORE_API Log
    {
    public:
        static void Initialize();

        static Ref<spdlog::logger> GetCoreLogger();
        static Ref<spdlog::logger> GetClientLogger();
    };
}

#define VECTOR_FORMATTER(vectorType, formatString, ...)                        \
    template<>                                                                 \
    struct fmt::formatter<vectorType> : fmt::formatter<vectorType::value_type> \
    {                                                                          \
        auto format(vectorType vector, format_context& ctx)                    \
        {                                                                      \
            return format_to(ctx.out(), formatString, __VA_ARGS__);            \
        }                                                                      \
    };

VECTOR_FORMATTER(glm::vec2, "Vector2({}; {})", vector.x, vector.y);
VECTOR_FORMATTER(glm::ivec2, "Vector2Int({}; {})", vector.x, vector.y);
VECTOR_FORMATTER(glm::uvec2, "Vector2UInt({}; {})", vector.x, vector.y);

VECTOR_FORMATTER(glm::vec3, "Vector3({}; {}; {})", vector.x, vector.y, vector.z);
VECTOR_FORMATTER(glm::ivec3, "Vector3Int({}; {}; {})", vector.x, vector.y, vector.z);
VECTOR_FORMATTER(glm::uvec3, "Vector3UInt({}; {}; {})", vector.x, vector.y, vector.z);

VECTOR_FORMATTER(glm::vec4, "Vector4({}; {}; {}; {})", vector.x, vector.y, vector.z, vector.w);
VECTOR_FORMATTER(glm::ivec4, "Vector4Int({}; {}; {}; {})", vector.x, vector.y, vector.z, vector.w);
VECTOR_FORMATTER(glm::uvec4, "Vector4UInt({}; {}; {}; {})", vector.x, vector.y, vector.z, vector.w);

#define Grapple_CORE_ERROR(...) Grapple::Log::GetCoreLogger()->error(__VA_ARGS__)
#define Grapple_CORE_WARN(...) Grapple::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define Grapple_CORE_INFO(...) Grapple::Log::GetCoreLogger()->info(__VA_ARGS__)
#define Grapple_CORE_TRACE(...) Grapple::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define Grapple_CORE_CRITICAL(...) Grapple::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define Grapple_ERROR(...) Grapple::Log::GetClientLogger()->error(__VA_ARGS__)
#define Grapple_WARN(...) Grapple::Log::GetClientLogger()->warn(__VA_ARGS__)
#define Grapple_INFO(...) Grapple::Log::GetClientLogger()->info(__VA_ARGS__)
#define Grapple_TRACE(...) Grapple::Log::GetClientLogger()->trace(__VA_ARGS__)
#define Grapple_CRITICAL(...) Grapple::Log::GetClientLogger()->critical(__VA_ARGS__)
