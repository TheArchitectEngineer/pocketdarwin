#!/bin/sh

#check if mkbootimg is installed
if ! command -v mkbootimg &> /dev/null
then
    echo "mkbootimg could not be found, please install it first"
    exit
fi

#check if the user is root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi