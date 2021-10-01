package main

import (
	"fmt"
	"os"
	"path/filepath"
	"time"

	"github.com/BurntSushi/toml"
	"github.com/go-git/go-git/v5"
)

var isShowSize = false

type Config struct {
	Package      string
	Dependencies map[string]Dependence
}

type Dependence struct {
	Version string
}

func main() {
	superPath := "/tmp"
	path := "/pikascript"

	var config Config
	if _, err := toml.DecodeFile("pikaScript.toml", &config); err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("package: %s\n", config.Package)

	for dependenceName, dependence := range config.Dependencies {
		fmt.Printf("dependencies: %s %s\n", dependenceName, dependence.Version)
	}

	go readPathSize(superPath + path)
	updatePikascript(superPath + path)

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
	fmt.Println("update OK !")
	for i := 3; i >= 0; i-- {
		time.Sleep(1 * time.Second)
		fmt.Println("this window will auto close after", i, "s...")
	}
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
