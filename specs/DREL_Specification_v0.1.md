# DREL — Dynamic RISC Execution Layer
**Draft v1 (spec skeleton)**  
**Author:** Milan Lakatoš Petrović (HeritEon)  
**License:** CC BY 4.0  
**Status:** Draft (for public discussion)

---

## 0) Principle
**CISC = control-plane** (boot, service, safety).  
**DREL = data-plane** (public, fixed 32-bit RISC-like ISA; in silicon a thin translator DREL→internal µops).  
DREL runs only when explicitly enabled by privileged software and can always exit to CISC synchronously.

---

## 1) ISA Layer (stability)
- The vendor does not expose internal µops. A stable, public DREL ISA is defined (fixed 32-bit, load/store).
- A thin hardware translator maps DREL instructions to the core’s internal representation per generation.
- DREL binary stability does not depend on the internal µops format.

---

## 2) Memory Model and Atomics
- Memory model: x86 **TSO** semantics.
- Barriers: **FENCE.r**, **FENCE.w**, **FENCE.rw** (acquire/release/seq-cst usage is toolchain-defined).
- Atomics: **XCHG, CMPXCHG, FETCHADD** with TSO semantics; no **LOCK** prefix; explicit opcodes.

---

## 3) Register File / ABI (summary)
- **GPR:** R0–R31, 64-bit (R0 may be ZERO). **VEC:** V0–V31, 256-bit; scalar alias for FP64/FP32.
- **Predicates:** P0–P7; P0 typically TRUE. No implicit FLAGS (C/Z/S/O).
- **Call conv (example):** args R1–R6, retval R1; callee-saved R8–R15, V16–V31; others caller-saved.
- **Trampolines RISC↔CISC:** pass 6 args in regs; spill excess on stack; clean V/FLAGS state.

---

## 4) Front-End Variants
- **v1:** RISC-tag in L1i (shares path with CISC); **v2:** dedicated **L1R** (cleaner FE gating).
- In DREL mode, fetch originates from **.drel/.xrisc** pages (R-tagged or L1R). No microcode/prefix path.

---

## 5) Interrupts, Debug, CET/IBT, Security
- **NMI/PMI/INT ⇒ default EXIT_DREL** (precise trap). Maskable INTs configurable via MSR mask.
- **Debug/PMU:** DREL-aware events (**DREL.\***), HW breakpoints, authorized single-step, LBR snapshots.
- **CET/CFI:** DREL indirect branches require **LPAD** (branch token). On failure ⇒ **EXIT_DREL** before jumping.
- **Paging:** new PTE bit **XRISC**; strict **W^X**. `.drel/.xrisc` pages are execute-only; no self-modifying code.
- **SIGNED_SEG:** header signature + hash; verified by CISC loader before mapping RX.

---

## 6) SMT and RMF Granularity
- Default **PER_THREAD** (safe when FE is shared). **PER_CORE** advisory when µarch can fully gate both threads.

---

## 7) Performance (realistic expectations)
- Wins in tight loops (ALU+LD/ST), RT slices, crypto/AI tiles, and wherever decode/microcode vanish.
- Target heuristics: energy −10–15% core-level, IPC +20–30% in ideal loops (workload-dependent).

---

## 8) PMU/Telemetry and Kill-Switch
- New PMU domain: **DREL.FETCH_STALL, DREL.RENAME_STALL, DREL.ISSUE, DREL.RETIRE, DREL.L1R_MISS, DREL.EXIT_COUNT…**
- **Kill:** MSR **CLEAR** + **FENCE** ⇒ guaranteed **EXIT_DREL** within **N** cycles (document N).

---

## 9) Binary Format and Toolchain
- ELF/PE: new section/segment (`.drel` / `.xrisc`), 32-bit instruction alignment.
- Fat binary: classic `.text` (CISC) + `.drel`; loader selects based on CPUID+policy.
- Toolchain stub: `clang/LLVM -target drel -mabi=drel`; `xrisc-objdump`; TableGen printer/disasm.

---

## 10) Minimal ISA Skeleton (indicative)
~~~txt
ALU:    ADD, SUB, AND, OR, XOR, NOT, SLL, SRL, SRA, MUL, DIV
LD/ST:  LD.{B/W/D/Q}  Rd, [Rb + simm16]
        ST.{B/W/D/Q}  Rs, [Rb + simm16]
        LDX/STX       [Rb + Rc]  (no scale; use SLL for scaling)
BR:     BEQ/BNE/BLT/BGE  rel
        JMP rel, JAL rel, JALR Rb
PRED:   SETP.eq/ne/lt/ge  Pk, Ra, Rb|imm
        CMOV/SEL         Rd, Ra, Rb, Pmask
