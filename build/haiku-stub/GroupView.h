// Stub of Haiku <GroupView.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_GROUPVIEW_H
#define YELLOW_HAIKU_STUB_GROUPVIEW_H

#include <View.h>
#include <Layout.h>

class BGroupView : public BView {
public:
	explicit BGroupView(uint32 flags = 0);
	BGroupView(const char* name, uint32 flags = 0);
	BGroupView(uint32 orientation, float spacing = BLayout::B_USE_DEFAULT_SPACING, uint32 flags = 0);
	virtual ~BGroupView();

	BGroupLayout* GroupLayout() const;
};

#endif
