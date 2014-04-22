armasm -g --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi txm_module_preamble.s 
armcc -g -c -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api demo_threadx_module.c
armlink -d -o demo_threadx_module.axf --elf --ro 0 --first txm_module_preamble.o(Init) --entry=_txm_module_thread_shell_entry --ropi --rwpi --remove --map --symbols --list demo_threadx_module.map txm_module_preamble.o demo_threadx_module.o txm.a

