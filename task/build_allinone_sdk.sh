#!/bin/bash

script_dir=$(cd `dirname $0`; pwd)
combodir=${script_dir}/combodir
cd $script_dir

function cloneCheck(){
  tagArr=$(git tag)
  echo $tagArr
  if [[ ${IS_TEST} == 'true' || ${tagArr} =~ ${VERSION} ]];
  then
     echo "不需要clone"
     # 不需要 clone
     result="false"
  else
     # 创建目录 clone 
      echo "需要clone"
      result="true"
       
  fi
}

cloneCheck
if [ $result == 'true' ];
then
    echo "start git clone "
    if [ ! -d "${combodir}" ];
    then
      echo "file not exist"
    mkdir $combodir
    fi
    cd $combodir
    git clone xxxx.git
    cd xxxx
fi

# 执行 build

./gradlew clean :allinonelibrary:xxxtask

