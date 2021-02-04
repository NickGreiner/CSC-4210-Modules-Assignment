/* Authors: Nick Greiner, Matt Ray,
 *
 * Description: Adds an entry to /proc allowing the user to print current value of seconds since module was loaded. Use by running "cat /proc/seconds"
 *
 * Compiling: Run the "make" command in the directory where your Makefile and seconds.c are located
 * Installing: From the directory where you ran "make", run "insmod seconds.ko". Make sure the module is loaded using "cat /proc/modules | grep 'seconds'"
 * Removing: From the directory where you ran "make", run "rmmod seconds.ko". Make sure the module is un-loaded using "cat /proc/modules | grep 'seconds'"
 *
 * https://github.com/NickGreiner/CSC-4210-Modules-Assignment
 */


#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

ssize_t seq_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

unsigned long jiffies_start;

static struct proc_ops my_fops={
    .proc_read = seq_read,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{
	/* stores the value of jiffies when module was loaded */
	jiffies_start = jiffies;

	/* creates the /proc/seconds entry */
	proc_create(PROC_NAME, 0666, NULL, &my_fops);
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{
	/* removes the /proc/seconds entry */
	remove_proc_entry(PROC_NAME, NULL);
}

/* This function is called each time /proc/seconds is read */
ssize_t seq_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	int rv = 0;
	char buffer[BUFFER_SIZE];
	static int completed = 0;
	if (completed) {
		completed = 0;
		return 0;
	}
	completed = 1;
	rv = sprintf(buffer, "%lu\n", (jiffies - jiffies_start) / HZ);

	/* copies kernel space buffer to user space usr_buf */
	raw_copy_to_user(usr_buf, buffer, rv);
	return rv;
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Nick Greiner");
