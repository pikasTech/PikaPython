package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"sync"
	"time"

	"github.com/go-git/go-git/v5"
)

var waitGroup sync.WaitGroup
var ch = make(chan struct{}, 255)
var isShowSize = false

func dirents(path string) ([]os.FileInfo, bool) {
	entries, err := ioutil.ReadDir(path)
	if err != nil {
		return nil, false
	}
	return entries, true
}

//递归计算目录下所有文件
func walkDir(path string, fileSize chan<- int64) {
	defer waitGroup.Done()
	ch <- struct{}{} //限制并发量
	entries, ok := dirents(path)
	<-ch
	if !ok {
		return
	}
	for _, e := range entries {
		if e.IsDir() {
			waitGroup.Add(1)
			go walkDir(filepath.Join(path, e.Name()), fileSize)
		} else {
			fileSize <- e.Size()
		}
	}
}

func all_file(dir_path string) {

	//文件大小chennel
	fileSize := make(chan int64)
	//文件总大小
	var sizeCount int64
	//文件数目
	var fileCount int

	//计算目录下所有文件占的大小总和
	waitGroup.Add(1)
	go walkDir(dir_path, fileSize)

	go func() {
		defer close(fileSize)
		waitGroup.Wait()
	}()

	for size := range fileSize {
		fileCount++
		sizeCount += size
	}

	fmt.Printf("recived:\n")
	fmt.Printf("%fM, %d\n", float64(sizeCount)/1024/1024, fileCount)
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

func readFolderSize(path string) {
	// t := time.Now()
	dir_path := path
	//all_file(dir_path)

	for {
		if !isShowSize {
			continue
		}
		all_file(dir_path)
		time.Sleep(time.Second)
	}

	// fmt.Println("花费的时间为 " + time.Since(t).String())
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

func main() {
	superPath := "/tmp"
	path := "/pikascript"

	go readFolderSize(superPath + path)

	updatePikascript(superPath + path)
}
