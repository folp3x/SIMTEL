#!/bin/bash

SERVER_CMD="./build/server/server -k ./build/server/data/config.json"
CLIENT_CMD="./build/client/client -k ./build/client/data/"

TEMP_FILES=()
for i in {0..2}; do
    TEMP_FILES+=($(mktemp))
done

$SERVER_CMD > /dev/null 2>&1 &
SERVER_PID=$!

sleep 1

if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Ошибка: сервер не запустился (возможно адрес занят или еще не освобожден ОС)"
    exit 1
fi

# запуск клиентов
printf "protocol b\nactive 1\nexit\n" | ${CLIENT_CMD}config1.json > ${TEMP_FILES[0]} 2>&1 &
printf "move 3 2 1\nactive 1\nexit\n" | ${CLIENT_CMD}config2.json > ${TEMP_FILES[1]} 2>&1 &
printf "active 1\nmove 1 2 3\nexit\n" | ${CLIENT_CMD}config3.json > ${TEMP_FILES[2]} 2>&1 &

sleep 1

kill $SERVER_PID 2>/dev/null

for i in {0..2}; do
    printf "\n\n\n"
    echo "=== CLIENT $((i+1)) ==="
    cat ${TEMP_FILES[$i]}

    rm ${TEMP_FILES[$i]}
done
