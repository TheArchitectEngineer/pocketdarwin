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

    static func capture(_ command: String, _ args: [String] = []) -> String? {
        let process = Process()
        process.executableURL = URL(fileURLWithPath: command)
        process.arguments = args

        let pipe = Pipe()
        process.standardOutput = pipe
        process.standardError = Pipe()

        do {
            try process.run()
        } catch {
            return nil
        }

        process.waitUntilExit()
        guard process.terminationStatus == 0 else { return nil }
        let data = pipe.fileHandleForReading.readDataToEndOfFile()
        return String(data: data, encoding: .utf8)?.trimmingCharacters(in: .whitespacesAndNewlines)
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
    static let systemDir = root.appendingPathComponent("System")
    static let distDir = root.appendingPathComponent("build/dist")
    static let defaultIPhoneOS20SDK = root.appendingPathComponent("Developer/SDKs/iPhoneOS2.0.sdk")
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
  system-bundle        Build apps/libs/services/programs and emit a .System bundle
  all                  Run interlude + sdk-validate (and installer on macOS)

Options:
  --headers <path>               Headers directory for sdk-populate
  --libs <path>                  Libs directory for sdk-populate
  --derived <path>               DerivedData path for installer build
  --config <Debug|Release>       Build configuration for installer (default Release)
  --sdk-root <path>              Sysroot used for cross-compiling system-bundle
  --iphoneos2-sdk                Shortcut for --sdk-root Developer/SDKs/iPhoneOS2.0.sdk
  --output-dir <path>            Output directory for generated .System bundle (default build/dist)
""")
}

func requireDir(_ path: String) throws {
    var isDir: ObjCBool = false
    if !FileManager.default.fileExists(atPath: path, isDirectory: &isDir) || !isDir.boolValue {
        throw NSError(domain: "pdbuild", code: 1, userInfo: [NSLocalizedDescriptionKey: "Missing directory: \(path)"])
    }
}

func runInterlude() throws {
    try requireDir(Paths.interludeDir.path)
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

func randomCodename() -> String {
    let adjectives = ["Aurora", "Crimson", "Nimbus", "Solar", "Velvet", "Copper", "Lunar", "Silent", "Rapid", "Frost"]
    let nouns = ["Otter", "Falcon", "Harbor", "Comet", "Cedar", "Wave", "Forge", "Echo", "Quartz", "Signal"]
    return "\(adjectives.randomElement() ?? "Pocket")-\(nouns.randomElement() ?? "Darwin")"
}

func currentBranchName() -> String {
    guard let git = Shell.which("git"),
          let branch = Shell.capture(git, ["rev-parse", "--abbrev-ref", "HEAD"]),
          !branch.isEmpty else {
        return "detached"
    }

    let allowed = CharacterSet.alphanumerics.union(CharacterSet(charactersIn: "-_."))
    return String(branch.unicodeScalars.map { allowed.contains($0) ? Character($0) : "-" })
}

func timestamp() -> String {
    let formatter = DateFormatter()
    formatter.dateFormat = "yyyyMMdd-HHmmss"
    formatter.locale = Locale(identifier: "en_US_POSIX")
    formatter.timeZone = TimeZone(secondsFromGMT: 0)
    return formatter.string(from: Date())
}

func crossCompileEnvironment(sdkRoot: String?) -> [String: String] {
    guard let sdkRoot = sdkRoot else { return [:] }

    let isysrootFlags = "-isysroot \(sdkRoot) -miphoneos-version-min=2.0"
    var env: [String: String] = [
        "SDKROOT": sdkRoot,
        "IPHONEOS_DEPLOYMENT_TARGET": "2.0",
        "CFLAGS": isysrootFlags,
        "CXXFLAGS": isysrootFlags,
        "OBJCFLAGS": isysrootFlags,
        "LDFLAGS": isysrootFlags
    ]

    if let cc = Shell.which("clang") {
        env["CC"] = cc
    }
    if let cxx = Shell.which("clang++") {
        env["CXX"] = cxx
    }

    return env
}

func buildWithMakeIfPresent(in directory: URL, env: [String: String]) throws -> Bool {
    let makefile = directory.appendingPathComponent("Makefile")
    let gnumakefile = directory.appendingPathComponent("GNUmakefile")
    guard FileManager.default.fileExists(atPath: makefile.path) || FileManager.default.fileExists(atPath: gnumakefile.path) else {
        return true
    }

    let make = Shell.which("make") ?? "/usr/bin/make"
    let status = try Shell.run(make, ["-C", directory.path], env: env)
    if status != 0 {
        return false
    }
    return true
}

func buildTargets(in root: URL, env: [String: String]) throws -> [String] {
    guard let entries = try? FileManager.default.contentsOfDirectory(at: root, includingPropertiesForKeys: nil, options: [.skipsHiddenFiles]) else {
        return []
    }
    var failed: [String] = []
    for entry in entries where entry.hasDirectoryPath {
        let ok = try buildWithMakeIfPresent(in: entry, env: env)
        if !ok { failed.append(entry.path) }
    }
    return failed
}

func writeBundleInfoPlist(to plistPath: URL, bundleName: String, branch: String, codename: String, builtAt: String) throws {
    let plist: [String: Any] = [
        "CFBundleName": "PocketDarwin System Bundle",
        "CFBundleIdentifier": "org.pocketdarwin.systembundle",
        "CFBundleVersion": builtAt,
        "PDSystemBundleName": bundleName,
        "PDBuildBranch": branch,
        "PDBuildTimestamp": builtAt,
        "PDBuildCodename": codename,
        "PDBuildSDK": "iPhoneOS2.0"
    ]

    let data = try PropertyListSerialization.data(fromPropertyList: plist, format: .xml, options: 0)
    try data.write(to: plistPath)
}

func copyIfExists(from src: URL, to dst: URL) throws {
    if FileManager.default.fileExists(atPath: src.path) {
        try FileManager.default.createDirectory(at: dst.deletingLastPathComponent(), withIntermediateDirectories: true)
        if FileManager.default.fileExists(atPath: dst.path) {
            try FileManager.default.removeItem(at: dst)
        }
        try FileManager.default.copyItem(at: src, to: dst)
    }
}

func buildSystemBundle(sdkRoot: String?, outputDir: String) throws {
    let fm = FileManager.default
    let bundleBranch = currentBranchName()
    let bundleTimestamp = timestamp()
    let codename = randomCodename()
    let bundleName = "PocketDarwin-\(bundleBranch)-\(bundleTimestamp)-\(codename).System"

    let outputURL = URL(fileURLWithPath: outputDir, relativeTo: Paths.root).standardizedFileURL
    try fm.createDirectory(at: outputURL, withIntermediateDirectories: true)

    let bundleURL = outputURL.appendingPathComponent(bundleName)
    if fm.fileExists(atPath: bundleURL.path) {
        try fm.removeItem(at: bundleURL)
    }

    let contentsURL = bundleURL.appendingPathComponent("Contents")
    let systemPayloadURL = contentsURL.appendingPathComponent("System")
    let appsURL = systemPayloadURL.appendingPathComponent("Applications")
    let frameworksURL = systemPayloadURL.appendingPathComponent("Libraries")
    let servicesURL = systemPayloadURL.appendingPathComponent("Services")
    let programsURL = systemPayloadURL.appendingPathComponent("Programs")

    try fm.createDirectory(at: appsURL, withIntermediateDirectories: true)
    try fm.createDirectory(at: frameworksURL, withIntermediateDirectories: true)
    try fm.createDirectory(at: servicesURL, withIntermediateDirectories: true)
    try fm.createDirectory(at: programsURL, withIntermediateDirectories: true)

    let env = crossCompileEnvironment(sdkRoot: sdkRoot)
    print("Building applications, libraries, services, and programs...")
    var failures: [String] = []
    failures += try buildTargets(in: Paths.systemDir.appendingPathComponent("Applications"), env: env)
    failures += try buildTargets(in: Paths.systemDir.appendingPathComponent("Frameworks"), env: env)
    failures += try buildTargets(in: Paths.systemDir.appendingPathComponent("Services"), env: env)
    failures += try buildTargets(in: Paths.systemDir.appendingPathComponent("Programs"), env: env)

    try copyIfExists(from: Paths.systemDir.appendingPathComponent("Applications"), to: appsURL)
    try copyIfExists(from: Paths.systemDir.appendingPathComponent("Frameworks"), to: frameworksURL)
    try copyIfExists(from: Paths.systemDir.appendingPathComponent("Services"), to: servicesURL)
    try copyIfExists(from: Paths.systemDir.appendingPathComponent("Programs"), to: programsURL)

    try writeBundleInfoPlist(
        to: contentsURL.appendingPathComponent("Info.plist"),
        bundleName: bundleName,
        branch: bundleBranch,
        codename: codename,
        builtAt: bundleTimestamp
    )

    print("Generated: \(bundleURL.path)")
    if !failures.isEmpty {
        print("Warning: some targets failed to build and were packaged from source state:")
        for failure in failures { print(" - \(failure)") }
    }
}

func main() throws {
    let args = CommandLine.arguments
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
    case "system-bundle":
        var sdkRoot = value(after: "--sdk-root")
        if args.contains("--iphoneos2-sdk") {
            sdkRoot = Paths.defaultIPhoneOS20SDK.path
        }
        if let sdkRoot = sdkRoot {
            try requireDir(sdkRoot)
        }
        let outputDir = value(after: "--output-dir") ?? Paths.distDir.path
        try buildSystemBundle(sdkRoot: sdkRoot, outputDir: outputDir)
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

do {
    try main()
} catch {
    fputs("error: \(error.localizedDescription)\n", stderr)
    exit(1)
}
