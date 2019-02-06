#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall1)(void);
asmlinkage long (*ref_sys_cs3013_syscall2)(void);
asmlinkage ssize_t (*ref_sys_open)(const char *filename, int flags, int mode);
asmlinkage ssize_t (*ref_sys_close)(unsigned int filedescriptor);
asmlinkage ssize_t (*ref_sys_read)(unsigned int filedescriptor, char *buf, size_t count);

/*
 * GOODBYE WORLD
 */

asmlinkage long new_sys_cs3013_syscall1(void) {
    printk(KERN_INFO "\"'Hello world?!' More like 'Goodbye, world!' EXTERMINATE!\" -- Dalek");
    return 0;
}

static unsigned long **find_sys_call_table(void) {
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) {
            printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                (unsigned long) sct);
            return sct;
        }

        offset += sizeof(void *);
    }

    return NULL;
}

/*
 * ANCESTRY
 */

typedef struct {
    pid_t ancestors[10];
    pid_t siblings[100];
    pid_t children[100];
} ancestry_t;

asmlinkage long new_sys_cs3013_syscall2(pid_t* pid, ancestry_t* response) {
    pid_t target;             // The target PID.
    ancestry_t ancestry;      // The ancestry to return.
    struct task_struct* task;
    struct list_head* cur;
    int i;
    
    // Get the target pid.
    //printk(KERN_INFO "ANCESTRY: Retreiving PID");
    if (copy_from_user(&target, pid, sizeof(pid_t)) != 0)
        return -1; // Die if the pid isn't good.

    // Retrieve target struct.
    //printk(KERN_INFO "ANCESTRY: Retreiving response struct");
    if (copy_from_user(&ancestry, response, sizeof(ancestry_t)) != 0)
        return -1; // Die if it didn't work.
    
    // Try to find the targeted PID.
    //printk(KERN_INFO "ANCESTRY: Finding task");
    task = pid_task(find_vpid(target), PIDTYPE_PID);
    if (task == NULL) return -1; // If it wasn't found.

    printk(KERN_INFO "ANCESTRY: Found target: %d\n", target);

    // Get all the children.
    //printk(KERN_INFO "ANCESTRY: Indexing children");
    cur = NULL;
    i = 0;
    list_for_each(cur, &task->children) {
        ancestry.children[i] = list_entry(cur, struct task_struct, sibling)->pid;
        printk(KERN_INFO "ANCESTRY: Found child: %d\n", ancestry.children[i]);
        i++;

        if (i >= 100) break; // Break if we hit the bounds.
    }

    // Get all the siblings.
    //printk(KERN_INFO "ANCESTRY: Indexing siblings");
    cur = NULL;
    i = 0;
    list_for_each(cur, &task->parent->children) {
        ancestry.siblings[i] = list_entry(cur, struct task_struct, sibling)->pid;
        printk(KERN_INFO "ANCESTRY: Found sibling: %d\n", ancestry.siblings[i]);
        i++;

        if (i >= 100) break; // Break if we hit the bounds.
    }

    // Get all the parents.
    //printk(KERN_INFO "ANCESTRY: Indexing ancestors");
    i = 0;
    while (task->parent != NULL && i < 10) {
        task = task->parent;
        ancestry.ancestors[i] = task->pid;
        if (ancestry.ancestors[i] == 0) break; // Break if we're past init.
        printk(KERN_INFO "ANCESTRY: Found ancestor: %d\n", ancestry.ancestors[i]);

        i++;
    }

    //printk(KERN_INFO "ANCESTRY: Returning data");
    // Return the data.
    if (copy_to_user(response, &ancestry, sizeof(ancestry_t)) != 0)
        return -1; // Die if we failed to return everything.

    //printk(KERN_INFO "ANCESTRY: Exiting normally");
    return 0;
}

/*
 * VIRUS SCAN
 */

/**************************************************************
 * NEW OPEN
 **************************************************************/

