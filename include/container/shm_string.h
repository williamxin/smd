﻿#pragma once
#include <string>
#include "shm_obj.h"
#include "pair.h"
#include "../common/utility.h"

namespace smd {

class ShmString : public ShmObj {
public:
	ShmString(Alloc& alloc, size_t size = 0)
		: ShmObj(alloc) {
		resize(GetSuitableCapacity(size + 1));
	}

	ShmString(Alloc& alloc, const std::string& r)
		: ShmObj(alloc) {
		resize(GetSuitableCapacity(r.size() + 1));
		internal_copy(r.data(), r.size());
	}

	ShmString(const ShmString& r)
		: ShmObj(r.m_alloc) {
		resize(GetSuitableCapacity(r.size() + 1));
		internal_copy(r.data(), r.size());
	}

	ShmString& operator=(const std::string& r) {
		ShmString(m_alloc, r).swap(*this);
		return *this;
	}

	ShmString& operator=(const ShmString& r) {
		if (this != &r) {
			ShmString(r).swap(*this);
		}
		return *this;
	}

	~ShmString() {
		resize(0);
		m_size = 0;
	}

	char* data() { return m_ptr.ObjPtr(m_alloc); }
	const char* data() const { return m_ptr.ObjPtr(m_alloc); }
	size_t size() const { return m_size; }
	bool empty() { return m_size > 0; }
	size_t capacity() const { return m_capacity; }

	ShmString& assign(const std::string& r) {
		if (r.size() < m_capacity) {
			internal_copy(r.data(), r.size());
			shrink_to_fit();
		} else {
			resize(GetSuitableCapacity(r.size() + 1));
			internal_copy(r.data(), r.size());
		}

		return *this;
	}

	ShmString& append(const ShmString& str) {
		if (capacity() <= size() + str.size()) {
			resize(GetSuitableCapacity(size() + str.size() + 1));
		}

		internal_append(str.data(), str.size());
		return *this;
	}

	ShmString& append(const std::string& str) {
		if (capacity() <= size() + str.size()) {
			resize(GetSuitableCapacity(size() + str.size() + 1));
		}

		internal_append(str.data(), str.size());
		return *this;
	}

	ShmString& append(const char* s) {
		size_t len = strlen(s);
		if (capacity() <= size() + len) {
			resize(GetSuitableCapacity(size() + len + 1));
		}

		internal_append(s, len);
		return *this;
	}

	ShmString& append(const char* s, size_t n) {
		if (capacity() <= size() + n) {
			resize(GetSuitableCapacity(size() + n + 1));
		}

		internal_append(s, n);
		return *this;
	}

	int compare(const ShmString& b) const {
		const size_t min_len = (size() < b.size()) ? size() : b.size();
		int r = memcmp(data(), b.data(), min_len);
		if (r == 0) {
			if (size() < b.size())
				r = -1;
			else if (size() > b.size())
				r = +1;
		}
		return r;
	}

	void clear() {
		m_size = 0;
		shrink_to_fit();
	}

	std::string ToString() const { return std::string(data(), size()); }

	bool operator==(const ShmString& rhs) const {
		return ((size() == rhs.size()) && (memcmp(data(), rhs.data(), size()) == 0));
	}

private:
	static size_t GetSuitableCapacity(size_t size) {
		if (size <= 16)
			return 16;
		else
			return Utility::NextPowOf2(size);
	}

	void swap(ShmString& x) {
		smd::swap(m_ptr, x.m_ptr);
		smd::swap(m_capacity, x.m_capacity);
		smd::swap(m_size, x.m_size);
	}

	void resize(size_t capacity) {
		if (m_ptr != shm_nullptr) {
			m_alloc.Free(m_ptr, m_capacity);
			m_capacity = 0;
		}

		if (capacity > 0) {
			m_capacity = capacity;
			m_ptr = m_alloc.Malloc<char>(m_capacity);
		}
	}

	void internal_copy(const char* buf, size_t len) {
		m_size = 0;
		internal_append(buf, len);
	}

	void internal_append(const char* buf, size_t len) {
		// 最后有一个0
		assert(m_capacity > m_size + len);
		char* ptr = m_ptr.ObjPtr(m_alloc);
		memcpy(&ptr[m_size], buf, len);
		ptr[len] = '\0';
		m_size = len;
	}

	void shrink_to_fit() {
		// 在此添加代码缩容
	}

private:
	ShmPointer<char> m_ptr = 0;
	size_t m_capacity = 0;
	size_t m_size = 0;
};

inline bool operator!=(const ShmString& x, const ShmString& y) { return !(x == y); }
inline bool operator<(const ShmString& x, const ShmString& y) { return x.compare(y) < 0; }
inline bool operator>(const ShmString& x, const ShmString& y) { return x.compare(y) > 0; }

} // namespace smd

namespace std {
template <>
struct hash<smd::ShmString> {
	typedef smd::ShmString argument_type;
	typedef std::size_t result_type;

	result_type operator()(argument_type const& s) const {
		return std::hash<std::string>()(s.ToString());
	}
};

} // namespace std
