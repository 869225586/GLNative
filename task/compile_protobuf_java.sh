#!/bin/bash

script_path=$(cd "$(dirname $0)"; pwd)
root_path=$script_path/../
output_path=$script_path/src/main/java
bin_path=$root_path/thirdparty/tool/mac/bin
repo_path=$root_path/thirdparty/repos/common-proto

dirs=("livestream" "protobuf" "socket")
for dir in ${dirs[@]}; do
    rm -rf $output_path/com/kuaishou/$dir
done

files=("livestream/live_stream_messages.proto" "voip.proto" "user_infos.proto" "livestream/live_stream_rich_text_feed.proto" "livestream/live_stream_start_play_source.proto" "livestream/red_pack_type.proto")

for file in ${files[@]}; do
    $bin_path/protoc-3.3.0 --java_out=$output_path -I=$repo_path/src/main/proto $repo_path/src/main/proto/kuaishou/$file
done