asmlinkage long new_sys_open(const char *filename, int flags, int mode) {
    ssize_t code = ref_sys_open(filename, flags, mode);
    kuid_t uid = current_uid();
    /* If open returned 0, then we know that the filename string is safe to read
     * otherwise the open call would have errored. */
    if (code >= 0 && uid.val >= 1000) // If it's a user.
        printk(KERN_INFO "User %u is opening file descriptor %u:  %s\n", uid.val, code, filename);
    
    return code;
}

/**************************************************************
 * NEW CLOSE
 **************************************************************/

asmlinkage ssize_t new_sys_close(unsigned int filedescriptor) {
    kuid_t uid = current_uid();
    if (uid.val >= 1000) // If it's a user.
        printk(KERN_INFO "User %u is closing file descriptor:  %u\n", uid.val, filedescriptor);
    
    return ref_sys_close(filedescriptor);
}

/**************************************************************
 * NEW READ
 **************************************************************/

// Implimentation of naive string search.
int contstr(const char* target, ssize_t tsize, const char* key) {
    ssize_t tindex = 0;
    ssize_t i = 0;
    // Also all declarations have to be at the top of the funcion. Is this ANSI C?

    /* While we could totally find the size of the key to slightly shorten the
     * search loop, it wouldn't change the O() complexity, and would just add
     * more potential for bugs because it'd be slightly more complicated to
     * impliment. */

    while (tindex < tsize) { // Go until the string couldn't possibly fit.
        i = 0;
        while (tindex + i < tsize && target[tindex + i] == key[i]) {
            // If the next character would be a null terminator, then the key has been found.
            if (key[i+1] == '\0') return 1;
            // Otherwise keep searching.
            i++;
        }

        tindex++;
    }

    return 0; // No match was found.
}

asmlinkage ssize_t new_sys_read(unsigned int filedescriptor, char *buf, ssize_t count) {
    ssize_t code = ref_sys_read(filedescriptor, buf, count);
    /* If the old read returned 0, we know the buffer is safe because presumably
     * it checks it first. It'd be kind of weird if we had to also check */
    if (code >= 0 && contstr(buf, count, "VIRUS")) // If it's a "virus".
        printk(KERN_INFO "User %u read from file descriptor %u, but the read contained malicious code!",
                current_uid().val, filedescriptor);

    return code;
}

/*
 * INTERCEPTOR FUNCTIONS
 */

static void disable_page_protection(void) {
    /*
     Control Register 0 (cr0) governs how the CPU operates.

     Bit #16, if set, prevents the CPU from writing to memory marked as
     read only. Well, our system call table meets that description.
     But, we can simply turn off this bit in cr0 to allow us to make
     changes. We read in the current value of the register (32 or 64
     bits wide), and AND that with a value where all bits are 0 except
     the 16th bit (using a negation operation), causing the write_cr0
     value to have the 16th bit cleared (with all other bits staying
     the same. We will thus be able to write to the protected memory.

     It's good to be the kernel!
    */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
    /*
     See the above description for cr0. Here, we use an OR to set the 
     16th bit to re-enable write protection on the CPU.
    */
    write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table())) {
        /* Well, that didn't work. 
         Cancel the module loading step. */
        return -1;
    }
  
    /* Store a copy of all the existing functions */
    ref_sys_cs3013_syscall1 = (void *)sys_call_table[__NR_cs3013_syscall1];
    ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
    ref_sys_open = (void *)sys_call_table[__NR_open];
    ref_sys_close = (void *)sys_call_table[__NR_close];
    ref_sys_read = (void *)sys_call_table[__NR_read];

    /* Replace the existing system calls */
    disable_page_protection();

    sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)new_sys_cs3013_syscall1;
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
    sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
    sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
  
    enable_page_protection();
  
    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!");

    return 0;
}

static void __exit interceptor_end(void) {
    /* If we don't know what the syscall table is, don't bother. */
    if(!sys_call_table)
        return;
  
    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)ref_sys_cs3013_syscall1;
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
    sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
    sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
