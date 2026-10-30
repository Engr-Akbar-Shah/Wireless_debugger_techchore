/***************************************************************************//**
 * @author :  Engineer Akbar Shah
 * @date   :  2024-06-10
 * @file   :  main.c
 * @brief  :        Provides the main firmware entry point and startup sequence.
 ******************************************************************************/


#include "sl_main_init.h"
#include "sl_main_kernel.h"

int main(void)
{
  // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
  sl_main_second_stage_init();

  app_init();

  while (sl_main_start_task_should_continue()) {
    app_process_action();
  }
}
