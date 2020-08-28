﻿#include <stdio.h>
#include <iostream>
#include "smd.h"
#include "util.h"

void TestPointer(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	smd::ShmPointer<int> shm_ptr = alloc.New<int>();
	int* tmp = &shm_ptr;
	assert(tmp != nullptr);
	*tmp = 10;

	int n = *shm_ptr;
	assert(n == 10);

	alloc.Delete(tmp);
	assert(mem_usage == alloc.GetUsed());
}

void TestShmString(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto temp = alloc.New<smd::ShmString>(16);
	auto s = &temp;
	assert(s->capacity() > 16);
	assert(s->size() == 0);
	assert(s->ToString() == "");

	const char* ori_ptr = s->data();
	s->assign("hello");
	assert(s->size() == strlen("hello"));
	assert(s->ToString() == "hello");

	//发生了扩容
	s->assign("hellohellohellohellohellohellohellohellohellohello");
	assert(s->data() != ori_ptr);
	s->assign("hello");

	// 验证两个对象的创建互不影响
	auto t = &alloc.New<smd::ShmString>(32);

	t->assign("world");
	assert(t->ToString() == "world");
	assert(s->ToString() == "hello");
	alloc.Delete(t);
	assert(t == nullptr);
	assert(s->ToString() == "hello");

	// 验证拷贝构造函数
	t = &alloc.New<smd::ShmString>(*s);
	assert(t->data() != s->data());
	assert(t->ToString() == "hello");
	alloc.Delete(t);
	assert(t == nullptr);
	assert(s->ToString() == "hello");

	// 验证拷贝构造函数
	t = &alloc.New<smd::ShmString>("hello");
	assert(t->data() != s->data());
	assert(t->ToString() == "hello");
	alloc.Delete(t);
	assert(t == nullptr);
	assert(s->ToString() == "hello");

	do {
		// 验证拷贝构造函数
		smd::ShmString t1(*s);
		assert(t1.data() != s->data());
		assert(t1.ToString() == s->ToString());

		// 验证赋值函数
		t1 = "312423232";
		assert(t1.ToString() == std::string("312423232"));
	} while (false);

	// 验证清除函数
	*s = "1234567812345678helloaaaa";
	assert(s->ToString() == "1234567812345678helloaaaa");
	s->clear();
	assert(s->ToString() == "");
	*s = "1234567812345678helloaaaafdsfdsddddddddddddddddddddddddddddddddddddddddddd";
	alloc.Delete(s);
	assert(s == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "String test complete");
}

