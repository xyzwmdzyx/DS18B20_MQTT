#!/bin/bash

# -b 代表云平台，HW = 华为，AL = 阿里，TX = 腾讯； -t 代表读取温度的时间间隔，单位秒（s）
PARSE="-b HW -t 60"

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

# 设置环境变量
LD_LIBRARY_PATH=$SCRIPT_DIR/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

# 启动服务器程序
./bin/client $PARSE
