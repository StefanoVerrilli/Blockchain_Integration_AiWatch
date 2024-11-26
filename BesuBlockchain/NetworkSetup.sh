#!/bin/bash

ArrayToString(){
    str="";
    delim=""
    for i in "$@";
    do
        str="$str$delim$i";
        delim=","
    done
    str="${str%,}"
}




for((i=1;i<$1+1;i++)) ; do
    mkdir -p Node$i/data
done


besu operator generate-blockchain-config --config-file=ibftConfigFile.json --to=networkFiles --private-key-file-name=key

#do more
root=$(pwd);
cd networkFiles/keys
for((i=1;i<$1+1;i++)); do
    result=$(ls -d */ | tail -n $(($1-i+1)) | head -1)
    echo ${result};
    cd $result
    echo $root/Node$i
    cp key $root/Node$i/data
    cp key.pub $root/Node$i/data
    cd ..
done
cd ../..
#cp genesis.json $root


host="172.16.239.30"
port=8545;
ptpPort=30303
cd Node1
key=$(besu --data-path=data public-key export | tail -1)
Enode="\"enode://${key:2}@${host}:${ptpPort}\""
cd ..
echo "bootnodes=["${Enode}"]" >> config.toml;

#for((i=0;i<$1;i++)); do
#    cd Node$i
#    key=$(besu --data-path=data public-key export | tail -1)
#    EnodeList[$i]="enode://${key:2}@${host}:$((${ptpPort}+$i))"
#    cd ..
#done


#cd Node0
#enode=$(besu --data-path=data public-key export | tail -1)
#echo $enode
#CompletePath="enode://${enode:2}@${host}:${ptpPort}"
#echo $CompletePath
#echo "besu --config-file=../configuration.toml --rpc-http-port=${port} --p2p-port=${ptpPort}" > ConfigNode0.sh
#chmod +x ConfigNode0.sh
#cd ..
#ArrayToString "${EnodeList[@]}"


#for((i=1;i<$1;i++)); do
#    cd Node$i
#    echo $(pwd)
#    Currentport=$((port+$i));
#    echo "besu --config-file=../configuration.toml --bootnodes=${CompletePath} --rpc-http-port=${Currentport} --p2p-port=$((${ptpPort}+$i))" > ConfigNode$i.sh
#    chmod +x ConfigNode$i.sh
#    cd ..
#done

