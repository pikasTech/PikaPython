/* Platform-only resource baseline. */
extern void pika_qemu_exit(int status);

int main(void) {
    pika_qemu_exit(0);
    return 0;
}
