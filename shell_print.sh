#!/bin/bash

#  For Color shell print
# Basic color
C_Black="\e[30m"
C_Red="\e[31m"
C_Green="\e[32m"
C_Yellow="\e[33m"
C_Blue="\e[34m"
C_Purple="\e[35m"
C_Cyan="\e[36m"
C_White="\e[37m"
C_Reset="\e[0m"

function LOG_V ()
{
    echo -e $C_Purple
    echo "$1" | boxes -d shell
    echo -e $C_Reset
}

function LOG_INF ()
{
    echo -e $C_Green
    echo "$1" | boxes -d shell
    echo -e $C_Reset
}

function LOG_ERR ()
{
    echo -e $C_Red
    echo "$1" | boxes -d nuke
    echo -e $C_Reset
}

function LOG_OK ()
{
    echo -e $C_Green
    echo "$1" | boxes -d sunset
    echo -e $C_Reset
}
