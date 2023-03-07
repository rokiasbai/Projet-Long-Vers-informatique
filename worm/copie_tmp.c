#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 100

int main() {
    char buffer[BUF_SIZE];
    char input_file[] = "input.txt";
    char output_file[] = "/tmp/output.txt";
    int input_fd, output_fd;
    ssize_t bytes_read, bytes_written;

    // Ouvrir le fichier source en lecture
    input_fd = syscall(SYS_open, input_file, O_RDONLY);
    if(input_fd == -1) {
        syscall(SYS_write, STDERR_FILENO, "Erreur: impossible d'ouvrir le fichier source.\n", 46);
        syscall(SYS_exit, 1);
    }

    // Ouvrir le fichier de destination en écriture
    output_fd = syscall(SYS_open, output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(output_fd == -1) {
        syscall(SYS_write, STDERR_FILENO, "Erreur: impossible d'ouvrir le fichier de destination.\n", 55);
        syscall(SYS_exit, 1);
    }

    // Copier le contenu du fichier source dans le fichier de destination
    while ((bytes_read = syscall(SYS_read, input_fd, buffer, BUF_SIZE)) > 0) {
        bytes_written = syscall(SYS_write, output_fd, buffer, bytes_read);
        if(bytes_written != bytes_read) {
            syscall(SYS_write, STDERR_FILENO, "Erreur: impossible d'écrire dans le fichier de destination.\n", 59);
            syscall(SYS_exit, 1);
        }
    }

    if(bytes_read == -1) {
        syscall(SYS_write, STDERR_FILENO, "Erreur: impossible de lire le fichier source.\n", 46);
        syscall(SYS_exit, 1);
    }

    // Fermer les fichiers
    syscall(SYS_close, input_fd);
    syscall(SYS_close, output_fd);

    syscall(SYS_write, STDOUT_FILENO, "Le fichier a été copié avec succès dans ", 40);
    syscall(SYS_write, STDOUT_FILENO, output_file, sizeof(output_file));
    syscall(SYS_write, STDOUT_FILENO, "\n", 1);

    syscall(SYS_exit, 0);
}