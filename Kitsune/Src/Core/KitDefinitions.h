#pragma once

#if __cplusplus >= 201703L
#define KIT_NODISCARD [[nodiscard]]
#else
#define KIT_NODISCARD
#endif