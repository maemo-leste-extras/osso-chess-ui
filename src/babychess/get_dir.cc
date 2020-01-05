#include "get_dir.hh"

#include "file_name.hh"
//#include "main.hh"
//#include "dbg.hh"
#include "file_test.hh"
//#include "version_number.hh"
#include "format.hh"
//#include "glib.hh"
#include "strdupa.hh"

#include "config.h"

#include <limits.h> 
#include <glib.h>
#include <libgen.h>
#include <unistd.h>

String Get_Dir(Install_Dir id)
{
	//Dbg_Function();

	static String base_dir;
	/* Base installation directory.  Eg: `/usr', `/home/joe'...  */

	if (! base_dir.Is())
	{
		//Dbg_Str(program_invocation_name);

		//String program_name= program_invocation_name; 
		String program_name= "";

		if (program_name[0] != '/') 
			/* argv[0] is no absolute file name */ 
		{
			/* Try to find out absolute file name */

			/* Read `/proc/PID/exe'.  */
			char buffer[PATH_MAX + 1];
			String file_name= Format("/proc/%d/exe", (int)getpid()); 
			//Dbg_Str(file_name());
			int r= readlink(file_name(), buffer, PATH_MAX);
			if (r != -1)
			{
				program_name= String(buffer, r);
				//Dbg_Str(program_name());
			}
			else
			{
				String file_name= "/usr/bin/" + program_name; 
				if (0 == access(file_name(), R_OK))
				{
					base_dir= "/usr";
				}
				else
				{
					file_name= String(g_get_home_dir()) / "bin" / program_name;
					if (0 == access(file_name(), R_OK))
						base_dir= g_get_home_dir(); 
					else
						base_dir= "."; 
				}
			}
		}
		if (! base_dir.Is())
		{
			char *program= strdupa(program_name());
			base_dir= dirname(dirname(program));
		}

		//Dbg_Str(base_dir());
	}

	switch (id)
	{
	default:
		assert(0);
		return ".";

	case id_bin:
	{
		static String bin_dir;
		if (! bin_dir.Is())
			bin_dir= base_dir / "bin";
		return bin_dir;
	}

	case id_install:
	{
		static String install_dir;
		if (! install_dir.Is())
		{
			install_dir= base_dir;
			String baby_dir= install_dir / "share/babychess"; 
			if (File_Is_Dir(baby_dir))
				install_dir= baby_dir / PACKAGE_VERSION; 
				//install_dir= baby_dir / VERSION_NUMBER; 
		}

		//Dbg_Str(install_dir());

		return install_dir;
	}
	}
}
