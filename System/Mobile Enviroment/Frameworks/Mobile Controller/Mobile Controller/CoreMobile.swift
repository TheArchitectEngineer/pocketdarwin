import Foundation

public struct MobileInfo {
    public let externalStorageRoot: String
    public let mobileEnvironmentRoot: String
    public let internalStorageAllowed: Bool

    public let uartBase: UInt64
    public let uartSize: UInt64
    public let uartIRQ: Int

    public let fbBase: UInt64
    public let fbSize: UInt64
    public let fbWidth: Int
    public let fbHeight: Int
    public let fbStride: Int
}

public func getMobileInfo() -> MobileInfo? {
    var info = MCMobileInfo()
    guard MCGetMobileInfo(&info) else {
        return nil
    }

    let externalStorageRoot = info.externalStorageRoot ?? "/sdcard"
    let mobileEnvironmentRoot = info.mobileEnvironmentRoot ?? "/System/Mobile Enviroment"

    return MobileInfo(
        externalStorageRoot: externalStorageRoot,
        mobileEnvironmentRoot: mobileEnvironmentRoot,
        internalStorageAllowed: info.internalStorageAllowed,
        uartBase: info.uartBase,
        uartSize: info.uartSize,
        uartIRQ: info.uartIRQ,
        fbBase: info.fbBase,
        fbSize: info.fbSize,
        fbWidth: info.fbWidth,
        fbHeight: info.fbHeight,
        fbStride: info.fbStride
    )
}
