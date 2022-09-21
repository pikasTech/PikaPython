go build -ldflags="-w -s"
upx -9 pikaPackage.exe