void TestShmList(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();

	auto l = &alloc.New<smd::ShmList<smd::ShmString>>();

	// 验证在尾部添加元素
	assert(l->size() == 0);
	l->push_back(smd::ShmString("hello"));
	assert(l->size() == 1);
	assert(l->front().ToString() == "hello");
	assert(l->back().ToString() == "hello");

	do {

		// 验证在尾部添加多个元素
		for (int i = 0; i < 100; i++) {
			l->push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(l->size() == 101);
		assert(l->front().ToString() == "hello");
		assert(l->back().ToString() == "TestText99");

		// 验证在尾部删除多个元素
		for (int i = 0; i < 100; i++) {
			l->pop_back();
		}
		assert(l->size() == 1);
		assert(l->front().ToString() == "hello");
		assert(l->back().ToString() == "hello");

		// 验证在头部添加多个元素
		for (int i = 0; i < 100; i++) {
			l->push_front(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(l->size() == 101);
		assert(l->front().ToString() == "TestText99");
		assert(l->back().ToString() == "hello");

		// 验证在头部删除多个元素
		for (int i = 0; i < 100; i++) {
			l->pop_front();
		}
		assert(l->size() == 1);
		assert(l->front().ToString() == "hello");
		assert(l->back().ToString() == "hello");

		// 验证在尾部添加多个元素
		for (int i = 0; i < 100; i++) {
			l->push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(l->size() == 101);
		assert(l->front().ToString() == "hello");
		assert(l->back().ToString() == "TestText99");

		// 验证删除元素
		for (auto it = l->begin(); it != l->end();) {
			if (it->ToString().find("TestText") != std::string::npos) {
				it = l->erase(it);
			} else {
				++it;
			}
		}
		assert(l->size() == 1);
		assert(l->front().ToString() == "hello");
		assert(l->back().ToString() == "hello");
	} while (false);

	do {
		smd::ShmList<smd::ShmString> l1(*l);
		// 验证在尾部添加多个元素
		for (int i = 0; i < 100; i++) {
			l1.push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(l1.size() == 101);
		assert(l1.front().ToString() == "hello");
		assert(l1.back().ToString() == "TestText99");

		smd::ShmList<smd::ShmString> l2;
		l2 = *l;
		// 验证在尾部添加多个元素
		for (int i = 0; i < 100; i++) {
			l2.push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(l2.size() == 101);
		assert(l2.front().ToString() == "hello");
		assert(l2.back().ToString() == "TestText99");
	} while (false);

	l->push_back(smd::ShmString("world"));
	assert(l->size() == 2);
	assert(l->front().ToString() == "hello");
	assert(l->back().ToString() == "world");

	l->push_back(smd::ShmString("will"));
	assert(l->size() == 3);
	assert(l->front().ToString() == "hello");
	assert(l->back().ToString() == "will");

	for (auto it = l->begin(); it != l->end();) {
		if (it->ToString() == "world") {
			it = l->erase(it);
		} else {
			++it;
		}
	}

	alloc.Delete(l);
	assert(l == nullptr);

	assert(mem_usage == alloc.GetUsed());
	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "List test complete");
}

void TestShmListPod(smd::Env* env) {
	struct StMyData {
		uint64_t role_id_;
		int hp_;

		StMyData(uint64_t role_id = 0, int hp = 0)
			: role_id_(role_id)
			, hp_(hp) {}
	};

	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto l = &alloc.New<smd::ShmList<StMyData>>();

	// 验证在尾部添加元素
	assert(l->size() == 0);
	l->push_back(StMyData(7131, 14));
	assert(l->size() == 1);
	auto& element = l->front();

	alloc.Delete(l);
	assert(l == nullptr);
	assert(mem_usage == alloc.GetUsed());
	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "ListPod test complete");
}

void TestShmVector(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto v = &alloc.New<smd::ShmVector<smd::ShmString>>();

	assert(v->size() == 0);
	v->push_back(smd::ShmString("hello"));
	assert(v->size() == 1);
	assert(v->front().ToString() == "hello");
	assert(v->back().ToString() == "hello");

	do {
		smd::ShmVector<smd::ShmString> v1(*v);
		for (int i = 0; i < 100; ++i) {
			v1.push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(v1.size() == 101);
		assert(v1[0].ToString() == "hello");
		assert(v1[100].ToString() == "TestText99");

		for (int i = 0; i < 100; ++i) {
			v1.pop_back();
		}
		assert(v1.size() == 1);
		assert(v1[0].ToString() == "hello");

		for (int i = 0; i < 100; ++i) {
			v1.push_back(smd::ShmString(Util::Text::Format("TestText%02d", i)));
		}
		assert(v1.size() == 101);
		assert(v1[0].ToString() == "hello");
		assert(v1[100].ToString() == "TestText99");

		v1.clear();
		assert(v1.size() == 0);

		v1 = *v;
		assert(v1.size() == 1);
		assert(v1[0].ToString() == "hello");

		v1.resize(300, smd::ShmString("123"));
		assert(v1.size() == 300);
		assert(v1[299].ToString() == "123");

	} while (false);

	v->push_back(smd::ShmString("world"));
	assert(v->size() == 2);
	assert(v->front().ToString() == "hello");
	assert(v->back().ToString() == "world");

	v->push_back(smd::ShmString("will"));
	assert(v->size() == 3);
	assert(v->front().ToString() == "hello");
	assert(v->back().ToString() == "will");

	v->pop_back();
	assert(v->size() == 2);
	assert(v->front().ToString() == "hello");
	assert(v->back().ToString() == "world");

	v->pop_back();
	assert(v->size() == 1);
	assert(v->front().ToString() == "hello");
	assert(v->back().ToString() == "hello");

	v->pop_back();
	assert(v->size() == 0);

	alloc.Delete(v);
	assert(v == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "Vector test complete");
}

void TestShmVectorResize(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto v = &alloc.New<smd::ShmVector<smd::ShmString>>(64);
	v->resize(v->capacity(), smd::ShmString(""));

	alloc.Delete(v);
	assert(v == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "VectorResize test complete");
}

void TestShmVectorPod(smd::Env* env) {
	struct StMyData {
		uint64_t role_id_;
		int hp_;

		StMyData(uint64_t role_id = 0, int hp = 0)
			: role_id_(role_id)
			, hp_(hp) {}
	};

	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto v = &alloc.New<smd::ShmVector<StMyData>>();
	
	// 验证在尾部添加元素
	assert(v->size() == 0);
	v->push_back(StMyData(7131, 14));
	assert(v->size() == 1);
	auto& element = v->front();

	alloc.Delete(v);
	assert(v == nullptr);
	assert(mem_usage == alloc.GetUsed());
	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "VectorPod test complete");
}

void TestHash(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto h = &alloc.New<smd::ShmHash<smd::ShmString>>();

	assert(h->size() == 0);
	h->insert(smd::ShmString("hello"));
	assert(h->size() == 1);
	assert((*h->begin()).ToString() == "hello");
	assert(h->count(smd::ShmString("hello")));
	h->clear();

	h->insert(smd::ShmString("hello"));
	h->insert(smd::ShmString("world"));
	assert(h->size() == 2);
	assert((*h->begin()).ToString() == "hello");
	assert(h->count(smd::ShmString("world")));

	h->insert(smd::ShmString("will"));
	assert(h->size() == 3);
	assert((*h->begin()).ToString() == "hello");
	assert(h->count(smd::ShmString("will")));

	for (auto it = h->begin(); it != h->end(); ++it) {
		env->GetLog().DoLog(smd::Log::LogLevel::kDebug, "%s", it->ToString().data());
	}

	for (auto it = h->begin(); it != h->end();) {
		it = h->erase(it);
	}

	assert(h->find(smd::ShmString("hello")) == h->end());
	assert(h->find(smd::ShmString("world")) == h->end());
	assert(h->find(smd::ShmString("will")) == h->end());

	assert(h->size() == 0);
	alloc.Delete(h);
	assert(h == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "Hash test complete");
}

void TestHashPod(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto h = &alloc.New<smd::ShmHash<uint64_t>>();

	for (int i = 0; i < 10; ++i) {
		h->insert(10000 + i);
	}

	assert(h->size() == 10);
	assert(h->find(10000) != h->end());
	assert(h->find(10008) != h->end());
	assert(h->find(10) == h->end());

	for (auto it = h->begin(); it != h->end(); ++it) {
		env->GetLog().DoLog(smd::Log::LogLevel::kDebug, "%llu", *it);
	}

	for (auto it = h->begin(); it != h->end();) {
		it = h->erase(it);
	}

	assert(h->find(10000) == h->end());
	assert(h->find(10008) == h->end());
	assert(h->find(10) == h->end());

	assert(h->size() == 0);
	alloc.Delete(h);
	assert(h == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "Hash test complete");
}

void TestMapString(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto m = &alloc.New<smd::ShmMap<smd::ShmString, smd::ShmString>>();

	for (int i = 0; i < 10; ++i) {
		Util::Text::Format("TestText%02d", i);
		auto key = smd::ShmString(Util::Text::Format("Key%02d", i));
		auto value = smd::ShmString(Util::Text::Format("Value%02d", i));
		m->insert(smd::make_pair(key, value));
	}

	assert(m->size() == 10);
	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 3))) != m->end());
	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 8))) != m->end());
	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 10))) == m->end());

	for (auto it = m->begin(); it != m->end(); ++it) {
		const auto& k = it->first;
		const auto& v = it->second;
		env->GetLog().DoLog(smd::Log::LogLevel::kDebug, "%s, %s", k.data(), v.data());
	}

	for (auto it = m->begin(); it != m->end();) {
		it = m->erase(it);
	}

	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 10000))) == m->end());
	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 10008))) == m->end());
	assert(m->find(smd::ShmString(Util::Text::Format("Key%02d", 10010))) == m->end());

	assert(m->size() == 0);
	alloc.Delete(m);
	assert(m == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "MapPod test complete");
}

