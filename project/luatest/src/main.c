/**
 * Copyright (c) 2015 Runtime Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <string.h>
#include <os/os.h>
#include <hal/hal_flash.h>
#include <console/console.h>
#include <shell/shell.h>
#include <elua_base/elua.h>
#include <nffs/nffs.h>
#include <util/flash_map.h>
#ifdef ARCH_sim
#include <mcu/mcu_sim.h>
#endif

/* Init all tasks */
int init_tasks(void);

/* Shell */
#define SHELL_TASK_PRIO      (8)
#define SHELL_TASK_STACK_SIZE (OS_STACK_ALIGN(1024))
static os_stack_t shell_stack[SHELL_TASK_STACK_SIZE];
static struct shell_cmd lua_shell_cmd;

/* NFFS */
#define NFFS_AREA_MAX		16

static int
lua_cmd(int argc, char **argv)
{
    lua_main(argc, argv);
    return 0;
}

static void
create_script_file(void)
{
    char filename[] = "/foobar";
    char script[] = "print \"eat my shorts\"\n";
    struct nffs_file *nf;
    int rc;

    rc = nffs_open(filename, NFFS_ACCESS_READ, &nf);
    if (rc) {
        rc = nffs_open(filename, NFFS_ACCESS_WRITE, &nf);
        assert(rc == 0);
        rc = nffs_write(nf, script, strlen(script));
        assert(rc == 0);
    }
    nffs_close(nf);
}

/**
 * main
 *
 * The main function for the project. This function initializes the os, calls
 * init_tasks to initialize tasks (and possibly other objects), then starts the
 * OS. We should not return from os start.
 *
 * @return int NOTE: this function should never return!
 */
int
main(int argc, char **argv)
{
    int rc;
    struct nffs_area_desc descs[NFFS_AREA_MAX];
    int cnt;

#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    rc = hal_flash_init();
    assert(rc == 0);

    /* Initialize OS */
    os_init();

    /* Init tasks */
    shell_task_init(SHELL_TASK_PRIO, shell_stack, SHELL_TASK_STACK_SIZE);
    console_init(shell_console_rx_cb);

    rc = shell_cmd_register(&lua_shell_cmd, "lua", lua_cmd);
    assert(rc == 0);

    nffs_init();

    rc = flash_area_to_nffs_desc(FLASH_AREA_NFFS, &cnt, descs);
    assert(rc == 0);

    if (nffs_detect(descs) == NFFS_ECORRUPT) {
        rc = nffs_format(descs);
        assert(rc == 0);
    }

    create_script_file();

    /* Start the OS */
    os_start();

    /* os start should never return. If it does, this should be an error */
    assert(0);

    return rc;
}

