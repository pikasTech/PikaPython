package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/BurntSushi/toml"
	"github.com/go-git/go-git/v5"
	"github.com/go-git/go-git/v5/plumbing"
	cp "github.com/otiai10/copy"
)

var isShowSize = false

type Requerment_t struct {
	Name    string
	Version string
	Commit  string
}

type Package_t struct {
	Name           string
	Releases       []string
	ReleasesName   []string
	ReleasesCommit []string
}

type Config_t struct {
	Packages []Package_t
}

func main() {
	superPath := "/tmp"
	path := "/pikascript"

	go readPathSize(superPath + path)
	repo := updatePikascript(superPath + path)

	packages, res := getPackages(superPath + path)
	if !res {
		fmt.Printf("[error]: get package info faild.\n")
		return
	}
	fmt.Printf("\n")

	requerments, res := getRequestment("requestment.txt")
	if !res {
		fmt.Printf("[error]: get requerment info faild.\n")
		return
	}
	fmt.Printf("\n")

	requerments, res = matchRequestments(packages, requerments)
	if !res {
		fmt.Printf("[error]: match requestment faild.\n")
		return
	}
	fmt.Printf("\n")

	checkOutRequsetments(superPath+path, repo, requerments)

	fmt.Println("update OK !")
	for i := 3; i >= 0; i-- {
		time.Sleep(1 * time.Second)
		fmt.Println("this window will auto close after", i, "s...")
	}
}

func FilterDirsGlob(dir, suffix string) ([]string, error) {
	return filepath.Glob(filepath.Join(dir, suffix))
}

func MoveFile(sourcePath, destPath string) error {
	inputFile, err := os.Open(sourcePath)
	if err != nil {
		return fmt.Errorf("Couldn't open source file: %s", err)
	}
	outputFile, err := os.Create(destPath)
	if err != nil {
		inputFile.Close()
		return fmt.Errorf("Couldn't open dest file: %s", err)
	}
	defer outputFile.Close()
	_, err = io.Copy(outputFile, inputFile)
	inputFile.Close()
	if err != nil {
		return fmt.Errorf("Writing to output file failed: %s", err)
	}
	// The copy was successful, so now delete the original file
	err = os.Remove(sourcePath)
	if err != nil {
		return fmt.Errorf("Failed removing original file: %s", err)
	}
	return nil
}

func checkOutRequsetments(path string, repo *git.Repository, requerments []Requerment_t) {
	os.Mkdir("pikascript-lib", os.ModePerm)
	os.Mkdir("pikascript-core", os.ModePerm)
	os.Mkdir("pikascript-api", os.ModePerm)
	workTree, _ := repo.Worktree()
	for _, requerment := range requerments {
		/* checkout commit */
		fmt.Printf("checking out: %s\n", requerment.Commit)
		err := workTree.Checkout(&git.CheckoutOptions{
			Hash:  plumbing.NewHash(requerment.Commit),
			Force: true,
		})
		CheckIfError(err)
		/* update file */
		var packagePath string = path + "/package/" + requerment.Name
		var dirPath string = "pikascript-lib/" + requerment.Name
		if requerment.Name == "pikascript-core" {
			packagePath = path + "/src"
			dirPath = "pikascript-core"
			CheckIfError(cp.Copy(packagePath+"/../tools/pikaCompiler/rust-msc-latest-win10.exe", "./rust-msc-latest-win10.exe"))
			CheckIfError(err)
		}
		// fmt.Printf("    copy" + " " + packagePath + " " + dirPath + "\n")
		CheckIfError(cp.Copy(packagePath, dirPath))
		pyFileList, _ := FilterDirsGlob(packagePath, "*.py")
		for i := range pyFileList {
			pyFileSource := strings.ReplaceAll(pyFileList[i], "\\", "/")
			pyFilePath := strings.Split(pyFileSource, "/")
			pyFileName := pyFilePath[len(pyFilePath)-1]
			fmt.Println("    Installed: " + pyFileName)
			CheckIfError(os.Rename(pyFileSource, pyFileName))
		}


		pyiFileList, _ := FilterDirsGlob(packagePath, "*.pyi")
		for i := range pyiFileList {
			pyFileSource := strings.ReplaceAll(pyiFileList[i], "\\", "/")
			pyFilePath := strings.Split(pyFileSource, "/")
			pyFileName := pyFilePath[len(pyFilePath)-1]
			fmt.Println("    Installed: " + pyFileName)
			CheckIfError(os.Rename(pyFileSource, pyFileName))
		}

	}
	err := workTree.Checkout(&git.CheckoutOptions{
		Hash:  plumbing.NewHash("master"),
		Force: true,
	})
	CheckIfError(err)
}
func CheckIfError(err error) {
	if err == nil {
		return
	}

	fmt.Printf("\x1b[31;1m%s\x1b[0m\n", fmt.Sprintf("error: %s", err))
}
func getMatchedPackage(requerment Requerment_t, packages []Package_t) (Package_t, bool) {
	var pkg Package_t
	for _, pkg = range packages {
		if requerment.Name == pkg.Name {
			return pkg, true
		}
	}
	return pkg, false
}

