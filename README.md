# DREL — Dynamic RISC Execution Layer

![License](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)
![Status](https://img.shields.io/badge/Status-Architecture%20RFC-blue.svg)
![Platform](https://img.shields.io/badge/Platform-x86__64%20%2F%20Heterogeneous-orange.svg)

> **"Unlocking the silicon. Empowering the user."**

---

## Overview

**DREL (Dynamic RISC Execution Layer)** is an open architectural specification designed to bridge the gap between legacy x86 CISC compatibility and the future of high-performance, heterogeneous computing.

Modern x86 processors pay a significant "legacy tax" in power and silicon area to decode complex variable-length instructions into internal micro-operations. DREL proposes a **Dual-Mode architecture** that allows software to bypass the legacy decoder and execute optimized, fixed-width RISC instructions directly on the core's execution units, without breaking OS or ABI compatibility.

This project is not just about CPU performance—it is a strategy to **democratize AI hardware**, enabling consumer PC components to compete with proprietary cloud silos through efficient, zero-copy heterogeneous compute.

---

## Repository Structure

This repository contains the defining documents of the DREL standard:

* **[`/docs/DREL_Architecture_Reference_Manual.pdf`](#)** The primary **Whitepaper**. [cite_start]Covers the high-level philosophy, the "Legacy Tax" problem, the VRAM Wall solution, and the strategic vision for a post-CUDA compute world[cite: 529].
    
* **[`/specs/DREL_Specification_v0.1.md`](#)** The engineering **Technical Specification**. Contains the "hard" implementation details derived from the draft, including:
    * [cite_start]**ISA Encoding:** Fixed 32-bit Load/Store architecture[cite: 534].
    * [cite_start]**MSR Definitions:** `IA32_DREL_CTL` (0xC0000080) & `EXIT_REASON` codes[cite: 603, 848].
    * [cite_start]**ABI & Register File:** R0-R31, Scalable Vectors (VEC), and Hardware Aliasing[cite: 557, 564].
    * [cite_start]**Binary Format:** `.drel` section definitions for PE/ELF[cite: 638, 867].

---

## Key Technical Pillars

### 1. Dual-Mode Operation
The CPU operates in **Legacy Mode** (default) for full x86 compatibility. [cite_start]Applications opt-in to **DREL Mode** via the `DREL_ENTER` instruction, switching the fetch unit to a high-speed, 4-byte aligned RISC path[cite: 593, 606].

### 2. Native RISC "Bypass"
DREL instructions map 1:1 to internal execution units. By removing the CISC decoder from the hot path, DREL aims for:
* **Lower Latency:** Elimination of length-decoding stages.
* **Higher IPC:** More predictable pipeline filling.
* [cite_start]**Scalable Vectors:** Support for variable-length vector operations (AI/HPC) independent of the physical register width[cite: 536].

### 3. The GPU Bridge & Democratization
DREL treats the CPU as a direct **Command Processor** for the GPU.
* [cite_start]**Zero-Copy:** Unified addressing model allows the CPU (in DREL mode) and GPU to share pointers without marshalling[cite: 700].
* [cite_start]**Smart Streaming:** Enables "Virtual Unified Memory" on standard PCs, allowing large LLMs (>24GB) to run on consumer hardware by efficiently streaming layers from system RAM to VRAM[cite: 712].
* [cite_start]**Open Compute:** Targets SPIR-V/Vulkan directly, bypassing proprietary driver stacks like CUDA[cite: 721].

---

## Security & Virtualization

DREL is designed with modern threat models in mind:
* **W^X Enforcement:** The `.drel` section is strictly Execute-Only. [cite_start]No Self-Modifying Code (SMC) is allowed[cite: 643].
* [cite_start]**CFI (Control Flow Integrity):** Hardware-enforced Shadow Stack and `LPAD` (Landing Pad) instructions prevent ROP/JOP attacks[cite: 663].
* [cite_start]**Virtualization Native:** DREL state aliases physical x86 registers, allowing Hypervisors (KVM/Hyper-V) to support DREL guests with zero modification to `XSAVE/XRSTOR` logic[cite: 683].

---

## Roadmap

* [x] **Phase 1:** Specification & "Prior Art" Publication (Current).
* [ ] **Phase 2:** Emulation (QEMU fork with `DREL_ENTER` support).
* [ ] **Phase 3:** FPGA Soft-core Implementation (RISC-V based proof-of-concept).
* [ ] **Phase 4:** Silicon Integration proposal.

---

## License & Citation

This work is released under the **Creative Commons Attribution 4.0 International (CC BY 4.0)** license. It is intended as **Prior Art** to encourage innovation while preventing patent monopolization of the transitionary architecture concept.

### How to Cite
If you use, implement, or reference this architecture, please cite:

> **"Based on the DREL Architecture concept designed by Milan Lakatos Petrović (HeritEon), originally published in 2025."**

---

*Open an issue to discuss specific ISA opcodes or system integration details.*
