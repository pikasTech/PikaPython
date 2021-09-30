package main

import (
	"fmt"
	"os"

	"github.com/go-git/go-git/v5"
)

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

func main() {
	path := "/tmp/pikascript"
	pathExist, err := PathExists(path)
	if err != nil {
		fmt.Printf("PathExists(%s),err(%v)\n", path, err)
	}

	if !pathExist {
		/* clone the pikascript repo */
		fmt.Printf("downloading pikascript to %s ...\n", path)
		fmt.Printf("need about 10 mins\n")
		fmt.Printf("pleas wait...\n")
		_, err = git.PlainClone(path, false, &git.CloneOptions{
			URL:      "https://github.com.cnpmjs.org/pikastech/pikascript",
			Progress: os.Stdout,
		})
		if nil != err {
			fmt.Println(err)
		}
	}

	/* pull the pikascript repo */
	r, _ := git.PlainOpen(path)
	w, _ := r.Worktree()
	fmt.Println("updateing pikascript...")
	w.Pull(&git.PullOptions{RemoteName: "origin"})
	ref, _ := r.Head()
	commit, _ := r.CommitObject(ref.Hash())
	fmt.Println(commit)
}
