while [ 1 ]
do
    sleep 30
    git pull --rebase
    git push gitee master
done
