pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
        stage('Build macOS Debug') {
          agent {
            node {
              label 'macos_dbg'
            }
          }
          steps {
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"
    
               # Build Debug
               mkdir -p build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DGET_DBG_DEPS=1
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build macOS') {
          agent {
            node {
              label 'macos'
            }
          }
          steps {
            sh '''
               # Setup environment
               export PATH="/usr/local/bin:$PATH"

               # Build Release
               mkdir -p build
               cd build
               cmake .. -G "Ninja"
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Windows x64 Debug') {
          agent {
            node {
              label 'win_x64_dbg'
            }
          }
          steps {
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe
              
               rem Build Debug
               if not exist build_dbg mkdir build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DGET_DBG_DEPS=1
               ninja
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Windows x64') {
          agent {
            node {
              label 'win_x64'
            }
          }
          steps {
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe

               rem Build Release
               if not exist build mkdir build
               cd build
               cmake .. -G "Ninja"
               ninja
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Windows x86 Debug') {
          agent {
            node {
              label 'win_x86_dbg'
            }
          }
          steps {
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64_x86
               set CC=cl.exe
               set CXX=cl.exe
              
               rem Build Debug
               if not exist build_dbg mkdir build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DGET_DBG_DEPS=1
               ninja
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Windows x86') {
          agent {
            node {
              label 'win_x86'
            }
          }
          steps {
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64_x86
               set CC=cl.exe
               set CXX=cl.exe

               rem Build Release
               if not exist build mkdir build
               cd build
               cmake .. -G "Ninja"
               ninja
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
        stage('Build Linux Debug') {
          agent {
            node {
              label 'linux_dbg'
            }
          }
          steps {
            sh '''     
               # Build Debug
               mkdir -p build_dbg
               cd build_dbg
               cmake .. -G "Ninja" -DGET_DBG_DEPS=1
               ninja
               cd ..
            '''
          }
          post {
            success {
              deployDebug();
            }
          }
        }
        stage('Build Linux') {
          agent {
            node {
              label 'linux'
            }
          }
          steps {
            sh '''     
               # Build Release
               mkdir -p build
               cd build
               cmake .. -G "Ninja"
               ninja
               cd ..
            '''
          }
          post {
            success {
              deploy();
            }
          }
        }
      }
    }
  }
}

def deployDebug() {
  withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com', credentials:'jenkins-access') {
    if (env.BRANCH_NAME == 'master') {
      s3Upload(bucket: 'ultralight-sdk-dbg', workingDir:'build_dbg', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk-dbg', workingDir:'build_dbg/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');
      
    } else if (env.BRANCH_NAME == 'dev') {
      s3Upload(bucket: 'ultralight-sdk-dev-dbg', workingDir:'build_dbg', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk-dev-dbg', workingDir:'build_dbg/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');
    }
  }
}

def deploy() {
  withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com', credentials:'jenkins-access') {
    if (env.BRANCH_NAME == 'master') {
      s3Upload(bucket: 'ultralight-sdk', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk', workingDir:'build_dbg/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');

    } else if (env.BRANCH_NAME == 'dev') {
      s3Upload(bucket: 'ultralight-sdk-dev', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk-dev', workingDir:'build_dbg/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');
    }
  }
}