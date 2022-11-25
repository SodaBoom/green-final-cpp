rm -rf green-final.tar
docker build -t green-final .

docker save green-final -o green-final.tar
