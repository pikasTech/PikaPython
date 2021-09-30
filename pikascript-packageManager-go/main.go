package main

import (
	"fmt"
	"os"

	"github.com/go-git/go-git/v5"
)

func main() {
	path := "tmp/pikascript"
	_, err := git.PlainClone(path, false, &git.CloneOptions{
		URL:      "https://github.com.cnpmjs.org/pikastech/pikascript",
		Progress: os.Stdout,
	})
	if nil != err {
		fmt.Println(err)
	}

	r, _ := git.PlainOpen(path)
	w, _ := r.Worktree()
	fmt.Println("git pull origin")
	w.Pull(&git.PullOptions{RemoteName: "origin"})
	ref, _ := r.Head()
	commit, _ := r.CommitObject(ref.Hash())
	fmt.Println(commit)
}
