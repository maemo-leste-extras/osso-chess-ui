#include "hash_table.hh"

guint Hash_Table::Hash_Func(gconstpointer key)
{
	const void *raw= key;
	const char *text= String::Get_String(raw);

	guint hash= g_str_hash(text); 

	return hash; 
}

gint
Hash_Table::Compare_Func(gconstpointer a, 
			 gconstpointer b)
{
	const void *raw_a= a, *raw_b= b;
	const char 
		*text_a= String::Get_String(raw_a),
		*text_b= String::Get_String(raw_b);

	return 0 == strcmp(text_a, text_b) ? TRUE : FALSE; 
}

void
Hash_Table::Iterate_Func(gpointer key,
			 gpointer val,
			 gpointer user_data)
{
	String key_string(key);
	String value_string(val);

	Callback &callback= *(Callback *)user_data;
	callback.Call(key_string, value_string);
}

class Hash_Table_Callback
	:  public Hash_Table::Callback
{
public:

	Hash_Table_Callback(Hash_Table &new_hash_table)
		:  hash_table(new_hash_table)
		{
		}

	Hash_Table &hash_table;

	void Call(String key, String value)
		{
			hash_table.Set(key, value);
		}
};

Hash_Table &Hash_Table::
operator = (const Hash_Table &hash_table)
{
	Free();

	if (! Alloc())
		return *this;

	Hash_Table_Callback callback(*this);

	hash_table.Iterate(callback);

	return *this; 
}

void Hash_Table::Set(String key,
		     String value)
{
	assert (key.Is());

	if (table == NULL)
	{
		if (! Alloc())
			return;
	}

	/* remove old entry if present */ 

	gpointer orig_key, orig_value;
	if (TRUE == g_hash_table_lookup_extended
	    (table,
	     key.Get_Raw(),
	     & orig_key,
	     & orig_value))
	{
		g_hash_table_remove(table, orig_key); 
		String::Free(orig_key);
		String::Free(orig_value);
	}

	/* attach new key/value */ 

	void
		*key_raw= key.Detach(),
		*value_raw= value.Detach();
			
	g_hash_table_insert(table,
			    key_raw,
			    value_raw);
}

void Hash_Table::Free()
{
	if (table == NULL)
		return;
	g_hash_table_foreach(table,
			     & Free_Func,
			     NULL);
	table= NULL;
}

bool Hash_Table::Get(String key,
		     String &value) const
{
	if (table == NULL)
		return false;
	void *raw_value= 
		g_hash_table_lookup
		(table,
		 key.Get_Raw());
	if (raw_value == NULL)
		return false;
	String result(raw_value);
	value= result;
	return true;
}


bool Hash_Table::Remove(String key)
{
	if (table == NULL)
		return false;

	gpointer orig_key, orig_value;
	if (TRUE == g_hash_table_lookup_extended
	    (table,
	     key.Get_Raw(),
	     & orig_key,
	     & orig_value))
	{
		g_hash_table_remove(table,
				    orig_key);
		String::Free(orig_key);
		String::Free(orig_value);
		return true; 
	}
	else
		return false;
}

bool Hash_Table::Alloc()
		{
			assert (table == NULL);
			table= g_hash_table_new(& Hash_Func,
						& Compare_Func);
			return table != NULL; 
		}

