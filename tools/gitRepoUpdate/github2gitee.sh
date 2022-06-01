while [ 1 ]
do
    git pull --rebase
    git push gitee master
    sleep 30
done
