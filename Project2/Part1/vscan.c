// Maximilian Thompson

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

// References.
unsigned long **sys_call_table;

asmlinkage long (*ref_sys_open)(const char *filename, int flags, int mode);
asmlinkage long (*ref_sys_close)(unsigned int filedescriptor);
asmlinkage long (*ref_sys_read)(unsigned int filedescriptor, char *buf, size_t count);

/**************************************************************
 * NEW OPEN
 **************************************************************/

asmlinkage long new_sys_open(const char *filename, int flags, int mode) {
    kuid_t uid = current_uid();
    if (uid.val >= 1000) // If it's a user.
        printk(KERN_INFO "User %u is opening file:  %s\n", uid.val, filename);
    
    return ref_sys_open(filename, flags, mode);
}

/**************************************************************
 * NEW CLOSE
 **************************************************************/

asmlinkage long new_sys_close(unsigned int filedescriptor) {
    kuid_t uid = current_uid();
    if (uid.val >= 1000) // If it's a user.
        printk(KERN_INFO "User %u is closing file descriptor:  %u\n", uid.val, filedescriptor);
    
    return ref_sys_close(filedescriptor);
}

/**************************************************************
 * NEW READ
 **************************************************************/

// Implimentation of naive string search.
int contstr(const char* target, unsigned long tsize, const char* key) {
    unsigned long tindex = 0;

    // Find size of key: (Not including null terminator.)
    unsigned long ksize = 0;
    while (key[ksize] != '\0')
        ksize++;

    if (ksize == 0) return 1; // An empty key will match anything.

    while (tindex < (tsize - ksize)) { // Go until the string couldn't possibly fit.
        for (unsigned int i = 0; target[tindex + i] == key[i]; i++) {
            if (i == ksize) return 1; // All characters of the substring match the key.
        }

        tindex++;
    }

    return 0; // No match was found.
}

asmlinkage long new_sys_read(unsigned int filedescriptor, char *buf, size_t count) {
    long code = ref_sys_read(filedescriptor, buf, count);
    if (contstr(buf, code, "VIRUS")) // If it's a "virus".
        printk(KERN_INFO "User %u read from file descriptor %u, but the read contained malicious code!",
                current_uid().val, filedescriptor);

    return code;
}

/**************************************************************
 * PAGE PROTECTION/FIND TABLE FUNCTIONS
 **************************************************************/

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

/**************************************************************
 * MODULE FUNCTIONS
 **************************************************************/

static int __init interceptor_start(void) {
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table())) {
        /* Well, that didn't work. 
             Cancel the module loading step. */
        return -1;
    }
    
    /* Store a copy of all the existing functions */
    ref_sys_open = (void *)sys_call_table[__NR_open];
    ref_sys_close = (void *)sys_call_table[__NR_close];
    ref_sys_read = (void *)sys_call_table[__NR_read];

    /* Replace the existing system calls */
    disable_page_protection();
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
    sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
    sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
