#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <unistd.h>
#include <glib.h>

GHashTable* hash_config;

void init_dict(FILE* input, GHashTable* hash_config)
{
	int num, i, number;
	char *key, *value;

	fscanf(input,"%d", &num);
	while(num--)
	{
		key = (char *)malloc(20);
		fscanf(input, "%s %d", key, &number);
		GSList *g_list = NULL;
		for (i=0; i<number; i++)
		{
			value = (char *)malloc(50);
			fscanf(input, "%s ", value);
			g_list = g_list_append(g_list, value);
		}
		g_hash_table_insert(hash_config, key, g_list);
	}
	
}

int pipe_config()
{
        pid_t pid;

        int pipe_fdw[2];
		char line[4096];
		FILE* out;

        if (pipe(pipe_fdw) == -1)
        {
			perror("fork failed!/n");
			exit(0);
        }

        pid = fork();

        if (pid == 0)
        {
			if (dup2(pipe_fdw[1], 1) == -1)
			{
				perror("dup failed!/n");
				exit(0);
			}

			close(pipe_fdw[0]);
			close(pipe_fdw[1]);

			if (execl("config/config.py", "config.py", "config/config.txt", NULL) < 0)
			{
				printf("Error!!/n");
				exit(0);
			}
			else
				printf("cal success/n");
        }
        else
        {
			close(pipe_fdw[1]);
			out = fdopen(pipe_fdw[0], "r");
			wait(NULL);

			hash_config = g_hash_table_new(g_str_hash, g_str_equal);
			init_dict(out, hash_config);
			fclose(out);
        }
        return 0;
}


//int main()
//{
//
//	pipe_config();
//			GSList *list = g_hash_table_lookup(hash_config,"block_list");
//			if(!list)
//				printf("fali\n");
//			printf("The list is now %d items long/n", g_slist_length(list));
//			GSList *iterator = NULL;
//			for (iterator = list; iterator; iterator = iterator->next) {
//				        printf("%s ", (char*)iterator->data);
//						    }
//			
//}

