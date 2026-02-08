#!/usr/bin/env swift
import Foundation

struct Shell {
    @discardableResult
    static func run(_ command: String, _ args: [String] = [], env: [String: String] = [:]) throws -> Int32 {
        let process = Process()
        process.executableURL = URL(fileURLWithPath: command)
        process.arguments = args

        var environment = ProcessInfo.processInfo.environment
        for (k, v) in env {
            environment[k] = v
        }
        process.environment = environment

        let pipe = Pipe()
        process.standardOutput = pipe
        process.standardError = pipe

        try process.run()

        let handle = pipe.fileHandleForReading
        while true {
            let data = handle.availableData
            if data.isEmpty { break }
            if let text = String(data: data, encoding: .utf8) {
                print(text, terminator: "")
            }
        }

        process.waitUntilExit()
        return process.terminationStatus
    }

    static func which(_ name: String) -> String? {
        let paths = ProcessInfo.processInfo.environment["PATH"] ?? ""
        for path in paths.split(separator: ":") {
            let candidate = "\(path)/\(name)"
            if FileManager.default.isExecutableFile(atPath: candidate) {
                return candidate
            }
        }
        return nil
    }
}

struct Paths {
    static let root: URL = {
        let cwd = FileManager.default.currentDirectoryPath
        return URL(fileURLWithPath: cwd)
    }()

    static let interludeDir = root.appendingPathComponent("System/Boot/interlude")
    static let sdkScripts = root.appendingPathComponent("Developer/SDKs/scripts")
    static let installerProject = root.appendingPathComponent("Platform/macOS/Install PocketDarwin/Install PocketDarwin.xcodeproj")
    static let dmgScript = root.appendingPathComponent("scripts/make_dmg.sh")
}

func usage() {
    print("""
PocketDarwin build system (standalone Swift)

Usage:
  pdbuild <command> [options]

Commands:
  interlude            Build the interlude loader (Linux)
  sdk-validate         Validate SDK sysroot
  sdk-populate         Populate SDK sysroot (requires --headers and --libs)
  installer-build      Build macOS installer app (uses xcodebuild on macOS)
  installer-dmg        Build macOS installer DMG (uses scripts/make_dmg.sh)
  all                  Run interlude + sdk-validate (and installer on macOS)

Options:
  --headers <path>     Headers directory for sdk-populate
  --libs <path>        Libs directory for sdk-populate
  --derived <path>     DerivedData path for installer build
  --config <Debug|Release>  Build configuration for installer (default Release)
""")
}

func requireDir(_ path: String) throws {
    var isDir: ObjCBool = false
    if !FileManager.default.fileExists(atPath: path, isDirectory: &isDir) || !isDir.boolValue {
        throw NSError(domain: "pdbuild", code: 1, userInfo: [NSLocalizedDescriptionKey: "Missing directory: \(path)"])
    }
}

func runInterlude() throws {
    let make = Shell.which("make") ?? "/usr/bin/make"
    let status = try Shell.run(make, ["-C", Paths.interludeDir.path])
    if status != 0 { throw NSError(domain: "pdbuild", code: 2, userInfo: [NSLocalizedDescriptionKey: "interlude build failed"]) }
}

func sdkValidate() throws {
    let script = Paths.sdkScripts.appendingPathComponent("validate_sysroot.sh")
    let status = try Shell.run(script.path)
    if status != 0 { throw NSError(domain: "pdbuild", code: 3, userInfo: [NSLocalizedDescriptionKey: "sdk validation failed"]) }
}

func sdkPopulate(headers: String, libs: String) throws {
    let script = Paths.sdkScripts.appendingPathComponent("populate_sysroot.sh")
    let status = try Shell.run(script.path, [headers, libs])
    if status != 0 { throw NSError(domain: "pdbuild", code: 4, userInfo: [NSLocalizedDescriptionKey: "sdk populate failed"]) }
}

func installerBuild(config: String, derived: String?) throws {
    #if os(macOS)
    let xcodebuild = Shell.which("xcodebuild") ?? "/usr/bin/xcodebuild"
    var args = [
        "-project", Paths.installerProject.path,
        "-scheme", "Install PocketDarwin",
        "-configuration", config,
        "-sdk", "macosx",
        "build"
    ]
    if let derived = derived {
        args.insert(contentsOf: ["-derivedDataPath", derived], at: 0)
    }
    let status = try Shell.run(xcodebuild, args)
    if status != 0 { throw NSError(domain: "pdbuild", code: 5, userInfo: [NSLocalizedDescriptionKey: "installer build failed"]) }
    #else
    print("installer-build is only supported on macOS.")
    #endif
}

func installerDMG() throws {
    #if os(macOS)
    let status = try Shell.run(Paths.dmgScript.path)
    if status != 0 { throw NSError(domain: "pdbuild", code: 6, userInfo: [NSLocalizedDescriptionKey: "installer dmg failed"]) }
    #else
    print("installer-dmg is only supported on macOS.")
    #endif
}

func main() throws {
    var args = CommandLine.arguments
    guard args.count >= 2 else {
        usage()
        return
    }
    let command = args[1]

    func value(after flag: String) -> String? {
        if let idx = args.firstIndex(of: flag), idx + 1 < args.count {
            return args[idx + 1]
        }
        return nil
    }

    switch command {
    case "interlude":
        try runInterlude()
    case "sdk-validate":
        try sdkValidate()
    case "sdk-populate":
        guard let headers = value(after: "--headers"), let libs = value(after: "--libs") else {
            throw NSError(domain: "pdbuild", code: 7, userInfo: [NSLocalizedDescriptionKey: "--headers and --libs required for sdk-populate"]) }
        try sdkPopulate(headers: headers, libs: libs)
    case "installer-build":
        let derived = value(after: "--derived")
        let config = value(after: "--config") ?? "Release"
        try installerBuild(config: config, derived: derived)
    case "installer-dmg":
        try installerDMG()
    case "all":
        try runInterlude()
        try sdkValidate()
        #if os(macOS)
        try installerBuild(config: "Release", derived: nil)
        #endif
    case "help", "-h", "--help":
        usage()
    default:
        print("Unknown command: \(command)\n")
        usage()
    }
}

try main()
