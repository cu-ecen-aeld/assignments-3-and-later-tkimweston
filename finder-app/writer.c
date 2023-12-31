// Author: tkimweston
//
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char filename[128];
	char text[256];
	FILE *f;

	// Open syslog 
	openlog(NULL, 0, LOG_USER);

	// Check 2 arguments were passed
	if (argc < 3) {
		syslog(LOG_ERR, "Invalid Number of arguments: %d", argc);
		closelog();
		exit(1);
	}
	
	// Save filename and text to write
	strcpy(filename, argv[1]);
	strcpy(text, argv[2]);

	f = fopen(filename, "w");
	if (f == NULL) {
		syslog(LOG_ERR, "Cannot open file: %s", filename);
		closelog();
		exit(1);
	}

	fprintf(f, "%s\n", text);
	fclose(f);
	closelog();
	exit(0);
}
