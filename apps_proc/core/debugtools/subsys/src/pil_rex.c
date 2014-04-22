#include "pil_os.h"
#include "rex.h"

//Todo: many functions needs to be added from pil_os.h similar to pil_qurt.c

void pil_os_sig_init(pil_sig_obj_ptr sig_obj)
{
  PIL_VERIFY(sig_obj != NULL, "pil_os_sig_init: sig_obj is NULL", 0, 0, 0);
  
  //Todo: need to change properly
  /* Rex TCB pointer needed for task synchronization */
  //pil_tcb_ptr = (pil_sig_obj_ptr)rcinit_lookup_rextask("pil_task");
}

  


pil_sigs_type pil_os_sig_wait(pil_sig_obj_ptr sig_obj, pil_sigs_type wait_sigs)
{
  (void)(sig_obj);
  return rex_wait( wait_sigs );
}

pil_sigs_type pil_os_sig_set(pil_sig_obj_ptr sig_obj, pil_sigs_type set_sigs)
{
  //Todo: check if this returns value or not
  return rex_set_sigs( (rex_tcb_type *)sig_obj, set_sigs );
}

void pil_os_sig_clr(pil_sig_obj_ptr sig_obj, pil_sigs_type clr_sigs)
{
  //Todo: check if this returns value or not
  rex_clr_sigs( (rex_tcb_type *)sig_obj, clr_sigs );
}
