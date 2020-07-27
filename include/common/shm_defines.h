﻿#pragma once

namespace smd {

enum : unsigned {
	create = 0x01,
	open = 0x02,
};

enum {
	GUID_SIZE = 15,
	MAGIC_NUM = 0x12345678,
};

enum GLOBAL_POINTER {
	GLOBAL_POINTER_ALL_STRINGS = 0,
	GLOBAL_POINTER_ALL_LISTS,
	GLOBAL_POINTER_ALL_MAPS,
	GLOBAL_POINTER_ALL_HASHES,
	GLOBAL_POINTER_MAX,
};

class ShmString;

template <class T>
class ShmList;

template <class T>
class ShmMap;

template <class T>
class ShmHash;


struct ShmHead {
	ShmHead() {
		memset(guid, 0, sizeof(guid));
		total_size	= 0;
		create_time = 0;
		visit_num	= 0;
		magic_num	= 0;
	}

	char	 guid[GUID_SIZE + 1];
	size_t	 total_size;
	time_t	 create_time;
	uint32_t visit_num;
	uint32_t magic_num;

	ShmMap<ShmString>*			allStrings;
	ShmMap<ShmList<ShmString>>* allLists;
	ShmMap<ShmMap<ShmString>>*	allMaps;
	ShmMap<ShmHash<ShmString>>* allHashes;

	char	 reserve[256];
};

} // namespace smd
