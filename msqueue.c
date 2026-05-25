#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int main(int argc, char *argv[]) {

    key_t key;
    int msgid;

    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s writer|reader\n", argv[0]);
        return 1;
    }

    // Create unique key
    key = ftok("msqueue.c", 65);

    if (key == -1) {
        perror("ftok");
        return 1;
    }

    // Create message queue
    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1) {
        perror("msgget");
        return 1;
    }

    // Writer process
    if (strcmp(argv[1], "writer") == 0) {

        message.mesg_type = 1;

        printf("Enter Message: ");
        fgets(message.mesg_text, sizeof(message.mesg_text), stdin);

        // Remove newline character
        message.mesg_text[strcspn(message.mesg_text, "\n")] = 0;

        if (msgsnd(msgid, &message, sizeof(message), 0) == -1) {
            perror("msgsnd");
            return 1;
        }

        printf("Message sent: %s\n", message.mesg_text);
    }

    // Reader process
    else if (strcmp(argv[1], "reader") == 0) {

        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1) {
            perror("msgrcv");
            return 1;
        }

        printf("Message received: %s\n", message.mesg_text);

        // Destroy message queue
        msgctl(msgid, IPC_RMID, NULL);
    }

    else {
        printf("Invalid argument. Use writer or reader.\n");
        return 1;
    }

    return 0;
}
