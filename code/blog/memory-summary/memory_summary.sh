#!/usr/bin/env bash


function dump_os_files()
{
	cat /proc/meminfo > ${TMP_DIR}/meminfo
	cat /proc/vmallocinfo > ${TMP_DIR}/vmallocinfo
	cat /proc/slabinfo > ${TMP_DIR}/slabinfo
}

function get_meminfo_field()
{
	local field=$1
	grep -w ${field} ${TMP_DIR}/meminfo | awk '{print $2}'
}

function calc_vmalloc_size()
{
	local pages=$(cat ${TMP_DIR}/vmallocinfo | grep -E "pages=[[:digit:]]+ vmalloc" | sed -r 's/(.*)(pages=[0-9]+)(.*)/\2/' | awk -F"=" 'BEGIN{sum=0}{sum += $2} END {print sum}')
	expr ${pages} '*' 4 # page to KB
}

function format_print_arg()
{
	local value_k=$1
	local value_m=$(expr ${value_k} '/' 1024)

	echo "${value_k}"
}

function show_summary_mem()
{
## show the overview meminfo
	echo "Meminfo Overview"
	printf "Item\tSize(KB)\n"
	printf "%s\t%s\n" "TotalMemory:" $(format_print_arg ${TOTAL_MEM})
	printf "%s\t%s\n" "FreeMemory:" $(format_print_arg ${FREE_MEM})
	printf "%s\t%s\n" "UsedMemory:" $(format_print_arg ${USED_MEM})
	printf "%s\t%s\n" "HugefsTotal:" $(format_print_arg ${HUGEFS_TOTAL})
	printf "%s\t%s\n" "HugefsFree:" $(format_print_arg ${HUGEFS_FREE})

## show user space memory
	printf "\n"
	printf "%s\t%s\n" "Data+Heap+Stack:" $(format_print_arg ${ANON_MEM})
	printf "%s\t%s\n" "TextMemory:" $(format_print_arg ${TEXT_MEM})
	printf "%s\t%s\n" "Tmpfs+Shmem:" $(format_print_arg ${TMPFS_MEM})
	printf "%s\t%s\n" "DeviceCache:" $(format_print_arg ${BUFFER_MEM})
	printf "%s\t%s\n" "UserSpaceTotal:" $(format_print_arg ${USER_SPACE_TOTAL})

## show kernel space memory
	printf "\n"
	printf "%s\t%s\n" "SlabMemory:" $(format_print_arg ${SLAB_MEM})
	printf "%s\t%s\n" "KernelStack:" $(format_print_arg ${KSTACK_MEM})
	printf "%s\t%s\n" "PageTable:" $(format_print_arg ${PGTABLE_MEM})
	printf "%s\t%s\n" "Vmalloc:" $(format_print_arg ${VMALLOC_MEM})
	printf "%s\t%s\n" "AllocPageSize:" $(format_print_arg ${ALLOC_PAGE_MEM})
	printf "%s\t%s\n" "KernelUsedTotal:" $(format_print_arg ${KERNEL_TOTAL})
}


function show_slab_mem()
{
	printf "\n\n"
	echo "Slabinfo"
	printf "Name\tactive_objs\tnum_objs\tobjsize\ttotalsize(K)\n"
	cat ${TMP_DIR}/slabinfo| sed '1,2d' | awk ' {
	size_k = $3 * $4 / 1024;
	printf $1 "\t" $2 "\t" $3 "\t" $4 "\t" "%u\n",
	size_k
	}' | sort -n -r -k5
}

function show_vmalloc()
{
	printf "\n\m"
	echo "VmallocInfo"
	cat ${TMP_DIR}/vmallocinfo | grep -E "pages=[[:digit:]]+ vmalloc" | while read line
	do
		echo $line | awk '{printf $1 "\t" $2 "\t" $3 "\t" $4 "\t" }'
		echo $line | sed -r 's/(.*)(pages=[0-9]+)(.*)/\2/' | awk -F"=" '{printf $2 "\n"}'
	done | sort -n -r -k5
}

# Dump all /proc and /sys files to tmp dir in order to
# avoid memory used by the process of analysis
TMP_DIR=`mktemp -d /tmp/mem-proc-XXXXXX`
trap `rm -fr {TMP_DIR}` SIGINT SIGTERM
echo 3 > /proc/sys/vm/drop_caches
sleep 5

dump_os_files

######################################################3
TOTAL_MEM=$(get_meminfo_field "MemTotal")
FREE_MEM=$(get_meminfo_field "MemFree")
USED_MEM=$(expr ${TOTAL_MEM} '-' ${FREE_MEM})
HUGEFS_TOTAL_NO=$(get_meminfo_field "HugePages_Total")
HUGEFS_FREE_NO=$(get_meminfo_field "HugePages_Free")
HUGEFS_PSIZE=$(get_meminfo_field "Hugepagesize")
HUGEFS_TOTAL=$(expr ${HUGEFS_TOTAL_NO} \* 2048)
HUGEFS_FREE=$(expr ${HUGEFS_FREE_NO} \* 2048)


ANON_MEM=$(expr $(get_meminfo_field "Active(anon)") '+' $(get_meminfo_field "Inactive(anon)"))
TEXT_MEM=$(get_meminfo_field "Mapped")
TMPFS_MEM=$(get_meminfo_field "Shmem")
CACHE_MEM=$(get_meminfo_field "Cached")
BUFFER_MEM=$(get_meminfo_field "Buffers")
USER_SPACE_TOTAL=$(expr ${ANON_MEM} '+' ${TEXT_MEM} '+' ${BUFFER_MEM})

SLAB_MEM=$(get_meminfo_field "Slab")
KSTACK_MEM=$(get_meminfo_field "KernelStack")
PGTABLE_MEM=$(get_meminfo_field "PageTables")
VMALLOC_MEM=$(calc_vmalloc_size)
KERNEL_USED=$(expr ${SLAB_MEM} '+' ${KSTACK_MEM} '+' ${PGTABLE_MEM} '+' ${VMALLOC_MEM})
ALLOC_PAGE_MEM=$(expr ${TOTAL_MEM} '-' ${FREE_MEM} '-' ${HUGEFS_TOTAL} '-' ${USER_SPACE_TOTAL} '-' ${KERNEL_USED})
KERNEL_TOTAL=$(expr ${KERNEL_USED} '+' ${ALLOC_PAGE_MEM})

show_summary_mem
show_slab_mem
show_vmalloc


rm -fr ${TMP_DIR}
