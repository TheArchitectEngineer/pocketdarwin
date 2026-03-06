/*
 * Fallback dynarmic wrapper when vendor/dynarmic is unavailable.
 * This only satisfies linking/build and does not provide CPU emulation.
 */

#include <cstdint>
#include <cstdlib>
#include <utility>

struct touchHLE_DynarmicWrapper {
  std::uint32_t regs[16]{};
  std::uint32_t cpsr{};
};

struct Dynarmic_A32_Context {
  std::uint32_t regs[16]{};
  std::uint32_t cpsr{};
};

extern "C" {

touchHLE_DynarmicWrapper *touchHLE_DynarmicWrapper_new(void *, size_t) {
  return new touchHLE_DynarmicWrapper();
}

void touchHLE_DynarmicWrapper_delete(touchHLE_DynarmicWrapper *cpu) { delete cpu; }

const std::uint32_t *
touchHLE_DynarmicWrapper_regs_const(const touchHLE_DynarmicWrapper *cpu) {
  return cpu->regs;
}

std::uint32_t *touchHLE_DynarmicWrapper_regs_mut(touchHLE_DynarmicWrapper *cpu) {
  return cpu->regs;
}

std::uint32_t touchHLE_DynarmicWrapper_cpsr(const touchHLE_DynarmicWrapper *cpu) {
  return cpu->cpsr;
}

void touchHLE_DynarmicWrapper_set_cpsr(touchHLE_DynarmicWrapper *cpu, std::uint32_t cpsr) {
  cpu->cpsr = cpsr;
}

void touchHLE_DynarmicWrapper_swap_context(touchHLE_DynarmicWrapper *cpu,
                                           Dynarmic_A32_Context *context) {
  for (int i = 0; i < 16; i++) {
    std::swap(cpu->regs[i], context->regs[i]);
  }
  std::swap(cpu->cpsr, context->cpsr);
}

void touchHLE_DynarmicWrapper_invalidate_cache_range(touchHLE_DynarmicWrapper *, std::uint32_t,
                                                      std::uint32_t) {}

std::int32_t touchHLE_DynarmicWrapper_run_or_step(touchHLE_DynarmicWrapper *, void *,
                                                  std::uint64_t *) {
  // Map to CpuError::UndefinedInstruction in Rust.
  return -3;
}

bool touchHLE_DynarmicWrapper_is_stub(const touchHLE_DynarmicWrapper *) {
  return true;
}

Dynarmic_A32_Context *touchHLE_DynarmicWrapper_Context_new() { return new Dynarmic_A32_Context(); }

void touchHLE_DynarmicWrapper_Context_delete(Dynarmic_A32_Context *context) { delete context; }

} // extern "C"
