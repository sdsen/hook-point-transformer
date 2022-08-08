#workflow for x-formation
#1)csquery to get list of files that need to be x-formed
#2) Rule to add pkt_cls.h to all files — txl does that?
#3)for each file, run coccinelle and txl and redirect output to same file (this will handle cases where no xdp->tc is needed for some functions)
#4)compile and check can be loaded

function run_pipeline {
    for f in ${allFiles[@]}; do
	echo "FILE: " ${f}
	run_coccinelle ${f}
	#run_txl ${f}
    done

    }

function run_coccinelle {
    file=$1
    echo "[RUN_COCCINELLE] FILE: " ${file}
    spatch --sp-file ${COCCI_FILE} ${file} --in-place --debug
}

function run_txl {
    file=$1
    echo "[RUN_TXL] FILE: " ${file}
    txl -o op.c ${file} ${TXL_FILE}
    mv op.c ${file}
}

function attach_and_check {
    echo "TODO"

}

#TODO: make cmd line args
allFiles=("xdp_prog_kern.c" "af_xdp_kern.c")
COCCI_FILE="xdp-drop.cocci"
TXL_FILE="c.txl.1"

run_pipeline