// Stub of Haiku <StorageDefs.h> - entry_ref/node_ref, functional.
#ifndef YELLOW_HAIKU_STUB_STORAGEDEFS_H
#define YELLOW_HAIKU_STUB_STORAGEDEFS_H

#include <SupportDefs.h>
#include <cstring>

struct entry_ref {
	dev_t device;
	ino_t directory;
	char name[1024];

	entry_ref() : device(0), directory(0) { name[0] = '\0'; }
	entry_ref(dev_t dev, ino_t dir, const char* name_);
	bool operator==(const entry_ref& other) const {
		return device == other.device && directory == other.directory && strcmp(name, other.name) == 0;
	}
	bool operator!=(const entry_ref& other) const { return !(*this == other); }
};

inline entry_ref::entry_ref(dev_t dev, ino_t dir, const char* name_)
	: device(dev), directory(dir)
{
	if (name_) {
		strncpy(name, name_, sizeof(name) - 1);
		name[sizeof(name) - 1] = '\0';
	} else {
		name[0] = '\0';
	}
}

struct node_ref {
	dev_t device;
	ino_t node;

	node_ref() : device(0), node(0) {}
	node_ref(dev_t dev, ino_t node_) : device(dev), node(node_) {}
	bool operator==(const node_ref& other) const { return device == other.device && node == other.node; }
	bool operator!=(const node_ref& other) const { return !(*this == other); }
	bool operator<(const node_ref& other) const {
		if (device != other.device) return device < other.device;
		return node < other.node;
	}
};

#endif
