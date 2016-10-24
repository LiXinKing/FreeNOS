/*
 * Copyright (C) 2009 Niek Linnenbank
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LstFileCommand.h"
#include <unistd.h>

LstFileCommand::LstFileCommand()
    : ShellCommand("lst", 0)
{
    m_help = "Lst working directory";
}

int LstFileCommand::execute(Size nparams, char **params)
{
    return lstDir(nparams, params);
}


int LstFileCommand::lstDir(Size nparams, char **params)
{
    DIR *d;
    struct dirent *dent;
    struct stat st;
    char path[255], tmp[255];

    /* Grab command-line arguments, if any */
    if (nparams > 1)
    {
        strncpy(path, params[1], sizeof(path));
        path[sizeof(path) - 1] = 0;
    }
    else
    {
        getcwd(path, sizeof(path));        
    }

    refreshMounts(0);

    /* Attempt to open the directory. */
    if (!(d = opendir(path)))
    {
	printf("%s: failed to open '%s': %s\r\n",
		params[0], path, strerror(errno));
	return EXIT_FAILURE;
    }
    /* Read directory. */
    while ((dent = readdir(d)))
    {
	/* Coloring. */
	switch (dent->d_type)
	{
	    case DT_DIR:
		printf("%s", BLUE);
		break;
	
	    case DT_BLK:
	    case DT_CHR:
		printf("%s", YELLOW);
		break;
	
	    case DT_REG:
	    default:
	    
		/* Construct full path. */
		snprintf(tmp, sizeof(tmp),
			 "%s/%s", path, dent->d_name);
		
		/* Is the file executable? */
		if (stat(tmp, &st) != -1 && st.st_mode & 0100)
		{
		    printf("%s", GREEN);
		}
		else
		    printf("%s", WHITE);
	}
	printf("%s ", dent->d_name);
    }
    printf("\r\n");

    /* Close it. */
    closedir(d);
    
    /* Success. */
    return EXIT_SUCCESS;
}

