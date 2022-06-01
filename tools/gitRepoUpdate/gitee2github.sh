while [ 1 ]
do
    sleep 30
    git pull gitee master --rebase
    git push
done