VEC:    VADD, VSUB, VMUL, VFMA; VLD/VST (aligned); optional P-mask predicate
SYNC:   FENCE.r, FENCE.w, FENCE.rw
ATOMIC: XCHG, CMPXCHG, FETCHADD   (TSO semantics)
CFI:    LPAD (legal indirect-branch target); token checked when CET enabled
META:   DREL_EXIT, DREL_DBELL (doorbell MMIO), FENCE.SPEC
~~~

---

## 11) OS / Hypervisor Integration
- Syscalls from DREL: trampoline to CISC stub (DREL-sysenter optional later).
- Scheduler: tag threads as ‘drel-friendly’. Migration rules between P/E cores unchanged; EXIT on INT.
- VM: vCPU capability; host decides RMF_EN per-vCPU; EXIT_REASON visible in vMSR.

---

## 12) CPUID / MSR and Exit Reasons
~~~txt
CPUID (example): EAX=07H, ECX=03H
  EAX[0]  DREL_SUPPORTED
  EAX[1]  DREL_XSAVE_REQUIRED
  EAX[2]  DREL_CET_TOKEN

MSR IA32_DREL_CTL (0xC0000080)
  Bit0  ENABLE
  Bit1  XSAVE_EXT
  Bit2  PER_THREAD
  Bit3  SIGNED_SEG
  Bit4  INT_EXIT_EN
  Bit5  CET_TOKEN_EN
  Bit6  ACTIVE  (RO)
  Bit7  CLEAR   (WO)

MSR IA32_DREL_INT_MASK (0xC0000081): vector mask causing auto-EXIT (NMI/PMI always EXIT).
MSR IA32_DREL_STATUS   (0xC0000082): EXIT_REASON, DREL_RIP_SNAPSHOT, impl-def fields.

EXIT_REASON codes:
  0x01 INT/NMI/PMI
  0x02 CET_TOKEN_FAULT
  0x03 SIG_VERIFY_FAIL
  0x04 WX_PAGE_FAULT
  0x05 DEBUG_SINGLESTEP
  0x06 ILLEGAL_ENCODING
  0x07 MSR_CLEAR
  0x08 TIME_SLICE
  0x09 PERF_BUDGET
  0x0A PRIV_VIOLATION
  0x0B OTHER
~~~

---

## 13) CET/CFI Rule (forward-edge)
- Every indirect branch (**JALR/JMP Rb**) must land on an **LPAD**; otherwise **EXIT_REASON=CET_TOKEN_FAULT**.
- **RET** obeys shadow-stack when enabled; mismatch ⇒ EXIT to CISC with precise frame.

---

## 14) SMT and Scheduler Notes
- **PER_THREAD** default to avoid FE contention; **PER_CORE** is advisory when FE can be fully gated per core.
- Migration between cores: **EXIT_DREL** on INT; re-entry policy-owned by OS.

---

## 15) Virtualization (KVM/Hyper-V/Xen)
- CPUID virtualization: hypervisor exposes/hides DREL per policy.
- vMSR passthrough or emulation for `IA32_DREL_*`; optional **VMEXIT on ENTER** for instrumentation.
- **XSAVE/XRSTOR:** reuse existing save areas (no extra DREL state beyond P/V registers).

---

## 16) Conformance (DACS)
- **Memory model litmus (TSO):** SB visibility, atomicity of XCHG/CMPXCHG/FETCHADD under MESI.
- **Decoder robustness:** illegal encodings ⇒ **ILLEGAL_ENCODING** without hang.
- **CFI:** indirect to non-LPAD ⇒ **CET_TOKEN_FAULT**; shadow-stack RET mismatch ⇒ CFI violation.
- **EXIT determinism:** **CLEAR+FENCE** ⇒ guaranteed CISC in ≤ **N** cycles (document value).

---

## 17) Toolchain Hooks
~~~c
/* Intrinsics */
void  __drel_enter(void* entry);
void  __drel_exit(void);
void  __drel_dbell(void* mmio, unsigned long long v);
void  __drel_fence_spec(void);
_Bool __drel_is_active(void);
~~~

---

## 18) DREL/.XRISC Section Header
~~~c
#pragma pack(push, 1)
typedef struct DREL_Header {
    uint32_t magic;        // 'DREL'
    uint16_t ver_major;
    uint16_t ver_minor;
    uint32_t flags;        // bit0 SIGNED, bit1 CET_TOKEN, bit2 PIC_ONLY ...
    uint64_t code_off;
    uint64_t code_len;
    uint64_t entry_off;
    uint64_t sig_off;
    uint32_t sig_len;
    uint8_t  hash[32];     // SHA-256 over code region
} DREL_Header;
#pragma pack(pop)
~~~

---

## 19) Firmware/Enablement & Governance
- **BIOS/ACPI** exposure (FADT/DSDT markers). DREL disabled by default; OS must opt-in.
- Microcode updates must not change public DREL semantics (only internal mapping).
- **Spec versioning:** SemVer; feature levels; reference emulator (QEMU-mode) for CI.