func getMatchedCommit(requestVersion string, pkg Package_t) (string, bool) {
	for i, packageVersion := range pkg.ReleasesName {
		if packageVersion == requestVersion {
			return pkg.ReleasesCommit[i], true
		}
		if "latest" == requestVersion {
			return "master", true
		}
	}
	return "", false
}

func matchRequestments(packages []Package_t, requerments []Requerment_t) ([]Requerment_t, bool) {
	for i, requerment := range requerments {
		pkg, res := getMatchedPackage(requerment, packages)
		if !res {
			fmt.Printf("[error]: match package for %s faild.\n", requerment.Name)
			return requerments, false
		}
		commit, res := getMatchedCommit(requerment.Version, pkg)
		if !res {
			fmt.Printf("[error]: match commit for %s faild.\n", requerment.Name)
			return requerments, false
		}
		fmt.Printf("matched: %s %s\n", pkg.Name, commit)
		requerments[i].Commit = commit
	}
	return requerments, true
}

func getRequestment(path string) ([]Requerment_t, bool) {
	var requestments []Requerment_t
	requestment_file, _ := os.Open(path)
	defer requestment_file.Close()
	scanner := bufio.NewScanner(requestment_file)
	var count int
	for scanner.Scan() {
		var requerment Requerment_t
		count++
		line := scanner.Text()
		line = strings.ReplaceAll(line, " ", "")
		requerment.Name = strings.Split(line, "==")[0]
		requerment.Version = strings.Split(line, "==")[1]
		fmt.Printf("request: %s %s\n", requerment.Name, requerment.Version)
		requestments = append(requestments, requerment)
	}
	return requestments, true
}

func getPackages(path string) ([]Package_t, bool) {
	var config Config_t
	if _, err := toml.DecodeFile(path+"/packages.toml", &config); err != nil {
		fmt.Println(err)
		return config.Packages, false
	}
	for i_package, pkg := range config.Packages {
		i := 0
		for _, release := range pkg.Releases {
			pkg.ReleasesName = append(pkg.ReleasesName, strings.Split(release, " ")[0])
			pkg.ReleasesCommit = append(pkg.ReleasesCommit, strings.Split(release, " ")[1])
			i++
		}
		config.Packages[i_package] = pkg
	}
	return config.Packages, true
}

func readPathSize(path string) {
	for {
		if !isShowSize {
			continue
		}
		size, _ := pathSize(path)
		fmt.Printf("recived : %2f MB \n", float64(size)/1024/1024)
		time.Sleep(time.Second)
	}
}

func updatePikascript(path string) *git.Repository {
	pathExist, err := PathExists(path)
	if err != nil {
		fmt.Printf("PathExists(%s),err(%v)\n", path, err)
	}
	if !pathExist {
		/* clone the pikascript repo */
		fmt.Printf("downloading pikascript to %s...\n", path)
		fmt.Printf("need about 10 min(s)\n")
		fmt.Printf("please wait...\n")
		isShowSize = true
		_, err = git.PlainClone(path, false, &git.CloneOptions{
			URL:      "https://gitee.com/lyon1998/pikascript",
			Progress: os.Stdout,
		})
		if nil != err {
			fmt.Println(err)
		}
	}

	/* pull the pikascript repo */
	repo, _ := git.PlainOpen(path)
	w, _ := repo.Worktree()
	fmt.Println("updating pikascript...")
	w.Pull(&git.PullOptions{RemoteName: "origin"})
	ref, _ := repo.Head()
	commit, _ := repo.CommitObject(ref.Hash())
	fmt.Println(commit)

	isShowSize = false

	err = w.Checkout(&git.CheckoutOptions{
		Hash:  plumbing.NewHash("master"),
		Force: true,
	})
	CheckIfError(err)

	return repo
}

func PathExists(path string) (bool, error) {
	_, err := os.Stat(path)
	if err == nil {
		return true, nil
	}
	if os.IsNotExist(err) {
		return false, nil
	}
	return false, err
}

func pathSize(path string) (int64, error) {
	var size int64
	err := filepath.Walk(path, func(_ string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if !info.IsDir() {
			size += info.Size()
		}
		return err
	})
	return size, err
}
