#!/bin/bash
post_fix="txt"
echo "Please input the absolute path of the folder:"
read Path
for file in $(ls ${Path})
do
        if [ ${file#*.} == ${post_fix} ]
        then
                count=$(wc -l ${Path}"/"${file}  | awk '{print $1}' )

                if grep -q "$USER" ${Path}"/"${file}
                then
                sed -i "${count},${count}d" ${Path}"/"${file}
                fi

                cur_dateTime="`date "+%Y-%m-%d %H:%M:%S"`"
                echo "$USER:${cur_dateTime}">> "${Path}/${file}"
        fi
done