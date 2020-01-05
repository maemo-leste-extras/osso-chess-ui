#ifndef HASH_TABLE_INCLUDED
#define HASH_TABLE_INCLUDED

#include "string.hh"
//#include "dbg.hh"

#include <glib.h>

class Hash_Table 
{
public:

	class Callback
	{
	public:

		virtual void Call(String key, String value)= 0;
	};

	Hash_Table()
		:  table(NULL)
		{
		}
	Hash_Table(const Hash_Table &hash_table)
		:  table (NULL)
		{
			*this= hash_table; 
		}
	~Hash_Table()
		{
			Free();
		}

	void Free();

	bool Get(String key,
		 String &value) const;
	/* Return whether there is such a key.  If yes, then VALUE is
	   set. 
	 */

	void Set(String key,
		 String value);

	void Iterate(Callback &callback) const
		{
			if (table == NULL)
				return;
			g_hash_table_foreach(table,
					     & Iterate_Func,
					     & callback);
		}
	bool Remove(String key);
		/* Return value:  whether the key was present. 
		 */
	String Get(String key) const
		{
			String value;
			Get(key, value);
			return value;
		}
	Hash_Table &operator = (const Hash_Table &);

private:

	GHashTable *table;

	bool Alloc();

	static void Free_Func(gpointer key,
			      gpointer value,
			      gpointer /* user_data */ )
		{
			String::Free(key);
			String::Free(value);
		}

	static guint Hash_Func(gconstpointer key);
	static gint Compare_Func(gconstpointer, gconstpointer);
	static void Iterate_Func(gpointer key,
				 gpointer value,
				 gpointer user_data);
};

#endif /* ! HASH_TABLE_INCLUDED */
