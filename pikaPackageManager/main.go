package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/BurntSushi/toml"
	"github.com/go-git/go-git/v5"
)

var isShowSize = false

type Requerment_t struct {
	Name    string
	Version string
}

type Package_t struct {
	Name     string
	Releases []string
}

type Config_t struct {
	Packages []Package_t
}

func main() {
	superPath := "/tmp"
	path := "/pikascript"

	go readPathSize(superPath + path)
	updatePikascript(superPath + path)

	getPackages(superPath + path)
	fmt.Printf("\n")
	getRequestment("requestment.txt")
	fmt.Printf("\n")

	fmt.Println("update OK !")
	for i := 3; i >= 0; i-- {
		time.Sleep(1 * time.Second)
		fmt.Println("this window will auto close after", i, "s...")
	}
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
		requerment.Name = strings.Split(line, "==")[0]
		requerment.Version = strings.Split(line, "==")[1]
		fmt.Printf("request: %s %s\n", requerment.Name, requerment.Version)
		requestments = append(requestments, requerment)
	}
	return requestments, true
}

func getPackages(path string) (Config_t, bool) {
	var config Config_t
	if _, err := toml.DecodeFile(path+"/pikaPackageManager/packages.toml", &config); err != nil {
		fmt.Println(err)
		return config, false
	}
	for _, pkg := range config.Packages {
		fmt.Printf("found package: %s\n", pkg.Name)
		for _, release := range pkg.Releases {
			fmt.Printf("    release: %s\n", release)
		}
	}
	return config, true
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

func updatePikascript(path string) {
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
	r, _ := git.PlainOpen(path)
	w, _ := r.Worktree()
	fmt.Println("updating pikascript...")
	w.Pull(&git.PullOptions{RemoteName: "origin"})
	ref, _ := r.Head()
	commit, _ := r.CommitObject(ref.Hash())
	fmt.Println(commit)

	isShowSize = false

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
