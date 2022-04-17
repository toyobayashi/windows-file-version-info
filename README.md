# windows-file-version-info

This package works on Windows only.

[FileVersionInfo MSDN](https://docs.microsoft.com/en-us/dotnet/api/system.diagnostics.fileversioninfo)

## Usage

```js
const { FileVersionInfo } = require('@tybys/windows-file-version-info')

const info = FileVersionInfo.getVersionInfo('C:\\path\\to\\exe\\or\\dll')
console.log(info.fileDescription)
console.log(info.fileVersion)
console.log(info.toString())
console.log(JSON.stringify(info, null, 2))
```

## API

```ts
export declare interface IFileVersionInfo {
  readonly comments: string
  readonly companyName: string
  readonly fileBuildPart: number
  readonly fileDescription: string
  readonly fileMajorPart: number
  readonly fileMinorPart: number
  readonly fileName: string
  readonly filePrivatePart: string
  readonly fileVersion: string
  readonly internalName: string
  readonly isDebug: boolean
  readonly isPatched: boolean
  readonly isPreRelease: boolean
  readonly isPrivateBuild: boolean
  readonly isSpecialBuild: boolean
  readonly language: string
  readonly legalCopyright: string
  readonly legalTrademarks: string
  readonly originalFilename: string
  readonly privateBuild: string
  readonly productBuildPart: number
  readonly productMajorPart: number
  readonly productMinorPart: number
  readonly productName: string
  readonly productPrivatePart: number
  readonly productVersion: string
  readonly specialBuild: string
}

export declare class FileVersionInfo implements IFileVersionInfo {
  /* ...from IFileVersionInfo... */

  private constructor (external: object)
  toString (): string
  toJSON (): IFileVersionInfo
  static getVersionInfo (fileName: string): FileVersionInfo
}
```
