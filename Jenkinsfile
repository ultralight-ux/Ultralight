pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
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
               cmake ../packager -G "Ninja"
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
        stage('Build Windows x64') {
          agent {
            node {
              label 'win_x64'
            }
          }
          steps {
            bat '''
               rem Setup environment
               call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" amd64
               set CC=cl.exe
               set CXX=cl.exe

               rem Build Release
               if not exist build mkdir build
               cd build
               cmake ../packager -G "Ninja"
               ninja
            '''
          }
          post {
            success {
              deploy();
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
               cmake ../packager -G "Ninja"
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

def deploy() {
  withAWS(endpointUrl:'https://sfo2.digitaloceanspaces.com', credentials:'jenkins-access') {
    if (env.BRANCH_NAME == 'master') {
      s3Upload(bucket: 'ultralight-sdk', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk', workingDir:'build/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');

    } else if (env.BRANCH_NAME == 'dev') {
      s3Upload(bucket: 'ultralight-sdk-dev', workingDir:'build', includePathPattern:'*.7z', acl:'PublicRead');
      s3Upload(bucket: 'ultralight-sdk-dev', workingDir:'build/latest', includePathPattern:'*.7z', acl:'PublicRead', cacheControl:'public,max-age=60');
    }
  }
}