void TestMapPod(smd::Env* env) {
	auto& alloc = env->GetMalloc();
	auto mem_usage = alloc.GetUsed();
	auto m = &alloc.New<smd::ShmMap<uint64_t, uint64_t>>();

	for (int i = 0; i < 10; ++i) {
		m->insert(smd::make_pair(i, i * 3 + 2199));
	}

	assert(m->size() == 10);
	assert(m->find(3) != m->end());
	assert(m->find(8) != m->end());
	assert(m->find(10) == m->end());

	for (auto it = m->begin(); it != m->end(); ++it) {
		const auto& k = it->first;
		const auto& v = it->second;
		env->GetLog().DoLog(smd::Log::LogLevel::kDebug, "%llu, %llu", k, v);
	}

	for (auto it = m->begin(); it != m->end();) {
		it = m->erase(it);
	}

	assert(m->find(0) == m->end());
	assert(m->find(8) == m->end());
	assert(m->find(10) == m->end());

	assert(m->size() == 0);
	m->clear();

	alloc.Delete(m);
	assert(m == nullptr);
	assert(mem_usage == alloc.GetUsed());

	env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "MapPod test complete");
}

int main() {
	auto mgr = new smd::EnvMgr;
	mgr->SetLogLevel(smd::Log::LogLevel::kDebug);
	mgr->SetLogHandler([](smd::Log::LogLevel lv, const char* s) {
		std::string time_now = Util::Time::FormatDateTime(time(nullptr));
		switch (lv) {
		case smd::Log::LogLevel::kError:
			printf("%s Error: %s\n", time_now.c_str(), s);
			break;
		case smd::Log::LogLevel::kWarning:
			printf("%s Warning: %s\n", time_now.c_str(), s);
			break;
		case smd::Log::LogLevel::kInfo:
			printf("%s Info: %s\n", time_now.c_str(), s);
			break;
		case smd::Log::LogLevel::kDebug:
			printf("%s Debug: %s\n", time_now.c_str(), s);
			break;
		default:
			break;
		}
	});

	const std::string GUID("0x1001187fb");
	auto env = mgr->CreateEnv(GUID, 20, smd::kOpenExist);
	//auto env = mgr->CreateEnv(GUID, 20, smd::create);
	assert(env != nullptr);

	int size = env->GetTestString().size();
	env->GetTestString().push_back(smd::ShmString(Util::Text::Format("TestText%02d", size)));

	for (auto it = env->GetTestString().begin(); it != env->GetTestString().end(); ++it) {
		const auto& str = (*it).ToString();
		env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "%s", str.data());
	}


 	TestPointer(env);
	TestShmString(env);
 	TestShmList(env);
  	TestShmListPod(env);
	TestShmVector(env);
	TestShmVectorResize(env);
	TestShmVectorPod(env);
	TestHash(env);
 	TestHashPod(env);
 	TestMapString(env);
 	TestMapPod(env);

// 	std::string key("StartCounter");
// 	smd::Slice value;
// 	if (env->SGet(key, &value)) {
// 		// 如果已经存在
// 		int count = 0;
// 		memcpy(&count, value.data(), value.size());
// 		count++;
// 
// 		env->GetLog().DoLog(smd::Log::LogLevel::kInfo, "%s is %d", key.data(), count);
// 	} else {
// 		// 如果不存在
// 		env->GetLog().DoLog(
// 			smd::Log::LogLevel::kInfo, "first time run");
// 
// 		int count = 1;
// 		value = smd::Slice((const char*)&count, sizeof(count));
// 		env->SSet(key, value);
// 	}

	int n = 0;
	std::cin >> n;
	return 0;
}
