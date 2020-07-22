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

struct ShmHead {
	ShmHead() {
		memset(guid, 0, sizeof(guid));
		total_size = 0;
		create_time = 0;
		visit_num = 0;
		magic_num = 0;
		memset(reserve, 0, sizeof(reserve));
	}

	char guid[GUID_SIZE + 1];
	size_t total_size;
	time_t create_time;
	uint32_t visit_num;
	uint32_t magic_num;
	char reserve[256];
};

} // namespace smd