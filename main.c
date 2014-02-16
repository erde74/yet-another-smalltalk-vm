#include "vm/Heap.h"
#include "vm/Handle.h"
#include "vm/Bootstrap.h"
#include "vm/Snapshot.h"
#include "vm/Entry.h"
#include "vm/Repl.h"
#include "vm/Thread.h"
#include "vm/CompiledCode.h"
#include "vm/Cli.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void bootstrapSmalltalk(char *snapshotFileName, char *bootstrapDir);


int main(int argc, char **args)
{
	CliArgs cliArgs;
	int result = EXIT_SUCCESS;

	parseCliArgs(&cliArgs, argc, args);
	initHeap();
	bootstrapSmalltalk(cliArgs.snapshotFileName, cliArgs.bootstrapDir);

	if (cliArgs.error != NULL) {
		printf(cliArgs.error, cliArgs.operand);
		printf("\n");
		result = EXIT_FAILURE;
	} else if (cliArgs.printHelp) {
		printCliHelp();
	} else if (cliArgs.fileName != NULL) {
		/*result = */parseFileAndInitialize(cliArgs.fileName);
	} else if (cliArgs.eval != NULL) {
		result = asCInt(evalCode(cliArgs.eval));
	} else {
		runRepl();
	}

	freeHandles();
	freeHeap();
	return result;
}



static void bootstrapSmalltalk(char *snapshotFileName, char *bootstrapDir)
{
	FILE *snapshot;
	if (bootstrapDir) {
		snapshot = fopen(snapshotFileName, "w+");
		if (snapshot == NULL) {
			printf("Cannot write to snapshot file: '%s'\n", snapshotFileName);
			exit(EXIT_FAILURE);
		}
		bootstrap(bootstrapDir);
		snapshotWrite(snapshot);
	} else {
		snapshot = fopen(snapshotFileName, "r");
		if (snapshot == NULL) {
			printf("Cannot read snapshot file: '%s'\n", snapshotFileName);
			exit(EXIT_FAILURE);
		}
		snapshotRead(snapshot);
		initThread(&CurrentThread);
	}
	fclose(snapshot);
}
