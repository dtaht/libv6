#!/bin/sh
# args filename struct name
# Transform a struct to C

m=0
insemi=0
incurly=0
incomment=0
line=0

start() {
	echo "bool $func {"
}

print_bool() {
    local valstr = $0
    local name = $1
    local val = $2

    echo "printf(\"$valstr;\",name,val ? \"enabled\" : \"disabled\" \")\";"
}

print() {
	echo "printf(\"$valstr;\",name,val)";
}

type_to_printf() {
    case $0 in
        ubase_t) S="%d" ;;
        uint32_t) S="%d" ;;
    esac
}

finalize() {
	echo "}"
}

// Find the struct
// extract fields
while read x
do
set
