const { FileVersionInfo } = require('..')

const cmdInfo = FileVersionInfo.getVersionInfo('C:\\WINDOWS\\system32\\cmd.exe')
console.log(cmdInfo.toString())
console.log(cmdInfo.toJSON())
