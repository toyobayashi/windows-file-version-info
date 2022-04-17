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

  private constructor (external: object)

  toString (): string
  toJSON (): IFileVersionInfo
  static getVersionInfo (fileName: string): FileVersionInfo
}
