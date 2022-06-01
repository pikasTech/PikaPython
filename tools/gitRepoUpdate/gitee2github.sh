while [ 1 ]
do
    git pull gitee master --rebase
    git push
    sleep 30
done
