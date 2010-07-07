/***************************************************************************
            mount.c  -  high level functions for backend mounting.
                             -------------------
    begin                : Sat Nov 3
    copyright            : (C) 2007 by Patrick Sabin
    email                : patricksabin@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if DEBUG && HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include "kdbinternal.h"


/**
 * @defgroup mount Interface for mounting backends
 */


/**
 * Creates a trie from a given configuration.
 *
 * The config will be deleted within this function.
 *
 * @param kdb the handle to work with
 * @param modules the current list of loaded modules
 * @param config the configuration which should be used to build up the trie.
 * @param errorKey the key used to report warnings
 * @return -1 on failure
 * @return 0 on success
 * @ingroup mount
 */
int elektraMountOpen(KDB *kdb, KeySet *config, KeySet *modules, Key *errorKey)
{
	Key *root;
	Key *cur;

	ksRewind(config);
	root=ksLookupByName(config, KDB_KEY_MOUNTPOINTS, 0);

	if (!root)
	{
		ELEKTRA_ADD_WARNING(22, errorKey, KDB_KEY_MOUNTPOINTS);
		ksDel (config);
		return -1;
	}

	while ((cur = ksNext(config)) != 0)
	{
		if (keyRel (root, cur) == 1)
		{
			KeySet *cut = ksCut(config, cur);
			Backend *backend = elektraBackendOpen(cut, modules, errorKey);
			if (elektraMountBackend(kdb, backend, errorKey) == -1)
			{
				/* warnings already set by elektraMountBackend */
				ksDel (cut);
			}
		}
	}

	return 0;
}


/** Reopens the default backend and mounts the default backend if needed.
 *
 * @param kdb the handle to work with
 * @param modules the current list of loaded modules
 * @param errorKey the key used to report warnings
 * @return -1 on error
 * @return 0 on success
 * @ingroup mount
 */
int elektraMountDefault (KDB *kdb, KeySet *modules, Key *errorKey)
{
	/* Reopen the default Backend for fresh user experience (update issue) */
	kdb->defaultBackend = elektraBackendOpenDefault(modules, errorKey);

	if (!kdb->defaultBackend)
	{
		ELEKTRA_ADD_WARNING(43, errorKey, "could not reopen default backend");
		return -1;
	}

	/* Trie was created successfully.
	   We want system/elektra/mountpoints still reachable
	   through default backend.
	   First check if it is still reachable.
	*/
	Key *key = keyNew ("system/elektra", KEY_END);
	Backend* backend = kdbGetBackend(kdb, key);
	if (backend != kdb->defaultBackend)
	{
		elektraMountBackend (kdb, kdb->defaultBackend, errorKey);
	}
	keyDel (key);

	return 0;
}


/** Mount all module configurations.
 *
 * @param kdb the handle to work with
 * @param modules the current list of loaded modules
 * @param errorKey the key used to report warnings
 * @ingroup mount
 */
int elektraMountModules (KDB *kdb, KeySet *modules, Key *errorKey)
{
	Key *root;
	Key *cur;

	root=ksLookupByName(modules, "system/elektra/modules", 0);

	if (!root)
	{
		ELEKTRA_ADD_WARNING(23, errorKey, "no root key found for modules");
		return -1;
	}


	while ((cur = ksNext (modules)) != 0)
	{
		Backend * backend = elektraBackendOpenModules(modules, errorKey);
		elektraMountBackend(kdb, backend, errorKey);
	}

	return 0;
}


/**
 * Mounts a backend into the trie.
 *
 * @param kdb the handle to work with
 * @param modules the current list of loaded modules
 * @param errorKey the key used to report warnings
 * @return -1 on failure (free KeySet for backend!)
 * @return 0 when nothing was done
 * @return 1 on success
 * @ingroup mount
 */
int elektraMountBackend (KDB *kdb, Backend *backend, Key *errorKey)
{
	char *mountpoint;

	if (!backend)
	{
		ELEKTRA_ADD_WARNING(24, errorKey, "no backend given to mount");
		return 0;
	}

	if (!backend->mountpoint)
	{
		ELEKTRA_ADD_WARNING(25, errorKey, "no mountpoint");
		elektraBackendClose(backend, errorKey);
		return -1;
	}

	if (!strcmp(keyName(backend->mountpoint), ""))
	{
		/* Mount as root backend */
		mountpoint = elektraStrDup ("");
	} else {
		/* Prepare the name for the mountpoint*/
		mountpoint = elektraMalloc (keyGetNameSize(backend->mountpoint)+1);
		sprintf(mountpoint,"%s/",keyName(backend->mountpoint));
	}
	kdb->trie = elektraTrieInsert(kdb->trie, mountpoint, (void*)backend);
	elektraFree(mountpoint);
	return 1;
}



/**
 * Lookup a mountpoint in a handle for a specific key.
 *
 * Will return a key representing the mountpoint or null
 * if there is no appropriate mountpoint e.g. its the
 * root mountpoint.
 *
 * @par Example:
 * @code
Key * key = keyNew ("system/template");
KDB * handle = kdbOpen();
Key *mountpoint=0;
mountpoint=kdbGetMountpoint(handle, key);

printf("The backend I am using is %s mounted in %s\n",
	keyValue(mountpoint),
	keyName(mountpoint));
kdbClose (handle);
keyDel (key);
 * @endcode
 *
 *
 * @param handle is the data structure, where the mounted directories are saved.
 * @param where the key, that should be looked up.
 * @return the mountpoint associated with the key
 * @ingroup mount
 */
Key* elektraMountGetMountpoint(KDB *handle, const Key *where)
{
	Backend *backend_handle;

	backend_handle=kdbGetBackend(handle,where);
	if (!backend_handle)
	{
		return 0;
	}

	return backend_handle->mountpoint;
}



/**
 * Lookup a backend handle for a specific key.
 *
 * The required canonical name is ensured by using a key as parameter,
 * which will transform the key to canonical representation.
 *
 * Will return handle when no more specific KDB could be
 * found.
 *
 * @param handle is the data structure, where the mounted directories are saved.
 * @param key the key, that should be looked up.
 * @return the backend handle associated with the key
 * @ingroup mount
 */
Backend* kdbGetBackend(KDB *handle, const Key *key)
{
	Backend *ret = elektraTrieLookup(handle->trie, key);
	if (!ret) return handle->defaultBackend;
	return ret;
}


