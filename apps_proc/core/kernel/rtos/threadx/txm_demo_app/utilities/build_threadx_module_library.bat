del txm.a
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_thread_shell_entry.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_callback_request_thread_entry.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call10.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call11.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call12.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call4.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call5.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call6.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call7.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call8.c
armcc -c -g -O0 --cpu Cortex-A7 --apcs /interwork --apcs /ropi --apcs /rwpi -I .\threadx_api txm_module_system_call9.c
armar --create txm.a txm_module_thread_shell_entry.o txm_module_callback_request_thread_entry.o txm_module_system_call10.o txm_module_system_call11.o txm_module_system_call4.o
armar -r txm.a txm_module_system_call5.o txm_module_system_call6.o txm_module_system_call7.o txm_module_system_call8.o txm_module_system_call9.o txm_module_system_call12.o

