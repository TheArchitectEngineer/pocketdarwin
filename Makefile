SHELL := /bin/sh
.DEFAULT_GOAL := help

ROOT := $(abspath .)
BUILD_DIR := $(ROOT)/.build

RUST_PROFILE ?= debug
RUST_CARGO_FLAGS :=
ifeq ($(RUST_PROFILE),release)
RUST_CARGO_FLAGS += --release
endif

ANDROID_DIR := Android/SBAndroid
ANDROID_GRADLE := $(ANDROID_DIR)/gradlew
ANDROID_TASK ?= assembleDebug
ANDROID_TEST_TASK ?= testDebugUnitTest

SYSBUNDLE_DIR := System/core/SysBundle
PDPACK_DIR := Tools/pdpack
ISTDLIB_DIR := istdlib
SYSTEM_DIR := System
SYSTEM_TARGET ?= harpia
SYSTEM_BUNDLE_FLAGS ?=

.PHONY: help
help:
	@printf '%s\n' \
		'PocketDarwin build system' \
		'' \
		'Usage:' \
		'  make <target>' \
		'' \
		'Top-level targets:' \
		'  build                Build Android + Rust components' \
		'  build-all            Build everything, including System target' \
		'  check                Run static checks (Rust + Android lint if available)' \
		'  test                 Run unit tests where available' \
		'  clean                Clean all build artifacts' \
		'' \
		'Component targets:' \
		'  android              Build Android SpringBoard app (ANDROID_TASK=<task>)' \
		'  android-lint         Run Android lint' \
		'  android-test         Run Android unit tests (ANDROID_TEST_TASK=<task>)' \
		'  sysbundle            Build SysBundle crate (RUST_PROFILE=debug|release)' \
		'  pdpack               Build pdpack crate (RUST_PROFILE=debug|release)' \
		'  istdlib              Build istdlib crate (RUST_PROFILE=debug|release)' \
		'  rust                 Build all Rust crates' \
		'  rust-check           cargo check for Rust crates' \
		'  rust-test            cargo test for Rust crates' \
		'  system               Build System target (SYSTEM_TARGET=<name>, default harpia)' \
		'  system-bundle        Build and package .System bundle (uses BuildSystem/pdbuild.swift)'

.PHONY: build
build: android rust

.PHONY: build-all
build-all: build system

.PHONY: check
check: rust-check android-lint

.PHONY: test
test: rust-test android-test

.PHONY: clean
clean: android-clean rust-clean system-clean
	@rm -rf $(BUILD_DIR)

.PHONY: bootstrap
bootstrap:
	@mkdir -p $(BUILD_DIR)
	@if [ ! -x "$(ANDROID_GRADLE)" ]; then \
		echo "Missing Gradle wrapper: $(ANDROID_GRADLE)"; \
		echo "Run inside repository root with Android project checked out."; \
		exit 1; \
	fi

.PHONY: android
android: bootstrap
	@cd $(ANDROID_DIR) && ./gradlew $(ANDROID_TASK)

.PHONY: android-lint
android-lint: bootstrap
	@cd $(ANDROID_DIR) && ./gradlew lint

.PHONY: android-test
android-test: bootstrap
	@cd $(ANDROID_DIR) && ./gradlew $(ANDROID_TEST_TASK)

.PHONY: android-clean
android-clean:
	@cd $(ANDROID_DIR) && ./gradlew clean || true

.PHONY: sysbundle
sysbundle:
	@cargo build --manifest-path $(SYSBUNDLE_DIR)/Cargo.toml $(RUST_CARGO_FLAGS)

.PHONY: pdpack
pdpack:
	@cargo build --manifest-path $(PDPACK_DIR)/Cargo.toml $(RUST_CARGO_FLAGS)

.PHONY: istdlib
istdlib:
	@cargo build --manifest-path $(ISTDLIB_DIR)/Cargo.toml $(RUST_CARGO_FLAGS)

.PHONY: rust
rust: sysbundle pdpack istdlib

.PHONY: rust-check
rust-check:
	@cargo check --manifest-path $(SYSBUNDLE_DIR)/Cargo.toml
	@cargo check --manifest-path $(PDPACK_DIR)/Cargo.toml
	@cargo check --manifest-path $(ISTDLIB_DIR)/Cargo.toml

.PHONY: rust-test
rust-test:
	@cargo test --manifest-path $(SYSBUNDLE_DIR)/Cargo.toml
	@cargo test --manifest-path $(PDPACK_DIR)/Cargo.toml
	@cargo test --manifest-path $(ISTDLIB_DIR)/Cargo.toml

.PHONY: rust-clean
rust-clean:
	@cargo clean --manifest-path $(SYSBUNDLE_DIR)/Cargo.toml
	@cargo clean --manifest-path $(PDPACK_DIR)/Cargo.toml
	@cargo clean --manifest-path $(ISTDLIB_DIR)/Cargo.toml

.PHONY: system
system:
	@$(MAKE) -C $(SYSTEM_DIR) $(SYSTEM_TARGET)


.PHONY: system-bundle
system-bundle:
	@./BuildSystem/pdbuild.swift system-bundle $(SYSTEM_BUNDLE_FLAGS)

.PHONY: system-clean
system-clean:
	@$(MAKE) -C $(SYSTEM_DIR) clean || true
