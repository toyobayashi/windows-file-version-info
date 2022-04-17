const { EOL } = require('os')
const { existsSync } = require('fs')
const mod = require('./dist/fvi.node')

const { FileVersionInfo, getVersionInfoInternal } = mod

Object.defineProperty(FileVersionInfo, 'getVersionInfo', {
  configurable: true,
  writable: true,
  value (fileName) {
    if (typeof fileName !== 'string') {
      throw new TypeError('Expect string path')
    }
    if (!existsSync(fileName)) {
      throw new Error('File is not found: ' + fileName)
    }
    return getVersionInfoInternal(fileName)
  }
})

Object.defineProperty(FileVersionInfo.prototype, 'toString', {
  configurable: true,
  writable: true,
  value () {
    return (
      'File:             ' + this.fileName + EOL +
      'InternalName:     ' + this.internalName + EOL +
      'OriginalFilename: ' + this.originalFilename + EOL +
      'FileVersion:      ' + this.fileVersion + EOL +
      'FileDescription:  ' + this.fileDescription + EOL +
      'Product:          ' + this.productName + EOL +
      'ProductVersion:   ' + this.productVersion + EOL +
      'Debug:            ' + this.isDebug + EOL +
      'Patched:          ' + this.isPatched + EOL +
      'PreRelease:       ' + this.isPreRelease + EOL +
      'PrivateBuild:     ' + this.isPrivateBuild + EOL +
      'SpecialBuild:     ' + this.isSpecialBuild + EOL +
      'Language:         ' + this.language + EOL
    )
  }
})

Object.defineProperty(FileVersionInfo.prototype, 'toJSON', {
  configurable: true,
  writable: true,
  value () {
    return {
      comments: this.comments,
      companyName: this.companyName,
      fileBuildPart: this.fileBuildPart,
      fileDescription: this.fileDescription,
      fileMajorPart: this.fileMajorPart,
      fileMinorPart: this.fileMinorPart,
      fileName: this.fileName,
      filePrivatePart: this.filePrivatePart,
      fileVersion: this.fileVersion,
      internalName: this.internalName,
      isDebug: this.isDebug,
      isPatched: this.isPatched,
      isPreRelease: this.isPreRelease,
      isPrivateBuild: this.isPrivateBuild,
      isSpecialBuild: this.isSpecialBuild,
      language: this.language,
      legalCopyright: this.legalCopyright,
      legalTrademarks: this.legalTrademarks,
      originalFilename: this.originalFilename,
      privateBuild: this.privateBuild,
      productBuildPart: this.productBuildPart,
      productMajorPart: this.productMajorPart,
      productMinorPart: this.productMinorPart,
      productName: this.productName,
      productPrivatePart: this.productPrivatePart,
      productVersion: this.productVersion,
      specialBuild: this.specialBuild
    }
  }
})

module.exports = mod
