<div align="center">
  <img src="logo.png" alt="PocketDarwin logo" width="180" />
  <h1>PocketDarwin</h1>
  <p><em>"I’m the crazy guy who’s gonna clone iOS!" — OakyMacintosh</em></p>
</div>

<div align="center">
  <img src="iphone-sdk-4.png" alt="PocketDarwin iPhone SDK preview" width="760" />
</div>

PocketDarwin is an experimental, open-source effort to bring a Darwin/iPhone-OS-style software stack to non-Apple hardware (especially Android devices).

In plain words: this repo is where people tinker with kernels, frameworks, apps, and SDK tooling to recreate the classic iPhone-era development/runtime experience.

---

## ✨ What is this project trying to do?

PocketDarwin is aiming to assemble a full "mini ecosystem":

- **System layer**: boot and low-level platform bits.
- **Framework layer**: Objective-C-friendly frameworks and glue.
- **Application layer**: SpringBoard-like UI and bundled apps.
- **Developer layer**: SDK/toolchain pieces for building software against PocketDarwin.
- **Android bridge work**: experiments that let parts of this stack run in Android environments.

This is an active hacking project, not a finished product.

---

## 🧭 Repo map (where things live)

If you’re new, start here:

- [`System/`](System/) → boot/platform code and system apps (including SpringBoard work)
- [`Frameworks/`](Frameworks/) → core frameworks + build helpers (`MakeKit`, `TerminalKit`, etc.)
- [`Developer/`](Developer/) → SDKs, toolchains, and scripts for developer workflows
- [`BuildSystem/`](BuildSystem/) → build orchestration pieces
- [`Android/`](Android/) → Android-side experiments/integration projects
- [`Tools/`](Tools/) → utility tools used by the project

---

## 🚀 Quick start (pick your adventure)

### 1) I just want to understand the build system
Read:

- [`BuildSystem/README.md`](BuildSystem/README.md)

### 2) I want SDK/toolchain details
Read:

- [`Developer/SDKs/README.md`](Developer/SDKs/README.md)
- [`Developer/common-3.0-sdk/README.md`](Developer/common-3.0-sdk/README.md)

### 3) I want to poke around SpringBoard/app code
Start in:

- [`System/Applications/springboard/`](System/Applications/springboard/)

### 4) I want to hack utilities
Start in:

- [`Tools/pdpack/`](Tools/pdpack/)

---

## 📦 Current status

PocketDarwin is **work-in-progress** and heavily experimental.
Expect rough edges, incomplete components, and frequent changes.

> [!NOTE]
> PocketDarwin does not aim to provide generic prebuilt Linux "interludes" for every setup.
> You should expect source-based setup/build workflows.

> [!TIP]
> Join the community Discord: https://discord.gg/bthwXwh8vP

---

## 📱 Supported hardware (today)

- **harpia** (Moto G4 Play, 2016, Linux kernel 3.10) — official development device
- Potentially other **aarch64 Android devices** with publicly available kernel source trees

---

## 🤝 Contributing

Contributions are welcome — code, docs, testing, bug reports, and wild ideas.

A good first contribution is improving docs for one subdirectory you touched.
If you had to reverse-engineer how something works, write it down for the next person.

---

## ⚖️ License

See [`LICENSE`](LICENSE) for project licensing.
