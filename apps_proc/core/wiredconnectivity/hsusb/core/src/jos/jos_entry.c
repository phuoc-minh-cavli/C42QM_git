/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.


==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jos/jos_entry.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "jos_internal.h"
#include "jos_driver.h"
#include "jinit.h"

#ifdef CONFIG_JOTG
# include <jotg_init.h>
#endif

#ifdef CONFIG_JHOST
# include "jhost_init.h"
#endif

#ifdef CONFIG_JSLAVE
# include "jos_int.h"
# include <jslave_init.h>
#endif

#ifdef CONFIG_JOS_TEST
jresult_t jtlow_init(void);
jresult_t jtlow_uninit(void);
jresult_t run_tests(jint_t argc, char **argv);
#endif

#ifdef CONFIG_EM_FS_TEST
jresult_t do_fs_check(void);
#endif

#ifdef CONFIG_TEST
# include <test.h>
#endif

static uw_init_opt_t *init_opts;
static juint8_t num_of_cores;

#ifdef CONFIG_JHOST 
static jbool_t need_host_init(void)
{
    int i;

    for (i = 0; i < num_of_cores; i++)
    {
        if (init_opts[i] == UW_INIT_OTG || init_opts[i] == UW_INIT_HOST ||
            init_opts[i] == UW_INIT_HOST_DEVICE)
        {
            return 1;
        }
    }
    return 0;
}
#endif

#ifdef CONFIG_JSLAVE
static jbool_t need_device_init(void)
{
    int i;

    for (i = 0; i < num_of_cores; i++)
    {
        if (init_opts[i] == UW_INIT_OTG || init_opts[i] == UW_INIT_DEVICE ||
            init_opts[i] == UW_INIT_HOST_DEVICE)
        {
            return 1;
        }
    }
    return 0;
}

#define NEED_DEVICE_ENABLE(hw_core_param) \
    ((hw_core_param.init_opt == UW_INIT_DEVICE || \
     hw_core_param.init_opt == UW_INIT_HOST_DEVICE) && \
     hw_core_param.device_param->auto_enable)
#endif

jresult_t jstart_stack(uw_args_t *args)
{
    jresult_t rc = 0;
    juint8_t i = 0;
#ifndef CONFIG_TEST_PORT
    jbool_t hw_initiliazed = 0;
#endif
#ifdef CONFIG_JOTG 
    jbool_t cotg = 0;
#endif
#ifdef CONFIG_JHOST 
    jbool_t chost = 0;
#endif
#ifdef CONFIG_JSLAVE
    jbool_t cslave = 0;
#endif
#ifdef JDEBUG
    debug_init();
#endif
    
    if (!args)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, no_arguments");
        return JEINVAL;
    }
        
    /* Initialize JOS */
    rc = jos_init(args); 
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_start_jos, rc %u", rc);        
        return rc;
    }

    /* Save it for jstop_stack */
    num_of_cores = args->num_of_cores;
    init_opts = (uw_init_opt_t *)jmalloc(sizeof(uw_init_opt_t) * num_of_cores, 
        M_ZERO);
    if (!init_opts)
    {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, failed_storing_init_options");        
        rc = JENOMEM;
        goto Error;
    }
    for (i = 0; i < num_of_cores; i++)
        init_opts[i] = args->hw_core_params[i].init_opt;

    rc = jdrivers_init();
    if (rc)
        goto Error;

#ifdef CONFIG_JOTG
    /* OTG should always be initialized if compiled */
    rc = jotg_init(args);
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_start_otg, rc %u", rc);        
        goto Error;
    }
    cotg = 1;
#endif

#ifdef CONFIG_JHOST 
    if (need_host_init())
    {
        rc = jhost_init(args);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_start_host, rc %u", rc);            
            goto Error;
        }
        chost = 1;
    }
#endif
    
#ifdef CONFIG_JSLAVE
    if (need_device_init())
    {
        rc = jslave_init(args);
        if (rc)
        {
            HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_start_slave, rc %u", rc);            
            goto Error;
        }
        cslave = 1;
    }
#endif
    
#ifndef CONFIG_TEST_PORT
    rc = jhw_init();
    if (rc)
    {
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_running_hw_init, rc %u", rc);        
        goto Error;
    }
    hw_initiliazed = 1;
#endif

#ifdef CONFIG_JSLAVE
    for (i = 0; i < num_of_cores; i++)
    {
        if (NEED_DEVICE_ENABLE(args->hw_core_params[i]))
        {
            rc = jslave_enable(args->hw_core_params[i].core_number);
            if (rc)
            {
                HSU_ULOG_2(ERROR_MSG, DBG_E_LOG, "DJOS_INIT,"
                  " err_running_jslave_enable, core %u, rc %u", i, rc);                
                goto Error;
            }
        }
    }
