/* Authors: Nick Greiner, Matt Ray, James Botsko
 *
 * Description: Adds an entry to /proc allowing user to print curent value of jiffies. Use by running "cat /proc/jiffies"
 *
 * Compiling: Run the "make" command in the directory where your Makefile and jiffies.c are located
 * Installing: From the directory where you ran "make", run "insmod jiffies.ko". Make sure the module is loaded using "cat /proc/modules | grep 'jiffies'"
 * Removing: From the directory where you ran "make", run "rmmod jiffies.ko". Make sure the module is un-loaded using "cat /proc/modules | grep 'jiffies'"
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
#define PROC_NAME "jiffies"

ssize_t seq_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct proc_ops my_fops={
    .proc_read = seq_read,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{
	/* creates the /proc/jiffies entry */
	proc_create(PROC_NAME, 0666, NULL, &my_fops);
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{
	/* removes the /proc/jiffies entry */
	remove_proc_entry(PROC_NAME, NULL);
}

/* This function is called each time /proc/jiffies is read */
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
	rv = sprintf(buffer, "%lu\n", jiffies);

	/* copies kernel space buffer to user space usr_buf */
	raw_copy_to_user(usr_buf, buffer, rv);
	return rv;
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("Nick Greiner");
