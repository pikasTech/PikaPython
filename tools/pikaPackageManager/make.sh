go build -ldflags="-w -s"
upx -8 pikaPackage.exe