#endif

#ifdef CONFIG_TEST_AUTORUN
    rc = test_run(TEST_SUITE, NULL);
#endif

#ifdef CONFIG_EM_FS_TEST
    rc = do_fs_check();
#endif

    DBG_I(DJOS_INIT, ("Stack initialized successfuly\n"));
    return 0;
    
Error:
#ifdef CONFIG_JSLAVE
    for (; i; i--)
    {
        if (!NEED_DEVICE_ENABLE(args->hw_core_params[i]))
            continue;
        jslave_disable(i);
    }
#endif
#ifndef CONFIG_TEST_PORT
    if (hw_initiliazed)
        jhw_uninit();
#endif

#ifdef CONFIG_JSLAVE
    if (cslave)
        jslave_uninit();
#endif
    
#ifdef CONFIG_JHOST 
    if (chost)
        jhost_uninit();
#endif
 
#ifdef CONFIG_JOTG 
    /* OTG should always be un-initialized if compiled */
    if (cotg)
        jotg_uninit();
#endif
 
    jdrivers_uninit();

    if (init_opts)
    {
        jfree(init_opts);
        init_opts = NULL;
        num_of_cores = 0;
    }

    jos_uninit();

    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_start_stack, rc %u", rc);    
    return rc;
}    

void mem_log_print(void);

void jstop_stack(void)
{
    LOCK_CHECK;

#ifdef CONFIG_JSLAVE
    if (need_device_init())
    {
        juint8_t i;

        for (i = 0; i < num_of_cores; i++)
            jslave_disable(i);
    }
#endif
    
#ifndef CONFIG_TEST_PORT
    /* Unload all controllers */
    jhw_uninit();
#endif

#ifdef CONFIG_JOTG 
    jotg_uninit();
#endif    
 
#ifdef CONFIG_JHOST 
    if (need_host_init())
        jhost_uninit();
#endif
    
#ifdef CONFIG_JSLAVE
    if (need_device_init())
        jslave_uninit();
#endif
    
    jdrivers_uninit();

    if (init_opts)
    {
        jfree(init_opts);
        init_opts = NULL;
        num_of_cores = 0;
    }

    jos_uninit();

    /* Print memory leaks */
    mem_log_print();
}

#ifdef CONFIG_TEST_HCI
int add_host_controller(jbus_h bus, char *id);
#endif
jresult_t add_generic_controller_ex(os_bus_h bus, jint_t id, void **handle,
    void *args)
{
    jdevice_t dev;
    jresult_t rc = JENOMEM;

    dev = jdevice_add(NULL);
    if (!dev)
        goto Error;

    j_device_set_ivars(dev, args);
    
    rc = jdriver_attach(DRIVER_CONTROLLER, dev, id, bus);
#ifdef CONFIG_TEST_HCI
    if (rc)
        rc = add_host_controller(bus, "");
#endif
    if (rc)
    {
        jdevice_delete(dev);
        HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DJOS_INIT, err_driver_attach, rc %u", rc);        
        goto Error;
    }

    *handle = dev;

    return 0;

Error:
    return rc;
}

jresult_t add_generic_controller(os_bus_h bus, jint_t id, void **handle)
{
    jresult_t rc;

    CORE_SAFE_ENTER;
    rc = add_generic_controller_ex(bus, id, handle, NULL);
    CORE_SAFE_LEAVE;

    return rc;
}
    
jresult_t generic_controller_suspend(void *handle)
{
    jresult_t rc;
    DBG_I(DJOS_INIT, ("ENTRY: Suspend generic controller %p\n", handle));
    CORE_SAFE_ENTER;
    rc = j_device_suspend((jdevice_t)handle);
    CORE_SAFE_LEAVE;
    return rc;
}

jresult_t generic_controller_resume(void *handle)
{
    jresult_t rc;
    DBG_I(DJOS_INIT, ("ENTRY: Resume generic controller %p\n", handle));
    CORE_SAFE_ENTER;
    rc = j_device_resume((jdevice_t)handle);
    CORE_SAFE_LEAVE;
    return rc;
}

jresult_t del_generic_controller_ex(jdevice_t dev)
{                                                                    
    DBG_I(DJOS_INIT, ("ENTRY: Del generic controller %p\n", dev));
    jdriver_detach(dev);
    jdevice_delete(dev);
    return 0;
}

jresult_t del_generic_controller(void *handle)
{
    CORE_SAFE_ENTER;
    del_generic_controller_ex((jdevice_t)handle);
    CORE_SAFE_LEAVE;
    return 0;
}

